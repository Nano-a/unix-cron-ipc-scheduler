#define _POSIX_C_SOURCE 200809L
#include "task_tree.h"
#include "task_tree.h"
#include "serialization.h"  // pour read_timing, read_arguments
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>   // malloc, free
#include <fcntl.h>    // open, O_RDONLY
#include <unistd.h>   // close, read, write

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

int load_task_from_dir(const char *run_dir, uint64_t taskid, task_t **task) {
    char path[MAX_PATH_LEN];
    int fd;
    
    *task = malloc(sizeof(task_t));
    if (!*task) return -1;
    
    (*task)->taskid = taskid;
    
    // Charger le timing
    if (build_task_path(path, sizeof(path), run_dir, taskid, "timing") < 0) {
        free(*task);
        *task = NULL;
        return -1;
    }
    
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        free(*task);
        *task = NULL;
        return -1;
    }
    
    if (read_timing(fd, &(*task)->timing) < 0) {
        close(fd);
        free(*task);
        *task = NULL;
        return -1;
    }
    close(fd);
    
    // Charger la commande
    if (build_task_path(path, sizeof(path), run_dir, taskid, "cmd") < 0) {
        free(*task);
        *task = NULL;
        return -1;
    }
    
    if (load_complex_command(path, &(*task)->cmd) < 0) {
        free(*task);
        *task = NULL;
        return -1;
    }
    
    return 0;
}

static int load_complex_command(const char *cmd_dir, command_t **cmd) {
    char path[MAX_PATH_LEN];
    int fd;
    uint16_t type;
    
    // Lire le type
    snprintf(path, sizeof(path), "%s/type", cmd_dir);
    fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    
    if (read_uint16(fd, &type) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    
    *cmd = malloc(sizeof(command_t));
    if (!*cmd) return -1;
    
    (*cmd)->type = type;
    
    if (type == CMD_TYPE_SIMPLE) {
        // Commande simple : lire argv
        snprintf(path, sizeof(path), "%s/argv", cmd_dir);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            free(*cmd);
            *cmd = NULL;
            return -1;
        }
        
        if (read_arguments(fd, &(*cmd)->u.args) < 0) {
            close(fd);
            free(*cmd);
            *cmd = NULL;
            return -1;
        }
        close(fd);
    } else {
        // Commande complexe : lire les sous-commandes
        // TODO: Implémenter la lecture récursive
    }
    
    return 0;
}

int save_task_to_dir(const char *run_dir, const task_t *task) {
    char path[MAX_PATH_LEN];
    char dir_path[MAX_PATH_LEN];
    int fd;
    
    // Construire le chemin du répertoire de la tâche
    if (build_task_dir_path(dir_path, sizeof(dir_path), run_dir, task->taskid) < 0) {
        return -1;
    }
    
    // Créer le répertoire récursivement
    // Utiliser mkdir -p équivalent
    // ...
    
    // Sauvegarder le timing
    if (build_task_path(path, sizeof(path), run_dir, task->taskid, "timing") < 0) {
        return -1;
    }
    
    fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) return -1;
    
    if (write_timing(fd, &task->timing) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    
    // Sauvegarder la commande
    if (build_task_path(path, sizeof(path), run_dir, task->taskid, "cmd") < 0) {
        return -1;
    }
    
    if (save_command_to_dir(path, task->cmd) < 0) {
        return -1;
    }
    
    return 0;
}
```

#### save_command_to_dir (récursif)

```c
static int save_command_to_dir(const char *cmd_dir, const command_t *cmd) {
    char path[MAX_PATH_LEN];
    int fd;
    
    // Créer le répertoire cmd si nécessaire
    // ...
    
    // Écrire le type
    snprintf(path, sizeof(path), "%s/type", cmd_dir);
    fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) return -1;
    
    if (write_uint16(fd, cmd->type) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    
    if (cmd->type == CMD_TYPE_SIMPLE) {
        // Commande simple : écrire argv
        snprintf(path, sizeof(path), "%s/argv", cmd_dir);
        fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd < 0) return -1;
        
        if (write_arguments(fd, &cmd->u.args) < 0) {
            close(fd);
            return -1;
        }
        close(fd);
    } else {
        // Commande complexe : sauvegarder récursivement
        // TODO: Implémenter la sauvegarde récursive
    }
    
    return 0;
}
```
