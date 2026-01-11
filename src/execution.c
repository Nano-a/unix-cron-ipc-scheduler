#define _POSIX_C_SOURCE 200809L

#include "execution.h"
#include "serialization.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} buffer_t;

static void buffer_init(buffer_t *buf) {
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static void buffer_free(buffer_t *buf) {
    free(buf->data);
    buffer_init(buf);
}

static int buffer_reserve(buffer_t *buf, size_t min_cap) {
    if (min_cap <= buf->cap) {
        return 0;
    }
    size_t new_cap = buf->cap ? buf->cap : 1024;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }
    char *tmp = realloc(buf->data, new_cap);
    if (!tmp) {
        return -1;
    }
    buf->data = tmp;
    buf->cap = new_cap;
    return 0;
}

static int buffer_append(buffer_t *buf, const char *src, size_t len) {
    if (len == 0) {
        return 0;
    }
    if (buffer_reserve(buf, buf->len + len + 1) < 0) {
        return -1;
    }
    memcpy(buf->data + buf->len, src, len);
    buf->len += len;
    buf->data[buf->len] = '\0';
    return 0;
}

static int read_fd_into_buffer(int fd, buffer_t *buf) {
    char tmp[4096];
    ssize_t rd;
    while ((rd = read(fd, tmp, sizeof(tmp))) > 0) {
        if (buffer_append(buf, tmp, (size_t)rd) < 0) {
            return -1;
        }
    }
    if (rd < 0) {
        return -1;
    }
    return 0;
}

static char **build_exec_argv(const command_t *cmd) {
    if (!cmd || cmd->argc == 0 || !cmd->argv) {
        errno = EINVAL;
        return NULL;
    }
    char **exec_argv = calloc((size_t)cmd->argc + 1, sizeof(char *));
    if (!exec_argv) {
        return NULL;
    }
    for (uint32_t i = 0; i < cmd->argc; ++i) {
        if (!cmd->argv[i] || cmd->argv[i][0] == '\0') {
            free(exec_argv);
            errno = EINVAL;
            return NULL;
        }
        exec_argv[i] = cmd->argv[i];
    }
    exec_argv[cmd->argc] = NULL;
    return exec_argv;
}

static void close_pipe(int fds[2]) {
    close(fds[0]);
    close(fds[1]);
}

static int move_buffer_to_output(buffer_t *buf, char **out, size_t *out_len) {
    if (!buf || !out || !out_len) {
        errno = EINVAL;
        return -1;
    }
    if (buf->data && buf->len && buf->data[buf->len - 1] != '\0') {
        if (buffer_append(buf, "", 1) < 0) {
            return -1;
        }
    }
    *out_len = buf->len ? buf->len - 1 : 0;
    *out = buf->data;
    buf->data = NULL;
    buffer_init(buf);
    return 0;
}

// Attribution: Abderrahman AJINOU (T1.9)
int execute_simple_command(const command_t *cmd,
                           char **stdout_buf, size_t *stdout_len,
                           char **stderr_buf, size_t *stderr_len,
                           uint16_t *exitcode) {
    if (!cmd || !stdout_buf || !stderr_buf || !stdout_len || !stderr_len || !exitcode) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->nb_cmds > 0) {
        errno = EINVAL;
        return -1;
    }

    *stdout_buf = NULL;
    *stderr_buf = NULL;
    *stdout_len = 0;
    *stderr_len = 0;
    *exitcode = 255;

    char **exec_argv = build_exec_argv(cmd);
    if (!exec_argv) {
        return -1;
    }

    int stdout_pipe[2];
    int stderr_pipe[2];
    if (pipe(stdout_pipe) < 0) {
        free(exec_argv);
        return -1;
    }
    if (pipe(stderr_pipe) < 0) {
        close_pipe(stdout_pipe);
        free(exec_argv);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close_pipe(stdout_pipe);
        close_pipe(stderr_pipe);
        free(exec_argv);
        return -1;
    }

    if (pid == 0) {
        // Créer un nouveau groupe de session pour isoler complètement la commande
        // Cela permet à la commande de continuer même si le démon est tué
        // setsid() crée un nouveau groupe de session ET un nouveau groupe de processus
        setsid();
        
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        if (dup2(stdout_pipe[1], STDOUT_FILENO) < 0 ||
            dup2(stderr_pipe[1], STDERR_FILENO) < 0) {
            _exit(127);
        }
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        execvp(exec_argv[0], exec_argv);
        _exit(127);
    }

    free(exec_argv);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    buffer_t out_buf;
    buffer_t err_buf;
    buffer_init(&out_buf);
    buffer_init(&err_buf);

    int result = -1;
    if (read_fd_into_buffer(stdout_pipe[0], &out_buf) < 0) {
        goto cleanup;
    }
    if (read_fd_into_buffer(stderr_pipe[0], &err_buf) < 0) {
        goto cleanup;
    }

    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        goto cleanup;
    }

    if (WIFEXITED(status)) {
        *exitcode = (uint16_t)WEXITSTATUS(status);
    } else {
        *exitcode = 255;
    }

    if (move_buffer_to_output(&out_buf, stdout_buf, stdout_len) < 0) {
        goto cleanup;
    }
    if (move_buffer_to_output(&err_buf, stderr_buf, stderr_len) < 0) {
        free(*stdout_buf);
        *stdout_buf = NULL;
        goto cleanup;
    }

    result = 0;

