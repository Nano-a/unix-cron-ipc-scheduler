#define _POSIX_C_SOURCE 200809L
#include "task_tree.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>   // malloc, free
#include <fcntl.h>    // open, O_RDONLY
#include <unistd.h>   // close, read, write

#define TASK_TIMING_FILENAME  "timing"
#define TASK_COMMAND_FILENAME "cmd.bin"

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
    int result = snprintf(path, path_size, "%s/erraid/tasks/%lu", run_dir, taskid);
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
    int result = snprintf(path, path_size, "%s/erraid/tasks/%lu/%s",
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

static int load_command_file(const char *path, command_t **cmd_out) {
    if (!cmd_out) {
        errno = EINVAL;
        return -1;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    command_t *cmd = NULL;
    if (read_command(fd, &cmd) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }
    close(fd);

    *cmd_out = cmd;
    return 0;
}

static int save_command_file(const char *path, const command_t *cmd) {
    if (!cmd) {
        errno = EINVAL;
        return -1;
    }

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }

    if (write_command(fd, cmd) < 0) {
        int saved = errno;
        close(fd);
        errno = saved;
        return -1;
    }

    if (close(fd) < 0) {
        return -1;
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

    if (build_task_path(path, sizeof(path), run_dir, taskid, TASK_COMMAND_FILENAME) < 0) {
        goto error;
    }

    if (load_command_file(path, &task->cmd) < 0) {
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

    if (build_task_path(file_path, sizeof(file_path), run_dir, task->taskid, TASK_COMMAND_FILENAME) < 0) {
        return -1;
    }

    return save_command_file(file_path, task->cmd);
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
