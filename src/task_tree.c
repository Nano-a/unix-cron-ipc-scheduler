#define _POSIX_C_SOURCE 200809L
#include "task_tree.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>   // malloc, free
#include <fcntl.h>    // open, O_RDONLY
#include <unistd.h>   // close, read, write
#include <ctype.h>   // isdigit
#include <dirent.h>   // opendir, readdir, closedir

#define TASK_TIMING_FILENAME  "timing"
#define TASK_CMD_DIRNAME      "cmd"

// Vérifie si un dossier existe
int dir_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        return 1; 
    if (errno == ENOENT)
        return 0;  
    return -1;     
}

// Crée un dossier récursivement (comme mkdir -p)
int create_dir_recursive(char *path, mode_t mode) {
    int res = dir_exists(path);
    if (res == 1) return 0;   
    if (res == -1) return -1; 

    // remonter jusqu’au dossier parent
    char *p = strrchr(path, '/');
    if (p != NULL) {
        *p = '\0';  // couper le dernier composant
        res = create_dir_recursive(path, mode); // créer le parent
        *p = '/';   // remettre le '/'
        if (res == -1) return -1;
    }

    // créer le dossier courant
    if (mkdir(path, mode) < 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    return 0;
}
// -------------------------------------------------------------------
// Crée récursivement le répertoire de base des tâches
// -------------------------------------------------------------------
int init_task_directory(const char *orig_path) {
    char path[512];
    strncpy(path, orig_path, sizeof(path));
    path[sizeof(path)-1] = '\0';
    return create_dir_recursive(path, 0755);
}


// -------------------------------------------------------------------
// Construit le chemin vers le dossier d'une tâche donnée
// Exemple : build_task_dir_path(path, 512, "/tmp/user", 5)
//  → "/tmp/user/erraid/tasks/5"
// -------------------------------------------------------------------
int build_task_dir_path(char *path, size_t path_size, const char *run_dir, uint64_t taskid) {
    int result = snprintf(path, path_size, "%s/tasks/%lu", run_dir, taskid);
    if (result < 0 || result >= (int)path_size) {
        return -1;  // Erreur : chemin trop long
    }
    return 0;
}

// -------------------------------------------------------------------
// Construit le chemin vers un fichier d'une tâche donnée
// Exemple : build_task_path(path, 512, "/tmp/user", 5, "timing")
//  → "/tmp/user/erraid/tasks/5/timing"
// -------------------------------------------------------------------
int build_task_path(char *path, size_t path_size, const char *run_dir,
                    uint64_t taskid, const char *filename) {
    int result = snprintf(path, path_size, "%s/tasks/%lu/%s",
                          run_dir, taskid, filename);
    if (result < 0 || result >= (int)path_size) {
        return -1;
    }
    return 0;
}

static int ensure_directory_exists(const char *path) {
    char mutable_path[MAX_PATH_LEN];

    size_t len = strnlen(path, sizeof(mutable_path));
    if (len == sizeof(mutable_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memcpy(mutable_path, path, len + 1);
    return create_dir_recursive(mutable_path, 0755);
}

static int load_command_from_dir(const char *cmd_dir, command_t **cmd_out) {
    if (!cmd_dir || !cmd_out) {
        errno = EINVAL;
        return -1;
    }
    *cmd_out = NULL;

    char path[MAX_PATH_LEN];
    int fd;
    int len;

    // Lire le type
    len = snprintf(path, sizeof(path), "%s/type", cmd_dir);
    if (len < 0 || len >= (int)sizeof(path)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    uint16_t type;
    if (read_uint16(fd, &type) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    close(fd);

    command_t *cmd = calloc(1, sizeof(command_t));
    if (!cmd) {
        return -1;
    }
    cmd->type = type;

    // Commande simple : lire argv
    if (type == type_from_str("SI")) {
        len = snprintf(path, sizeof(path), "%s/argv", cmd_dir);
        if (len < 0 || len >= (int)sizeof(path)) {
            free(cmd);
            errno = ENAMETOOLONG;
            return -1;
        }
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            free(cmd);
            return -1;
        }
        if (read_arguments(fd, &cmd->argc, &cmd->argv) < 0) {
            int saved = errno;
            close(fd);
            free(cmd);
            errno = saved;
            return -1;
        }
        close(fd);
        cmd->nb_cmds = 0;
        cmd->cmds = NULL;
    } else {
        // Commande complexe : lire les sous-commandes
        cmd->argc = 0;
        cmd->argv = NULL;
        
        // D'abord, trouver le maximum index pour connaître la taille
        DIR *dir = opendir(cmd_dir);
        if (!dir) {
            free(cmd);
            return -1;
        }
        
        uint32_t max_idx = 0;
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            if (strcmp(entry->d_name, "type") == 0) {
                continue;
            }
            // Vérifier que c'est un nombre
            int is_num = 1;
            for (const char *p = entry->d_name; *p; ++p) {
                if (!isdigit((unsigned char)*p)) {
                    is_num = 0;
                    break;
                }
            }
            if (!is_num) {
                continue;
            }
            uint32_t idx = (uint32_t)strtoul(entry->d_name, NULL, 10);
            if (idx > max_idx) {
                max_idx = idx;
            }
        }
        closedir(dir);
        
        // Allouer le tableau avec la bonne taille (max_idx + 1)
        uint32_t count = max_idx + 1;
        command_t **cmds = calloc(count, sizeof(command_t *));
        if (!cmds) {
            free(cmd);
            return -1;
        }
        
        // Parcourir dans l'ordre numérique (0, 1, 2, ...)
        for (uint32_t i = 0; i < count; ++i) {
            len = snprintf(path, sizeof(path), "%s/%u", cmd_dir, i);
            if (len < 0 || len >= (int)sizeof(path)) {
                for (uint32_t j = 0; j < i; ++j) {
                    free_command(cmds[j]);
                }
                free(cmds);
                free(cmd);
                errno = ENAMETOOLONG;
                return -1;
            }
            
            // Vérifier que le répertoire existe
            struct stat st;
            if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode)) {
                // Répertoire manquant, erreur
                for (uint32_t j = 0; j < i; ++j) {
                    free_command(cmds[j]);
                }
                free(cmds);
                free(cmd);
                errno = ENOENT;
                return -1;
            }
            
            if (load_command_from_dir(path, &cmds[i]) < 0) {
                for (uint32_t j = 0; j < i; ++j) {
                    free_command(cmds[j]);
                }
                free(cmds);
                free(cmd);
                return -1;
            }
        }

        cmd->nb_cmds = count;
        cmd->cmds = cmds;
    }

    *cmd_out = cmd;
    return 0;
}

static int save_command_to_dir(const char *cmd_dir, const command_t *cmd) {
    if (!cmd_dir || !cmd) {
        errno = EINVAL;
        return -1;
    }

    if (ensure_directory_exists(cmd_dir) < 0) {
        return -1;
    }

    char path[MAX_PATH_LEN];
    int fd;
    int len;

    // Écrire le type
    len = snprintf(path, sizeof(path), "%s/type", cmd_dir);
    if (len < 0 || len >= (int)sizeof(path)) {
        errno = ENAMETOOLONG;
        return -1;
    }
    fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }
    if (write_uint16(fd, cmd->type) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    close(fd);

    // Commande simple : écrire argv
    if (cmd->type == type_from_str("SI")) {
        len = snprintf(path, sizeof(path), "%s/argv", cmd_dir);
        if (len < 0 || len >= (int)sizeof(path)) {
            errno = ENAMETOOLONG;
            return -1;
        }
        fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd < 0) {
            return -1;
        }
        if (write_arguments(fd, cmd->argc, cmd->argv) < 0) {
            int saved = errno;
            close(fd);
            errno = saved;
            return -1;
        }
        close(fd);
    } else {
        // Commande complexe : écrire les sous-commandes
        for (uint32_t i = 0; i < cmd->nb_cmds; ++i) {
            len = snprintf(path, sizeof(path), "%s/%u", cmd_dir, i);
            if (len < 0 || len >= (int)sizeof(path)) {
                errno = ENAMETOOLONG;
                return -1;
            }
            if (save_command_to_dir(path, cmd->cmds[i]) < 0) {
                return -1;
            }
        }
    }

    return 0;
}