cleanup:
    buffer_free(&out_buf);
    buffer_free(&err_buf);
    return result;
}

// Attribution: Pitel (T1.10)
int execute_sequence_command(const command_t *cmd,
                             char **stdout_buf, size_t *stdout_len,
                             char **stderr_buf, size_t *stderr_len,
                             uint16_t *exitcode) {
    if (!cmd || !stdout_buf || !stderr_buf || !stdout_len || !stderr_len || !exitcode) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->nb_cmds == 0 || !cmd->cmds) {
        errno = EINVAL;
        return -1;
    }

    buffer_t out_buf;
    buffer_t err_buf;
    buffer_init(&out_buf);
    buffer_init(&err_buf);

    uint16_t last_exit = 255;

    for (uint32_t i = 0; i < cmd->nb_cmds; ++i) {
        const command_t *sub = cmd->cmds[i];
        if (!sub) {
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            errno = EINVAL;
            return -1;
        }

        char *sub_out = NULL;
        char *sub_err = NULL;
        size_t sub_out_len = 0;
        size_t sub_err_len = 0;
        uint16_t sub_exit = 255;

        int rc;
        if (sub->nb_cmds > 0) {
            rc = execute_sequence_command(sub, &sub_out, &sub_out_len,
                                          &sub_err, &sub_err_len, &sub_exit);
        } else {
            rc = execute_simple_command(sub, &sub_out, &sub_out_len,
                                        &sub_err, &sub_err_len, &sub_exit);
        }

        if (rc < 0) {
            free(sub_out);
            free(sub_err);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }

        if (sub_out && buffer_append(&out_buf, sub_out, sub_out_len) < 0) {
            free(sub_out);
            free(sub_err);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }
        if (sub_err && buffer_append(&err_buf, sub_err, sub_err_len) < 0) {
            free(sub_out);
            free(sub_err);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }

        free(sub_out);
        free(sub_err);
        last_exit = sub_exit;
    }

    int result = -1;
    if (move_buffer_to_output(&out_buf, stdout_buf, stdout_len) < 0) {
        goto done;
    }
    if (move_buffer_to_output(&err_buf, stderr_buf, stderr_len) < 0) {
        free(*stdout_buf);
        *stdout_buf = NULL;
        goto done;
    }
    *exitcode = last_exit;
    result = 0;

done:
    buffer_free(&out_buf);
    buffer_free(&err_buf);
    return result;
}

