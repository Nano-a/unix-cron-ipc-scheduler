#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

pipes_t pipes;

int get_request_pipe_fd(void) {
    return pipes.req_fd; // ou ton objet pipes global
}

void close_pipes(pipes_t *p) {
    if (p->req_fd >= 0) close(p->req_fd);
    if (p->rep_fd >= 0) close(p->rep_fd);
}

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
    char pipe_dir[512];

    // Construire le path du dossier pipes
    snprintf(pipe_dir, sizeof(pipe_dir), "%s/pipes", run_dir);

    // Crée le dossier si inexistant
    mkdir(pipe_dir, 0777);

    // Construire les chemins des FIFOs
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

// --------------------------------------------------------------------
// DAEMON : ouvre request en lecture+écriture, reply en lecture+écriture
// ---------------------------------------------------------------------
int open_pipes_daemon(pipes_t *p, const char *run_dir) {
    char req_path[512];
    char rep_path[512];

    build_pipe_path(req_path, sizeof(req_path), run_dir, REQUEST_FIFO);
    build_pipe_path(rep_path, sizeof(rep_path), run_dir, REPLY_FIFO);

    // Ouvrir le pipe de requête en lecture+écriture pour éviter blocage
    p->req_fd = open(req_path, O_RDWR);
    if (p->req_fd < 0) {
        perror("daemon open request");
        return -1;
    }

    // Ouvrir le pipe de réponse en lecture+écriture pour éviter blocage
    p->rep_fd = open(rep_path, O_RDWR);
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

void handle_request(int fd) {
    request_t *req = NULL;
    if (receive_request(fd, &req) < 0) {
        perror("receive_request");
        return;
    }

    response_t *resp = malloc(sizeof(response_t));
    if (!resp) {
        perror("malloc response");
        free_request(req);
        return;
    }

    // Initialiser la réponse en erreur
    resp->anstype = ANSTYPE_ERROR;    // au lieu de 'status = ERROR'
    resp->u.error.errcode = ERRCODE_GENERIC; // ou autre code d'erreur défini

    if (send_response(fd, resp) < 0) {
        perror("send_response");
    }

    free_request(req);
    free_response(resp);
}