int load_task_from_dir(const char *run_dir, uint64_t taskid, task_t **task_out) {
    if (!run_dir || !task_out) {
        errno = EINVAL;
        return -1;
    }
    *task_out = NULL;

    char path[MAX_PATH_LEN];
    task_t *task = calloc(1, sizeof(task_t));
    if (!task) {
        return -1;
    }
    task->taskid = taskid;

    int fd = -1;

    if (build_task_path(path, sizeof(path), run_dir, taskid, TASK_TIMING_FILENAME) < 0) {
        goto error;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        goto error;
    }
    if (read_timing(fd, &task->timing) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        goto error;
    }
    close(fd);
    fd = -1;

    char cmd_dir_path[MAX_PATH_LEN];
    if (build_task_dir_path(cmd_dir_path, sizeof(cmd_dir_path), run_dir, taskid) < 0) {
        goto error;
    }
    int len = snprintf(path, sizeof(path), "%s/%s", cmd_dir_path, TASK_CMD_DIRNAME);
    if (len < 0 || len >= (int)sizeof(path)) {
        errno = ENAMETOOLONG;
        goto error;
    }

    if (load_command_from_dir(path, &task->cmd) < 0) {
        goto error;
    }

    *task_out = task;
    return 0;

error:
    if (fd >= 0) {
        close(fd);
    }
    free_task(task);
    return -1;
}