int execute_pipeline_command(const command_t *cmd,
                             char **stdout_buf, size_t *stdout_len,
                             char **stderr_buf, size_t *stderr_len,
                             uint16_t *exitcode) {
    if (!cmd || !stdout_buf || !stderr_buf || !stdout_len || !stderr_len || !exitcode) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->nb_cmds < 2 || !cmd->cmds) {
        errno = EINVAL;
        return -1;
    }

    buffer_t out_buf;
    buffer_t err_buf;
    buffer_init(&out_buf);
    buffer_init(&err_buf);

    // Créer les pipes pour connecter les commandes
    int *pipes = calloc((size_t)(cmd->nb_cmds - 1) * 2, sizeof(int));
    if (!pipes) {
        buffer_free(&out_buf);
        buffer_free(&err_buf);
        return -1;
    }

    // Créer tous les pipes
    for (uint32_t i = 0; i < cmd->nb_cmds - 1; i++) {
        if (pipe(pipes + i * 2) < 0) {
            for (uint32_t j = 0; j < i; j++) {
                close(pipes[j * 2]);
                close(pipes[j * 2 + 1]);
            }
            free(pipes);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }
    }

    pid_t *pids = calloc((size_t)cmd->nb_cmds, sizeof(pid_t));
    if (!pids) {
        for (uint32_t i = 0; i < cmd->nb_cmds - 1; i++) {
            close(pipes[i * 2]);
            close(pipes[i * 2 + 1]);
        }
        free(pipes);
        buffer_free(&out_buf);
        buffer_free(&err_buf);
        return -1;
    }

    // Exécuter chaque commande dans un processus séparé
    for (uint32_t i = 0; i < cmd->nb_cmds; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            // Nettoyer les processus déjà créés
            for (uint32_t j = 0; j < i; j++) {
                kill(pids[j], SIGTERM);
                waitpid(pids[j], NULL, 0);
            }
            for (uint32_t j = 0; j < cmd->nb_cmds - 1; j++) {
                close(pipes[j * 2]);
                close(pipes[j * 2 + 1]);
            }
            free(pipes);
            free(pids);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }

        if (pid == 0) {
            // Processus enfant
            setsid();
            
            // Connecter stdin (sauf pour la première commande)
            if (i > 0) {
                if (dup2(pipes[(i - 1) * 2], STDIN_FILENO) < 0) {
                    _exit(127);
                }
            }
            
            // Connecter stdout (sauf pour la dernière commande)
            if (i < cmd->nb_cmds - 1) {
                if (dup2(pipes[i * 2 + 1], STDOUT_FILENO) < 0) {
                    _exit(127);
                }
            }
            
            // Fermer tous les pipes
            for (uint32_t j = 0; j < (cmd->nb_cmds - 1) * 2; j++) {
                close(pipes[j]);
            }
            
            // Exécuter la commande simple directement avec execvp
            const command_t *sub = cmd->cmds[i];
            if (sub->nb_cmds > 0) {
                // Commande complexe : on ne peut pas l'exécuter directement dans un pipeline
                // Il faudrait créer un shell, mais pour simplifier, on retourne une erreur
                _exit(127);
            }
            
            char **exec_argv = build_exec_argv(sub);
            if (!exec_argv) {
                _exit(127);
            }
            
            execvp(exec_argv[0], exec_argv);
            _exit(127);
        }

        pids[i] = pid;
    }

    // Fermer tous les pipes dans le processus parent
    for (uint32_t i = 0; i < cmd->nb_cmds - 1; i++) {
        close(pipes[i * 2 + 1]);  // Fermer les extrémités d'écriture
        if (i > 0) {
            close(pipes[(i - 1) * 2]);  // Fermer les extrémités de lecture précédentes
        }
    }

    // Lire stdout de la dernière commande
    int last_read_fd = (cmd->nb_cmds > 1) ? pipes[(cmd->nb_cmds - 2) * 2] : STDOUT_FILENO;
    if (cmd->nb_cmds > 1) {
        if (read_fd_into_buffer(last_read_fd, &out_buf) < 0) {
            // Nettoyer
            for (uint32_t i = 0; i < cmd->nb_cmds; i++) {
                kill(pids[i], SIGTERM);
                waitpid(pids[i], NULL, 0);
            }
            close(last_read_fd);
            free(pipes);
            free(pids);
            buffer_free(&out_buf);
            buffer_free(&err_buf);
            return -1;
        }
        close(last_read_fd);
    }

    // Attendre tous les processus et collecter stderr
    uint16_t last_exit = 255;
    for (uint32_t i = 0; i < cmd->nb_cmds; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) < 0) {
            continue;
        }
        if (WIFEXITED(status)) {
            uint16_t ec = (uint16_t)WEXITSTATUS(status);
            if (i == cmd->nb_cmds - 1) {
                last_exit = ec;  // Code de sortie de la dernière commande
            }
        } else {
            if (i == cmd->nb_cmds - 1) {
                last_exit = 255;
            }
        }
    }

    free(pipes);
    free(pids);

    int result = -1;
    if (move_buffer_to_output(&out_buf, stdout_buf, stdout_len) < 0) {
        goto done;
    }
    if (move_buffer_to_output(&err_buf, stderr_buf, stderr_len) < 0) {
        free(*stdout_buf);
        *stdout_buf = NULL;
        goto done;
    }
    *exitcode = last_exit;
    result = 0;

