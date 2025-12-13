#define _POSIX_C_SOURCE 200809L

#include "protocol.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Construit le chemin du FIFO de requêtes.
 */
static int build_request_fifo_path(const char *run_dir, char *path, size_t path_size) {
    int len = snprintf(path, path_size, "%s/erraid-request-pipe", run_dir);
    if (len < 0 || len >= (int)path_size) {
        errno = ENAMETOOLONG;
        return -1;
    }
    return 0;
}

/**
 * Construit le chemin du FIFO de réponses.
 */
static int build_reply_fifo_path(const char *run_dir, char *path, size_t path_size) {
    int len = snprintf(path, path_size, "%s/erraid-reply-pipe", run_dir);
    if (len < 0 || len >= (int)path_size) {
        errno = ENAMETOOLONG;
        return -1;
    }
    return 0;
}

/**
 * Crée un FIFO s'il n'existe pas déjà.
 */
static int create_fifo_if_needed(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        // Le fichier existe déjà
        if (S_ISFIFO(st.st_mode)) {
            // C'est déjà un FIFO, c'est bon
            return 0;
        } else {
            // C'est un fichier normal, erreur
            errno = EEXIST;
            return -1;
        }
    }
    
    // Le fichier n'existe pas, on le crée
    if (mkfifo(path, 0666) < 0) {
        return -1;
    }
    return 0;
}

/**
 * Initialise les tubes nommés (FIFO) pour la communication.
 * Crée les FIFO request et reply dans le répertoire run_dir s'ils n'existent pas.
 *
 * @param run_dir Répertoire de base (ex: /tmp/$USER/erraid)
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int init_pipes(const char *run_dir) {
    if (!run_dir) {
        errno = EINVAL;
        return -1;
    }
    
    char request_path[1024];
    char reply_path[1024];
    
    if (build_request_fifo_path(run_dir, request_path, sizeof(request_path)) < 0) {
        return -1;
    }
    if (build_reply_fifo_path(run_dir, reply_path, sizeof(reply_path)) < 0) {
        return -1;
    }
    
    if (create_fifo_if_needed(request_path) < 0) {
        return -1;
    }
    if (create_fifo_if_needed(reply_path) < 0) {
        return -1;
    }
    
    return 0;
}

/**
 * Ouvre les tubes nommés pour le démon.
 * Le démon lit les requêtes et écrit les réponses.
 *
 * @param run_dir Répertoire de base
 * @param request_fd_out Pointeur qui recevra le descripteur de lecture des requêtes
 * @param reply_fd_out   Pointeur qui recevra le descripteur d'écriture des réponses
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int open_pipes_daemon(const char *run_dir, int *request_fd_out, int *reply_fd_out) {
    if (!run_dir || !request_fd_out || !reply_fd_out) {
        errno = EINVAL;
        return -1;
    }
    
    char request_path[1024];
    char reply_path[1024];
    
    if (build_request_fifo_path(run_dir, request_path, sizeof(request_path)) < 0) {
        return -1;
    }
    if (build_reply_fifo_path(run_dir, reply_path, sizeof(reply_path)) < 0) {
        return -1;
    }
    
    // Le démon ouvre request en lecture (O_RDONLY) et reply en écriture (O_WRONLY)
    // IMPORTANT: Pour les pipes nommés, open() en O_RDONLY BLOQUE jusqu'à ce qu'un client
    // ouvre l'autre extrémité en O_WRONLY. Pour éviter que le démon bloque au démarrage,
    // on utilise O_RDWR (lecture+écriture) qui permet d'ouvrir le pipe sans bloquer.
    // Ensuite, on utilise seulement la partie lecture pour request et écriture pour reply.
    // Note: O_RDWR n'est pas idéal mais c'est la seule façon d'éviter le blocage au démarrage.
    int request_fd = open(request_path, O_RDWR);
    if (request_fd < 0) {
        return -1;
    }
    
    int reply_fd = open(reply_path, O_RDWR);
    if (reply_fd < 0) {
        close(request_fd);
        return -1;
    }
    
    *request_fd_out = request_fd;
    *reply_fd_out = reply_fd;
    return 0;
}

/**
 * Ouvre les tubes nommés pour le client.
 * Le client écrit les requêtes et lit les réponses.
 *
 * @param run_dir Répertoire de base
 * @param request_fd_out Pointeur qui recevra le descripteur d'écriture des requêtes
 * @param reply_fd_out   Pointeur qui recevra le descripteur de lecture des réponses
 * @return 0 en cas de succès, -1 en cas d'erreur (errno positionné)
 */