int save_task_to_dir(const char *run_dir, const task_t *task) {
    if (!run_dir || !task || !task->cmd) {
        errno = EINVAL;
        return -1;
    }

    char dir_path[MAX_PATH_LEN];
    char file_path[MAX_PATH_LEN];

    if (build_task_dir_path(dir_path, sizeof(dir_path), run_dir, task->taskid) < 0) {
        return -1;
    }
    if (ensure_directory_exists(dir_path) < 0) {
        return -1;
    }

    if (build_task_path(file_path, sizeof(file_path), run_dir, task->taskid, TASK_TIMING_FILENAME) < 0) {
        return -1;
    }
    int fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }
    if (write_timing(fd, &task->timing) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    if (close(fd) < 0) {
        return -1;
    }

    if (build_task_dir_path(dir_path, sizeof(dir_path), run_dir, task->taskid) < 0) {
        return -1;
    }
    int len = snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TASK_CMD_DIRNAME);
    if (len < 0 || len >= (int)sizeof(file_path)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return save_command_to_dir(file_path, task->cmd);
}

void free_task(task_t *task) {
    if (!task) {
        return;
    }
    if (task->cmd) {
        free_command(task->cmd);
    }
    free(task);
}

int append_execution_log(const char *run_dir, uint64_t taskid, int64_t timestamp, uint16_t exitcode) {
    char path[MAX_PATH_LEN];
    if (build_task_path(path, sizeof(path), run_dir, taskid, "times-exitcodes") < 0) {
        return -1;
    }

    int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) {
        return -1;
    }

    if (write_int64(fd, timestamp) < 0 || write_uint16(fd, exitcode) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    return close(fd);
}

static int save_stream_file(const char *run_dir, uint64_t taskid,
                            const char *filename, const char *output, size_t len) {
    char path[MAX_PATH_LEN];
    if (build_task_path(path, sizeof(path), run_dir, taskid, filename) < 0) {
        return -1;
    }

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }
    if (len > 0) {
        ssize_t written = write(fd, output, len);
        if (written != (ssize_t)len) {
            int saved = errno;
            close(fd);
            errno = saved;
            return -1;
        }
    }
    return close(fd);
}

int save_stdout(const char *run_dir, uint64_t taskid, const char *output, size_t len) {
    return save_stream_file(run_dir, taskid, "stdout", output ? output : "", output ? len : 0);
}

int save_stderr(const char *run_dir, uint64_t taskid, const char *output, size_t len) {
    return save_stream_file(run_dir, taskid, "stderr", output ? output : "", output ? len : 0);
}


