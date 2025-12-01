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