int open_pipes_client(const char *run_dir, int *request_fd_out, int *reply_fd_out) {
    if (!run_dir || !request_fd_out || !reply_fd_out) {
        errno = EINVAL;
        return -1;
    }
    
    char request_path[1024];
    char reply_path[1024];
    
    if (build_request_fifo_path(run_dir, request_path, sizeof(request_path)) < 0) {
        return -1;
    }
    if (build_reply_fifo_path(run_dir, reply_path, sizeof(reply_path)) < 0) {
        return -1;
    }
    
    // Utiliser O_RDWR pour éviter le blocage et assurer la compatibilité avec le démon
    int request_fd = open(request_path, O_RDWR);
    if (request_fd < 0) {
        // Debug: vérifier si le fichier existe
        struct stat st;
        if (stat(request_path, &st) < 0) {
            errno = ENOENT; // Le fichier n'existe pas
        }
        return -1;
    }
    
    int reply_fd = open(reply_path, O_RDWR);
    if (reply_fd < 0) {
        // Debug: vérifier si le fichier existe
        struct stat st;
        if (stat(reply_path, &st) < 0) {
            errno = ENOENT; // Le fichier n'existe pas
        }
        close(request_fd);
        return -1;
    }
    
    *request_fd_out = request_fd;
    *reply_fd_out = reply_fd;
    return 0;
}

//helpers
static ssize_t local_robust_write(int fd, const void *buf, size_t count) {
    const uint8_t *p = (const uint8_t *)buf;
    size_t left = count;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (w == 0) { errno = EIO; return -1; }
        p += w;
        left -= (size_t)w;
    }
    return (ssize_t)count;
}

static ssize_t local_robust_read(int fd, void *buf, size_t count) {
    uint8_t *p = (uint8_t *)buf;
    size_t left = count;
    while (left > 0) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) { errno = EBADMSG; return -1; }
        p += r;
        left -= (size_t)r;
    }
    return (ssize_t)count;
}

//helpers
static int write_taskid(int fd, uint64_t id) {
    return write_uint64(fd, id);
}
static int read_taskid(int fd, uint64_t *id) {
    return read_uint64(fd, id);
}

// Forward declaration
static int write_command_list_format(int fd, const command_t *cmd);
static int read_command_list_format(int fd, command_t **cmd_out);

//send/receive request
int send_request(int fd, const request_t *req) {
    if (!req) { errno = EINVAL; return -1; }

    if (write_uint16(fd, req->opcode) < 0) return -1;

    switch (req->opcode) {
    case OPCODE_TIMES_EXITCODES:
    case OPCODE_STDOUT:
    case OPCODE_STDERR:
        return write_taskid(fd, req->u.query.taskid);

    case OPCODE_LIST:
    case OPCODE_TERMINATE:
        return 0;

    default:
        errno = EPROTO;
        return -1;
    }
}

int receive_request(int fd, request_t **req_out) {
    if (!req_out) { errno = EINVAL; return -1; }

    request_t *req = calloc(1, sizeof(request_t));
    if (!req) return -1;

    if (read_uint16(fd, &req->opcode) < 0) { free(req); return -1; }

    switch (req->opcode) {
    case OPCODE_TIMES_EXITCODES:
    case OPCODE_STDOUT:
    case OPCODE_STDERR:
        if (read_taskid(fd, &req->u.query.taskid) < 0) goto fail;
        *req_out = req;
        return 0;

    case OPCODE_LIST:
    case OPCODE_TERMINATE:
        *req_out = req;
        return 0;

    default:
        errno = EPROTO;
        goto fail;
    }

fail:
    free(req);
    return -1;
}


