#ifndef NAMED_PIPES_H
#define NAMED_PIPES_H

#include <stddef.h>

// Noms des FIFOs
#define REQUEST_FIFO "request.fifo"
#define REPLY_FIFO   "reply.fifo"

// Structure contenant les deux descripteurs de fichier
typedef struct {
    int req_fd;   // request fifo
    int rep_fd;   // reply fifo
} pipes_t;

// Initialise les tubes nommés (mkfifo)
int init_pipes(const char *run_dir);

// Fonctions pour construire les chemins (internes, mais tu peux les exposer si besoin)
int build_pipe_path(char *buffer, size_t size, const char *run_dir, const char *name);

// Ouverture côté démon : lit request, écrit reply
int open_pipes_daemon(pipes_t *p, const char *run_dir);

// Ouverture côté client : écrit request, lit reply
int open_pipes_client(pipes_t *p, const char *run_dir);

#endif