int read_execution_logs(const char *run_dir, uint64_t taskid,
                        int64_t **timestamps_out, uint16_t **exitcodes_out,
                        uint32_t *nbruns_out) {
    if (!run_dir || !timestamps_out || !exitcodes_out || !nbruns_out) {
        errno = EINVAL;
        return -1;
    }

    *timestamps_out = NULL;
    *exitcodes_out = NULL;
    *nbruns_out = 0;

    char path[MAX_PATH_LEN];
    if (build_task_path(path, sizeof(path), run_dir, taskid, "times-exitcodes") < 0) {
        return -1;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            return 0; // Pas de runs enregistrés
        }
        return -1;
    }

    size_t cap = 8;
    int64_t *timestamps = calloc(cap, sizeof(int64_t));
    uint16_t *exitcodes = calloc(cap, sizeof(uint16_t));
    if (!timestamps || !exitcodes) {
        int saved = errno;
        close(fd);
        free(timestamps);
        free(exitcodes);
        errno = saved;
        return -1;
    }

    uint32_t count = 0;
    while (1) {
        int64_t ts;
        uint16_t ec;
        if (read_int64(fd, &ts) < 0) {
            // Si errno == 0, c'est probablement EOF (read() a retourné 0)
            // Si errno == EINVAL, c'est aussi probablement EOF
            // Dans ces cas, on arrête la boucle normalement
            if (errno == 0 || errno == EINVAL || errno == EBADMSG) {
                break; // EOF probable
            }
            // Autre erreur : on retourne -1
            int saved = errno;
            close(fd);
            free(timestamps);
            free(exitcodes);
            errno = saved;
            return -1;
        }
        if (read_uint16(fd, &ec) < 0) {
            // Même logique : si errno == 0, EINVAL ou EBADMSG, c'est EOF
            if (errno == 0 || errno == EINVAL || errno == EBADMSG) {
                break; // EOF probable
            }
            int saved = errno;
            close(fd);
            free(timestamps);
            free(exitcodes);
            errno = saved;
            return -1;
        }
        if (count == cap) {
            cap *= 2;
            int64_t *tmp_ts = realloc(timestamps, cap * sizeof(int64_t));
            uint16_t *tmp_ec = realloc(exitcodes, cap * sizeof(uint16_t));
            if (!tmp_ts || !tmp_ec) {
                int saved = errno;
                close(fd);
                free(tmp_ts ? tmp_ts : timestamps);
                free(tmp_ec ? tmp_ec : exitcodes);
                errno = saved;
                return -1;
            }
            timestamps = tmp_ts;
            exitcodes = tmp_ec;
        }
        timestamps[count] = ts;
        exitcodes[count] = ec;
        count++;
    }
    close(fd);

    if (count == 0) {
        free(timestamps);
        free(exitcodes);
        return 0;
    }

    *timestamps_out = timestamps;
    *exitcodes_out = exitcodes;
    *nbruns_out = count;
    return 0;
}

static int read_stream_file(const char *run_dir, uint64_t taskid,
                            const char *filename, char **output_out, size_t *len_out) {
    if (!run_dir || !output_out || !len_out) {
        errno = EINVAL;
        return -1;
    }
    *output_out = NULL;
    *len_out = 0;

    char path[MAX_PATH_LEN];
    if (build_task_path(path, sizeof(path), run_dir, taskid, filename) < 0) {
        return -1;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        // Si le fichier n'existe pas (ENOENT), retourner -1 avec errno=ENOENT
        // pour que le démon puisse distinguer "fichier inexistant" de "autre erreur"
        // Le démon utilisera errno pour déterminer si c'est NOT_RUN ou NOT_FOUND
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    if (st.st_size == 0) {
        close(fd);
        return 0;
    }

    char *buf = malloc((size_t)st.st_size + 1);
    if (!buf) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    ssize_t rd = read(fd, buf, (size_t)st.st_size);
    int saved = errno;
    close(fd);
    if (rd != st.st_size) {
        free(buf);
        errno = saved ? saved : EIO;
        return -1;
    }
    buf[st.st_size] = '\0';
    *output_out = buf;
    *len_out = (size_t)st.st_size;
    return 0;
}

int read_stdout(const char *run_dir, uint64_t taskid, char **output_out, size_t *len_out) {
    return read_stream_file(run_dir, taskid, "stdout", output_out, len_out);
}

int read_stderr(const char *run_dir, uint64_t taskid, char **output_out, size_t *len_out) {
    return read_stream_file(run_dir, taskid, "stderr", output_out, len_out);
}