int send_response(int fd, const response_t *resp) {
    if (!resp) { errno = EINVAL; return -1; }

    if (write_uint16(fd, resp->anstype) < 0) return -1;

    if (resp->anstype == ANSTYPE_ERROR) {
        return write_uint16(fd, resp->u.error.errcode);
    }

    // ANSTYPE_OK : utiliser opcode_used si disponible pour déterminer le type de réponse
    if (resp->opcode_used == OPCODE_LIST) {
        // LIST : envoyer le nombre de tâches puis chaque tâche
        if (write_uint32(fd, resp->u.list_ok.nbtasks) < 0) return -1;
        for (uint32_t i = 0; i < resp->u.list_ok.nbtasks; ++i) {
            task_t *t = resp->u.list_ok.tasks[i];
            if (!t) { errno = EPROTO; return -1; }
            if (write_uint64(fd, t->taskid) < 0) return -1;
            if (write_timing(fd, &t->timing) < 0) return -1;
            // Utiliser write_command_list_format au lieu de write_command
            if (write_command_list_format(fd, t->cmd) < 0) return -1;
        }
        return 0;
    } else if (resp->opcode_used == OPCODE_TIMES_EXITCODES) {
        // TIMES_EXITCODES : envoyer le nombre d'exécutions puis chaque timestamp/exitcode
        if (write_uint32(fd, resp->u.times_exitcodes_ok.nbruns) < 0) return -1;
        for (uint32_t i = 0; i < resp->u.times_exitcodes_ok.nbruns; ++i) {
            if (write_int64(fd, resp->u.times_exitcodes_ok.timestamps[i]) < 0) return -1;
            if (write_uint16(fd, resp->u.times_exitcodes_ok.exitcodes[i]) < 0) return -1;
        }
        return 0;
    } else if (resp->opcode_used == OPCODE_STDOUT || resp->opcode_used == OPCODE_STDERR) {
        // STDOUT/STDERR : envoyer la longueur puis le contenu
        if (resp->u.output_ok.len > UINT32_MAX) { errno = EOVERFLOW; return -1; }
        if (write_uint32(fd, (uint32_t)resp->u.output_ok.len) < 0) return -1;
        if (resp->u.output_ok.len > 0) {
            if (local_robust_write(fd, resp->u.output_ok.output, resp->u.output_ok.len) != (ssize_t)resp->u.output_ok.len)
                return -1;
        }
        return 0;
    } else if (resp->opcode_used == OPCODE_TERMINATE) {
        // TERMINATE : pas de données supplémentaires
        return 0;
    }
    
    // Fallback : si opcode_used n'est pas défini, utiliser l'ancienne logique
    // ANSTYPE_OK : déterminer le type de réponse en testant les champs dans l'ordre de priorité
    // On teste les pointeurs NULL pour déterminer quel champ de l'union est utilisé
    
    // 1. LIST : tester si tasks est non-NULL
    if (resp->u.list_ok.tasks != NULL) {
        if (write_uint32(fd, resp->u.list_ok.nbtasks) < 0) return -1;
        for (uint32_t i = 0; i < resp->u.list_ok.nbtasks; ++i) {
            task_t *t = resp->u.list_ok.tasks[i];
            if (!t) { errno = EPROTO; return -1; }
            if (write_uint64(fd, t->taskid) < 0) return -1;
            if (write_timing(fd, &t->timing) < 0) return -1;
            if (write_command(fd, t->cmd) < 0) return -1;
        }
        return 0;
    }
    
    // 2. TIMES_EXITCODES : tester si timestamps est non-NULL
    if (resp->u.times_exitcodes_ok.timestamps != NULL) {
        if (write_uint32(fd, resp->u.times_exitcodes_ok.nbruns) < 0) return -1;
        for (uint32_t i = 0; i < resp->u.times_exitcodes_ok.nbruns; ++i) {
            if (write_int64(fd, resp->u.times_exitcodes_ok.timestamps[i]) < 0) return -1;
            if (write_uint16(fd, resp->u.times_exitcodes_ok.exitcodes[i]) < 0) return -1;
        }
        return 0;
    }
    
    // 3. STDOUT/STDERR : tester si output est non-NULL
    if (resp->u.output_ok.output != NULL) {
        if (resp->u.output_ok.len > UINT32_MAX) { errno = EOVERFLOW; return -1; }
        if (write_uint32(fd, (uint32_t)resp->u.output_ok.len) < 0) return -1;
        if (resp->u.output_ok.len > 0) {
            if (local_robust_write(fd, resp->u.output_ok.output, resp->u.output_ok.len) != (ssize_t)resp->u.output_ok.len)
                return -1;
        }
        return 0;
    }
    
    // TERMINATE : pas de données supplémentaires
    if (resp->opcode_used == OPCODE_TERMINATE) {
        return 0;
    }
    
    return 0;
}

