#ifndef TASK_TREE_H
#define TASK_TREE_H

#include <stddef.h>   // pour size_t
#include <stdint.h>   // pour uint64_t

// Si timing_t et command_t sont définis ailleurs, il faut inclure leurs headers aussi
#include "timing_t.h"   // (si tu as un fichier qui définit timing_t)
#include "command_t.h"  // (si tu as un fichier qui définit command_t)

#define MAX_PATH_LEN 512

typedef struct {
    uint64_t taskid;      // Identifiant unique
    timing_t timing;      // Horaire d’exécution
    command_t *cmd;       // Commande à exécuter
} task_t;

int init_task_directory(const char *orig_path);
int build_task_dir_path(char *path, size_t path_size, const char *run_dir, uint64_t taskid);
int build_task_path(char *path, size_t path_size, const char *run_dir, uint64_t taskid, const char *filename);

int load_task_from_dir(const char *run_dir, uint64_t taskid, task_t **task);
static int load_complex_command(const char *cmd_dir, command_t **cmd);
#endif // TASK_TREE_H
