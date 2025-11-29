#include "named_pipes.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

// ---------------------------------------------------------------------
// Construit : run_dir/pipes/<name>
// ---------------------------------------------------------------------
int build_pipe_path(char *buffer, size_t size, const char *run_dir, const char *name) {
    int r = snprintf(buffer, size, "%s/pipes/%s", run_dir, name);
    if (r < 0 || r >= (int)size) return -1;
    return 0;
}

// ---------------------------------------------------------------------
// Création des FIFOs (supprime si déjà existants)
// ---------------------------------------------------------------------
int init_pipes(const char *run_dir) {
    char req_path[512];
    char rep_path[512];

    if (build_pipe_path(req_path, sizeof(req_path), run_dir, REQUEST_FIFO) < 0 ||
        build_pipe_path(rep_path, sizeof(rep_path), run_dir, REPLY_FIFO) < 0) {
        return -1;
    }

    // Supprimer si existants
    unlink(req_path);
    unlink(rep_path);

    // Créer FIFOs
    if (mkfifo(req_path, 0666) < 0) {
        perror("mkfifo request");
        return -1;
    }

    if (mkfifo(rep_path, 0666) < 0) {
        perror("mkfifo reply");
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------------------
// DAEMON : ouvre request en lecture, reply en écriture
// ---------------------------------------------------------------------
int open_pipes_daemon(pipes_t *p, const char *run_dir) {
    char req_path[512];
    char rep_path[512];

    build_pipe_path(req_path, sizeof(req_path), run_dir, REQUEST_FIFO);
    build_pipe_path(rep_path, sizeof(rep_path), run_dir, REPLY_FIFO);

    p->req_fd = open(req_path, O_RDONLY);
    if (p->req_fd < 0) {
        perror("daemon open request");
        return -1;
    }

    p->rep_fd = open(rep_path, O_WRONLY);
    if (p->rep_fd < 0) {
        perror("daemon open reply");
        close(p->req_fd);
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------------------
// CLIENT : ouvre request en écriture, reply en lecture
// ---------------------------------------------------------------------
int open_pipes_client(pipes_t *p, const char *run_dir) {
    char req_path[512];
    char rep_path[512];

    build_pipe_path(req_path, sizeof(req_path), run_dir, REQUEST_FIFO);
    build_pipe_path(rep_path, sizeof(rep_path), run_dir, REPLY_FIFO);

    p->req_fd = open(req_path, O_WRONLY);
    if (p->req_fd < 0) {
        perror("client open request");
        return -1;
    }

    p->rep_fd = open(rep_path, O_RDONLY);
    if (p->rep_fd < 0) {
        perror("client open reply");
        close(p->req_fd);
        return -1;
    }

    return 0;
}