/**
 * Reçoit une réponse depuis un descripteur de fichier (FIFO).
 * 
 * NOTE: Cette fonction doit connaître le contexte (opcode de la requête originale)
 * pour déterminer le type de réponse attendu. Pour l'instant, on essaie de deviner
 * en lisant et en testant, mais une meilleure approche serait de passer l'opcode
 * en paramètre.
 * 
 * Pour les jalons 1 et 2, on utilise l'opcode passé en paramètre pour déterminer
 * le type de réponse attendu (LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE).
 */

/**
 * Écrit une commande dans le format utilisé dans les réponses LIST.
 * Ce format est différent de write_command : il n'y a pas de champ 'kind'.
 * Format: TYPE (uint16) puis:
 *   - Si TYPE='SI': ARGC (uint32) puis ARGV
 *   - Sinon (SQ, etc.): NBCMDS (uint32) puis CMDS récursifs
 */
static int write_command_list_format(int fd, const command_t *cmd) {
    if (!cmd) { errno = EINVAL; return -1; }
    
    if (write_uint16(fd, cmd->type) < 0) return -1;
    
    // Commande simple
    if (cmd->argc > 0 && cmd->argv != NULL) {
        if (write_arguments(fd, cmd->argc, cmd->argv) < 0) return -1;
        return 0;
    }
    
    // Commande séquence
    if (cmd->nb_cmds > 0 && cmd->cmds != NULL) {
        if (write_uint32(fd, cmd->nb_cmds) < 0) return -1;
        for (uint32_t i = 0; i < cmd->nb_cmds; ++i) {
            if (!cmd->cmds[i]) { errno = EINVAL; return -1; }
            if (write_command_list_format(fd, cmd->cmds[i]) < 0) return -1;
        }
        return 0;
    }
    
    errno = EINVAL;
    return -1;
}

/**
 * Lit une commande dans le format utilisé dans les réponses LIST.
 * Ce format est différent de read_command : il n'y a pas de champ 'kind'.
 * Format: TYPE (uint16) puis:
 *   - Si TYPE='SI': ARGC (uint32) puis ARGV
 *   - Sinon (SQ, etc.): NBCMDS (uint32) puis CMDS récursifs
 */
static int read_command_list_format(int fd, command_t **cmd_out) {
    if (!cmd_out) { errno = EINVAL; return -1; }
    *cmd_out = NULL;
    
    command_t *cmd = calloc(1, sizeof(command_t));
    if (!cmd) return -1;
    
    // Initialiser tous les champs à zéro (déjà fait par calloc, mais être explicite)
    cmd->type = 0;
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->nb_cmds = 0;
    cmd->cmds = NULL;
    
    if (read_uint16(fd, &cmd->type) < 0) {
        free(cmd);
        return -1;
    }
    
    // Vérifier si c'est une commande simple
    if (cmd->type == type_from_str("SI")) {
        uint32_t argc = 0;
        char **argv = NULL;
        if (read_arguments(fd, &argc, &argv) < 0) {
            free(cmd);
            return -1;
        }
        cmd->argc = argc;
        cmd->argv = argv;
        cmd->nb_cmds = 0;
        cmd->cmds = NULL;
        *cmd_out = cmd;
        return 0;
    }
    
    // Sinon, c'est une combinaison (séquence, etc.)
    uint32_t nb_cmds = 0;
    if (read_uint32(fd, &nb_cmds) < 0) {
        free(cmd);
        return -1;
    }
    if (nb_cmds == 0 || nb_cmds > 10000) {
        free(cmd);
        errno = EPROTO;
        return -1;
    }
    
    command_t **children = calloc(nb_cmds, sizeof(command_t *));
    if (!children) {
        free(cmd);
        return -1;
    }
    
    // Initialiser tous les pointeurs à NULL
    for (uint32_t i = 0; i < nb_cmds; ++i) {
        children[i] = NULL;
    }
    
    for (uint32_t i = 0; i < nb_cmds; ++i) {
        children[i] = NULL;  // S'assurer que c'est NULL avant la lecture
        if (read_command_list_format(fd, &children[i]) < 0) {
            // Libérer les enfants partiellement alloués
            for (uint32_t j = 0; j < i; ++j) {
                if (children && children[j]) {
                    free_command(children[j]);
                    children[j] = NULL;
                }
            }
            if (children) {
                free(children);
                children = NULL;
            }
            if (cmd) {
                free(cmd);
                cmd = NULL;
            }
            return -1;
        }
        // Vérifier que la commande a bien été allouée
        if (!children[i]) {
            // Libérer ce qui a été alloué
            for (uint32_t j = 0; j < i; ++j) {
                if (children[j]) {
                    free_command(children[j]);
                    children[j] = NULL;
                }
            }
            free(children);
            free(cmd);
            errno = ENOMEM;
            return -1;
        }
    }
    
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->nb_cmds = nb_cmds;
    cmd->cmds = children;
    *cmd_out = cmd;
    return 0;
}

