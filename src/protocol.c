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
    int len = snprintf(path, path_size, "%s/request", run_dir);
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
    int len = snprintf(path, path_size, "%s/reply", run_dir);
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
    int request_fd = open(request_path, O_RDONLY);
    if (request_fd < 0) {
        return -1;
    }
    
    int reply_fd = open(reply_path, O_WRONLY);
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
    
    // Le client ouvre request en écriture (O_WRONLY) et reply en lecture (O_RDONLY)
    int request_fd = open(request_path, O_WRONLY);
    if (request_fd < 0) {
        return -1;
    }
    
    int reply_fd = open(reply_path, O_RDONLY);
    if (reply_fd < 0) {
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

//send/receive request
int send_request(int fd, const request_t *req) {
    if (!req) { errno = EINVAL; return -1; }

    if (write_uint16(fd, req->opcode) < 0) return -1;

    switch (req->opcode) {
    case OPCODE_CREATE:
        if (write_timing(fd, &req->u.create.timing) < 0) return -1;
        if (write_uint32(fd, req->u.create.argc) < 0) return -1;
        for (uint32_t i = 0; i < req->u.create.argc; ++i) {
            if (write_string(fd, req->u.create.argv[i]) < 0) return -1;
        }
        return 0;

    case OPCODE_COMBINE:
        if (write_timing(fd, &req->u.combine.timing) < 0) return -1;
        if (write_uint16(fd, req->u.combine.type) < 0) return -1;
        if (write_uint32(fd, req->u.combine.nbtasks) < 0) return -1;
        for (uint32_t i = 0; i < req->u.combine.nbtasks; ++i) {
            if (write_taskid(fd, req->u.combine.taskids[i]) < 0) return -1;
        }
        return 0;

    case OPCODE_REMOVE:
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
    case OPCODE_CREATE: {
        if (read_timing(fd, &req->u.create.timing) < 0) goto fail;
        if (read_uint32(fd, &req->u.create.argc) < 0) goto fail;
        if (req->u.create.argc > 0) {
            req->u.create.argv = calloc(req->u.create.argc, sizeof(char *));
            if (!req->u.create.argv) goto fail;
            for (uint32_t i = 0; i < req->u.create.argc; ++i) {
                if (read_string(fd, &req->u.create.argv[i]) < 0) {
                    // Libérer ce qu'on a déjà alloué
                    for (uint32_t j = 0; j < i; ++j) free(req->u.create.argv[j]);
                    free(req->u.create.argv);
                    goto fail;
                }
            }
        } else {
            req->u.create.argv = NULL;
        }
        *req_out = req;
        return 0;
    }
    case OPCODE_COMBINE: {
        if (read_timing(fd, &req->u.combine.timing) < 0) goto fail;
        if (read_uint16(fd, &req->u.combine.type) < 0) goto fail;
        if (read_uint32(fd, &req->u.combine.nbtasks) < 0) goto fail;
        uint32_t n = req->u.combine.nbtasks;
        req->u.combine.taskids = calloc(n, sizeof(uint64_t));
        if (!req->u.combine.taskids) goto fail;
        for (uint32_t i = 0; i < n; ++i) {
            if (read_taskid(fd, &req->u.combine.taskids[i]) < 0) {
                free(req->u.combine.taskids);
                goto fail;
            }
        }
        *req_out = req;
        return 0;
    }
    case OPCODE_REMOVE:
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
    
    // 4. CREATE/COMBINE : tester si taskid est non-zéro (mais 0 est valide, donc on vérifie différemment)
    // Pour CREATE/COMBINE, on envoie toujours le taskid si les autres champs sont NULL
    // Note: cette logique suppose que l'appelant remplit correctement les champs
    if (resp->u.create_ok.taskid != 0 || 
        (resp->u.list_ok.tasks == NULL && 
         resp->u.times_exitcodes_ok.timestamps == NULL && 
         resp->u.output_ok.output == NULL)) {
        // C'est soit CREATE/COMBINE OK, soit REMOVE/TERMINATE OK (taskid = 0)
        return write_uint64(fd, resp->u.create_ok.taskid);
    }

    // 5. REMOVE/TERMINATE OK : pas de données supplémentaires (déjà géré ci-dessus avec taskid = 0)
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
 * Pour l'instant, on utilise une heuristique :
 * - Si on peut lire un uint64, c'est CREATE/COMBINE OK ou REMOVE/TERMINATE OK
 * - Si on peut lire un uint32, on teste si c'est LIST, TIMES_EXITCODES, ou OUTPUT
 */
int receive_response(int fd, response_t **resp_out) {
    if (!resp_out) { errno = EINVAL; return -1; }

    response_t *resp = calloc(1, sizeof(response_t));
    if (!resp) return -1;

    if (read_uint16(fd, &resp->anstype) < 0) { free(resp); return -1; }

    if (resp->anstype == ANSTYPE_ERROR) {
        if (read_uint16(fd, &resp->u.error.errcode) < 0) { free(resp); return -1; }
        *resp_out = resp;
        return 0;
    }

    // ANSTYPE_OK : on doit déterminer le type de réponse
    // On utilise une heuristique basée sur ce qu'on peut lire
    
    // Stratégie : on essaie de lire un uint32 d'abord
    // Si ça marche, c'est soit LIST (nbtasks), TIMES_EXITCODES (nbruns), ou OUTPUT (len)
    // Si ça échoue, on essaie uint64 (CREATE/COMBINE/REMOVE/TERMINATE)
    
    // Sauvegarder la position pour pouvoir revenir en arrière si nécessaire
    // Note: on ne peut pas vraiment "rewind" un FIFO, donc on doit deviner intelligemment
    
    // On lit un uint32 et on teste différentes interprétations
    uint32_t v32;
    if (read_uint32(fd, &v32) == 0) {
        // On a lu un uint32, ça peut être :
        // 1. LIST: nbtasks suivi de nbtasks tâches
        // 2. TIMES_EXITCODES: nbruns suivi de nbruns (timestamp + exitcode)
        // 3. OUTPUT: len suivi de len octets
        
        // Heuristique: si nbtasks/nbruns/len est raisonnable, on essaie de lire
        // On teste LIST d'abord (le plus probable pour le Jalon 2)
        
        uint32_t nbtasks = v32;
        if (nbtasks > 0 && nbtasks < 1000000) {
            // Essayer LIST: lire nbtasks tâches
            task_t **tasks = calloc(nbtasks, sizeof(task_t *));
            if (tasks) {
                int ok = 1;
                for (uint32_t i = 0; i < nbtasks && ok; ++i) {
                    task_t *t = calloc(1, sizeof(task_t));
                    if (!t) { ok = 0; break; }
                    if (read_uint64(fd, &t->taskid) < 0) { free(t); ok = 0; break; }
                    if (read_timing(fd, &t->timing) < 0) { free(t); ok = 0; break; }
                    if (read_command(fd, &t->cmd) < 0) { free(t); ok = 0; break; }
                    tasks[i] = t;
                }
                if (ok) {
                    resp->u.list_ok.nbtasks = nbtasks;
                    resp->u.list_ok.tasks = tasks;
                    *resp_out = resp;
                    return 0;
                }
                // Échec, libérer ce qu'on a alloué
                for (uint32_t j = 0; j < nbtasks; ++j) {
                    if (tasks[j]) {
                        if (tasks[j]->cmd) free_command(tasks[j]->cmd);
                        free(tasks[j]);
                    }
                }
                free(tasks);
            }
        }
        
        // Essayer TIMES_EXITCODES
        uint32_t nbruns = v32;
        if (nbruns > 0 && nbruns <= 1000000) {
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
        
        // Essayer OUTPUT (STDOUT/STDERR)
        uint32_t len = v32;
        if (len <= (1024 * 1024 * 100)) { // limite 100MB
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
                    // len == 0, sortie vide
                    buf[0] = '\0';
                    resp->u.output_ok.output = buf;
                    resp->u.output_ok.len = 0;
                    *resp_out = resp;
                    return 0;
                }
                free(buf);
            }
        }
        
        // Aucune interprétation n'a fonctionné
        errno = EPROTO;
        goto fail;
    }
    
    // On n'a pas pu lire un uint32, essayer uint64 (CREATE/COMBINE/REMOVE/TERMINATE)
    uint64_t taskid;
    if (read_uint64(fd, &taskid) == 0) {
        resp->u.create_ok.taskid = taskid;
        *resp_out = resp;
        return 0;
    }
    
    // REMOVE/TERMINATE OK : pas de données supplémentaires
    // (on a déjà lu anstype = OK, et il n'y a rien d'autre à lire)
    *resp_out = resp;
    return 0;

fail:
    free(resp);
    return -1;
}

//free
void free_request(request_t *req) {
    if (!req) return;
    switch (req->opcode) {
    case OPCODE_CREATE:
        if (req->u.create.argv) {
            for (uint32_t i = 0; i < req->u.create.argc; ++i) free(req->u.create.argv[i]);
            free(req->u.create.argv);
        }
        break;
    case OPCODE_COMBINE:
        free(req->u.combine.taskids);
        break;
    default:
        break;
    }
    free(req);
}

void free_response(response_t *resp) {
    if (!resp) return;

    if (resp->anstype == ANSTYPE_ERROR) {
        
    } else if (resp->anstype == ANSTYPE_OK) {
        if (resp->u.list_ok.tasks) {
            for (uint32_t i = 0; i < resp->u.list_ok.nbtasks; ++i) {
                task_t *t = resp->u.list_ok.tasks[i];
                if (!t) continue;
                if (t->cmd) free_command(t->cmd);
                free(t);
            }
            free(resp->u.list_ok.tasks);
        }
        if (resp->u.times_exitcodes_ok.timestamps) free(resp->u.times_exitcodes_ok.timestamps);
        if (resp->u.times_exitcodes_ok.exitcodes) free(resp->u.times_exitcodes_ok.exitcodes);
        if (resp->u.output_ok.output) free(resp->u.output_ok.output);
    }

    free(resp);
}
