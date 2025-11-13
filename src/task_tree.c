#define _POSIX_C_SOURCE 200809L
#include "task_tree.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

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


int main(void) {
    char path[512];

    // Test 1 : initialisation du répertoire
    init_task_directory("/mnt/c/Users/ahmed/OneDrive/Bureau/System");

    // Test 2 : construction d’un chemin de tâche
    if (build_task_dir_path(path, sizeof(path), "/mnt/c/Users/ahmed/OneDrive/Bureau/System", 5) == 0) {
        printf("Dossier tâche : %s\n", path);
    }

    // Test 3 : construction d’un chemin de fichier
    if (build_task_path(path, sizeof(path), "/mnt/c/Users/ahmed/OneDrive/Bureau/System", 5, "timing") == 0) {
        printf("Fichier tâche : %s\n", path);
    }

    return 0;
}