int receive_response(int fd, response_t **resp_out, uint16_t opcode) {
    if (!resp_out) { errno = EINVAL; return -1; }

    response_t *resp = calloc(1, sizeof(response_t));
    if (!resp) return -1;
    resp->opcode_used = opcode;  // Mémoriser l'opcode

    if (read_uint16(fd, &resp->anstype) < 0) { free(resp); return -1; }

    if (resp->anstype == ANSTYPE_ERROR) {
        if (read_uint16(fd, &resp->u.error.errcode) < 0) { free(resp); return -1; }
        *resp_out = resp;
        return 0;
    }

    // ANSTYPE_OK : utiliser l'opcode pour déterminer le type de réponse
    if (opcode == OPCODE_TERMINATE) {
        // TERMINATE : pas de données supplémentaires
        resp->opcode_used = opcode;
        *resp_out = resp;
        return 0;
    }
    
    uint32_t v32;
    if (read_uint32(fd, &v32) == 0) {
        if (opcode == OPCODE_LIST) {
            // Réponse LIST
            uint32_t nbtasks = v32;
            if (nbtasks == 0) {
                resp->u.list_ok.nbtasks = 0;
                resp->u.list_ok.tasks = NULL;
                *resp_out = resp;
                return 0;
            }
            if (nbtasks > 0 && nbtasks < 10000) {
                task_t **tasks = calloc(nbtasks, sizeof(task_t *));
                if (!tasks) {
                    errno = ENOMEM;
                    goto fail;
                }
                int ok = 1;
                uint32_t i = 0;
                for (i = 0; i < nbtasks && ok; ++i) {
                    task_t *t = calloc(1, sizeof(task_t));
                    if (!t) { ok = 0; break; }
                    if (read_uint64(fd, &t->taskid) < 0) { free(t); ok = 0; break; }
                    if (read_timing(fd, &t->timing) < 0) { free(t); ok = 0; break; }
                    if (read_command_list_format(fd, &t->cmd) < 0) { free(t); ok = 0; break; }
                    tasks[i] = t;
                }
                if (ok && i == nbtasks) {
                    resp->u.list_ok.nbtasks = nbtasks;
                    resp->u.list_ok.tasks = tasks;
                    *resp_out = resp;
                    return 0;
                }
                for (uint32_t j = 0; j < i; ++j) {
                    if (tasks[j]) {
                        if (tasks[j]->cmd) free_command(tasks[j]->cmd);
                        free(tasks[j]);
                    }
                }
                free(tasks);
                errno = EPROTO;
                goto fail;
            }
            errno = EPROTO;
            goto fail;
        } else if (opcode == OPCODE_TIMES_EXITCODES) {
            // Réponse TIMES_EXITCODES
            uint32_t nbruns = v32;
            if (nbruns == 0) {
                // Liste vide : tâche jamais exécutée
                resp->u.times_exitcodes_ok.nbruns = 0;
                resp->u.times_exitcodes_ok.timestamps = NULL;
                resp->u.times_exitcodes_ok.exitcodes = NULL;
                *resp_out = resp;
                return 0;
            } else if (nbruns > 0 && nbruns <= 1000000) {
                int64_t *timestamps = calloc(nbruns, sizeof(int64_t));
                uint16_t *exitcodes = calloc(nbruns, sizeof(uint16_t));
                if (timestamps && exitcodes) {
                    int ok = 1;
                    for (uint32_t i = 0; i < nbruns && ok; ++i) {
                        if (read_int64(fd, &timestamps[i]) < 0) ok = 0;
                        else if (read_uint16(fd, &exitcodes[i]) < 0) ok = 0;
                    }
                    if (ok) {
                        resp->u.times_exitcodes_ok.nbruns = nbruns;
                        resp->u.times_exitcodes_ok.timestamps = timestamps;
                        resp->u.times_exitcodes_ok.exitcodes = exitcodes;
                        *resp_out = resp;
                        return 0;
                    }
                    free(timestamps);
                    free(exitcodes);
                } else {
                    free(timestamps);
                    free(exitcodes);
                }
            }
            errno = EPROTO;
            goto fail;
        } else if (opcode == OPCODE_STDOUT || opcode == OPCODE_STDERR) {
            // Réponse OUTPUT
            uint32_t len = v32;
            if (len <= (1024 * 1024 * 100)) {
                char *buf = malloc((size_t)len + 1);
                if (buf) {
                    if (len > 0) {
                        if (local_robust_read(fd, buf, len) == (ssize_t)len) {
                            buf[len] = '\0';
                            resp->u.output_ok.output = buf;
                            resp->u.output_ok.len = (size_t)len;
                            *resp_out = resp;
                            return 0;
                        }
                    } else {
                        buf[0] = '\0';
                        resp->u.output_ok.output = buf;
                        resp->u.output_ok.len = 0;
                        *resp_out = resp;
                        return 0;
                    }
                    free(buf);
                }
            }
            errno = EPROTO;
            goto fail;
        } else {
            // Opcode non reconnu ou 0
            errno = EPROTO;
            goto fail;
        }
    } else {
        // On n'a pas pu lire un uint32
        errno = EPROTO;
        goto fail;
    }

fail:
    free(resp);
    return -1;
}