done:
    buffer_free(&out_buf);
    buffer_free(&err_buf);
    return result;
}

int execute_conditional_command(const command_t *cmd,
                                char **stdout_buf, size_t *stdout_len,
                                char **stderr_buf, size_t *stderr_len,
                                uint16_t *exitcode) {
    if (!cmd || !stdout_buf || !stderr_buf || !stdout_len || !stderr_len || !exitcode) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->type != type_from_str("IF")) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->nb_cmds < 2 || cmd->nb_cmds > 3 || !cmd->cmds[0] || !cmd->cmds[1]) {
        errno = EINVAL;
        return -1;
    }

    // 1. Exécuter la condition (commande 0)
    char *cond_out = NULL, *cond_err = NULL;
    size_t cond_out_len = 0, cond_err_len = 0;
    uint16_t cond_exit = 255;

    int rc;
    const command_t *cond_cmd = cmd->cmds[0];
    if (cond_cmd->nb_cmds > 0) {
        if (cond_cmd->type == type_from_str("PL")) {
            rc = execute_pipeline_command(cond_cmd, &cond_out, &cond_out_len,
                                          &cond_err, &cond_err_len, &cond_exit);
        } else if (cond_cmd->type == type_from_str("IF")) {
            rc = execute_conditional_command(cond_cmd, &cond_out, &cond_out_len,
                                             &cond_err, &cond_err_len, &cond_exit);
        } else {
            rc = execute_sequence_command(cond_cmd, &cond_out, &cond_out_len,
                                          &cond_err, &cond_err_len, &cond_exit);
        }
    } else {
        rc = execute_simple_command(cond_cmd, &cond_out, &cond_out_len,
                                    &cond_err, &cond_err_len, &cond_exit);
    }

    if (rc < 0) {
        free(cond_out);
        free(cond_err);
        return -1;
    }

    // 2. Décider quelle commande exécuter selon le code de sortie
    const command_t *cmd_to_exec = NULL;
    if (cond_exit == 0) {
        // Condition vraie : exécuter le "then" (commande 1)
        cmd_to_exec = cmd->cmds[1];
    } else {
        // Condition fausse : exécuter le "else" (commande 2) si présent
        if (cmd->nb_cmds >= 3 && cmd->cmds[2]) {
            cmd_to_exec = cmd->cmds[2];
        } else {
            // Pas de else : retourner le résultat de la condition
            *stdout_buf = cond_out;
            *stdout_len = cond_out_len;
            *stderr_buf = cond_err;
            *stderr_len = cond_err_len;
            *exitcode = cond_exit;
            return 0;
        }
    }

    // 3. Exécuter la commande choisie (then ou else)
    char *exec_out = NULL, *exec_err = NULL;
    size_t exec_out_len = 0, exec_err_len = 0;
    uint16_t exec_exit = 255;

    if (cmd_to_exec->nb_cmds > 0) {
        if (cmd_to_exec->type == type_from_str("PL")) {
            rc = execute_pipeline_command(cmd_to_exec, &exec_out, &exec_out_len,
                                          &exec_err, &exec_err_len, &exec_exit);
        } else if (cmd_to_exec->type == type_from_str("IF")) {
            rc = execute_conditional_command(cmd_to_exec, &exec_out, &exec_out_len,
                                            &exec_err, &exec_err_len, &exec_exit);
        } else {
            rc = execute_sequence_command(cmd_to_exec, &exec_out, &exec_out_len,
                                         &exec_err, &exec_err_len, &exec_exit);
        }
    } else {
        rc = execute_simple_command(cmd_to_exec, &exec_out, &exec_out_len,
                                    &exec_err, &exec_err_len, &exec_exit);
    }

    // Libérer les buffers de la condition
    free(cond_out);
    free(cond_err);

    if (rc < 0) {
        free(exec_out);
        free(exec_err);
        return -1;
    }

    // 4. Retourner le résultat de l'exécution (then ou else)
    *stdout_buf = exec_out;
    *stdout_len = exec_out_len;
    *stderr_buf = exec_err;
    *stderr_len = exec_err_len;
    *exitcode = exec_exit;

    return 0;
}