//free
void free_request(request_t *req) {
    // Pour les jalons 1 et 2, aucune allocation à libérer
    // (les requêtes consultatives n'allouent pas de mémoire)
    (void)req;
    free(req);
}

void free_response(response_t *resp) {
    if (!resp) return;

    if (resp->anstype == ANSTYPE_ERROR) {
        // Pas de mémoire à libérer pour les erreurs
    } else if (resp->anstype == ANSTYPE_OK) {
        // Utiliser opcode_used pour savoir quel type de réponse libérer
        if (resp->opcode_used == OPCODE_LIST) {
            for (uint32_t i = 0; i < resp->u.list_ok.nbtasks; ++i) {
                task_t *t = resp->u.list_ok.tasks[i];
                if (!t) continue;
                if (t->cmd) {
                    free_command(t->cmd);
                    t->cmd = NULL;
                }
                free(t);
                t = NULL;
            }
            free(resp->u.list_ok.tasks);
            resp->u.list_ok.tasks = NULL;
        } else if (resp->opcode_used == OPCODE_TIMES_EXITCODES) {
            free(resp->u.times_exitcodes_ok.timestamps);
            resp->u.times_exitcodes_ok.timestamps = NULL;
            if (resp->u.times_exitcodes_ok.exitcodes != NULL) {
                free(resp->u.times_exitcodes_ok.exitcodes);
                resp->u.times_exitcodes_ok.exitcodes = NULL;
            }
        } else if (resp->opcode_used == OPCODE_STDOUT || resp->opcode_used == OPCODE_STDERR) {
            free(resp->u.output_ok.output);
            resp->u.output_ok.output = NULL;
        }
        // TERMINATE : pas de mémoire à libérer
    }

    free(resp);
}