# Conventions de Code - Projet erraid-tadmor

## Style de code

### Indentation
- **4 espaces** (pas de tabs)
- Utiliser des espaces cohérents pour l'alignement

### Nommage

#### Fonctions
- Style : `snake_case`
- Exemples : `load_task_from_dir`, `write_uint32`, `execute_simple_command`
- Verbes à l'infinitif ou impératif : `read`, `write`, `save`, `load`, `execute`

#### Types et structures
- Style : `snake_case` avec suffixe `_t`
- Exemples : `timing_t`, `arguments_t`, `command_t`, `task_t`
- Structures : `typedef struct { ... } nom_t;`

#### Constantes et macros
- Style : `UPPER_CASE`
- Exemples : `CMD_TYPE_SIMPLE`, `OPCODE_LIST`, `MAX_PATH_LEN`
- Définitions de macros : `#define NOM valeur`

#### Variables
- Style : `snake_case`
- Exemples : `task_count`, `stdout_buf`, `request_fd`
- Variables globales : éviter si possible, sinon préfixer avec `g_` ou module

### Fichiers d'en-tête (.h)

#### Protection contre inclusion multiple
```c
#ifndef SERIALIZATION_H
#define SERIALIZATION_H

// ... contenu ...

#endif // SERIALIZATION_H
```

#### Ordre des includes
1. Définitions de macros système (`_POSIX_C_SOURCE`, etc.)
2. Includes système (`<stdio.h>`, `<stdlib.h>`, etc.)
3. Includes du projet (`"serialization.h"`, etc.)

#### Documentation
- Commenter les fonctions publiques
- Documenter les paramètres et valeurs de retour
- Exemples de format :
```c
/**
 * Charge une tâche depuis le répertoire de stockage
 * @param run_dir Répertoire racine de stockage
 * @param taskid Identifiant de la tâche
 * @param task Pointeur vers la structure task_t à remplir
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int load_task_from_dir(const char *run_dir, uint64_t taskid, task_t **task);
```

### Fichiers source (.c)

#### Structure
1. Définitions de macros locales
2. Includes
3. Déclarations de fonctions statiques
4. Implémentation des fonctions publiques
5. Implémentation des fonctions statiques

#### Commentaires
- Commenter les algorithmes complexes
- Expliquer les choix de conception
- Utiliser `//` pour commentaires de ligne
- Utiliser `/* */` pour commentaires multi-lignes

## Gestion de la mémoire

### Allocation
- **Toujours vérifier** le retour de `malloc()`, `calloc()`, `realloc()`
- En cas d'échec, libérer ce qui a déjà été alloué et retourner une erreur
- Exemple :
```c
char *buf = malloc(size);
if (!buf) {
    // Nettoyer et retourner erreur
    return -1;
}
```

### Libération
- Libérer toute mémoire allouée
- Utiliser les fonctions de libération dédiées : `free_command()`, `free_task()`, etc.
- Après `free()`, mettre le pointeur à `NULL` si nécessaire
- Ne pas libérer deux fois le même pointeur

### Fuites mémoire
- Vérifier avec `valgrind` avant de merger
- S'assurer que tous les chemins d'exécution libèrent la mémoire

## Gestion des erreurs

### Valeurs de retour
- **0** : succès
- **-1** : erreur (ou valeur négative)
- Vérifier **toutes** les valeurs de retour des appels système

### Messages d'erreur
- Utiliser `perror()` pour les erreurs système
- Messages explicites pour l'utilisateur final
- Exemples :
```c
if (open(path, O_RDONLY) < 0) {
    perror("open");
    return -1;
}
```

### Codes d'erreur du protocole
- Utiliser les constantes définies : `ERRCODE_NOT_FOUND`, `ERRCODE_NOT_RUN`
- Ne pas inventer de nouveaux codes sans justification

## Appels système

### Vérifications
- Vérifier **tous** les retours d'appels système
- Gérer `errno` correctement
- Ne pas ignorer les warnings de compilation

### Exemples d'appels système utilisés
- `open()`, `close()`, `read()`, `write()` : fichiers
- `fork()`, `execvp()`, `waitpid()` : processus
- `pipe()`, `mkfifo()`, `dup2()` : communication
- `select()`, `time()`, `localtime()` : timing
- `opendir()`, `readdir()`, `closedir()` : répertoires

## Formatage

### Longueur des lignes
- Maximum 80-100 caractères par ligne
- Couper les longues lignes de manière lisible

### Espaces
- Espace après `if`, `for`, `while`, `switch`
- Pas d'espace avant `;` ou `,`
- Espace autour des opérateurs : `a = b + c`
- Pas d'espace dans les appels de fonction : `func(a, b)`

### Accolades
- Style K&R (accolade ouvrante sur la même ligne)
```c
if (condition) {
    // code
}
```

## Tests

### Avant commit
- Compiler sans erreurs ni warnings
- Tester les cas de base
- Vérifier avec `valgrind` si possible
- Tester sur lulu avant de merger

### Cas à tester
- Cas normaux (happy path)
- Cas d'erreur (fichiers manquants, mémoire insuffisante, etc.)
- Cas limites (valeurs nulles, tailles maximales, etc.)

## Git

### Messages de commit
- Format : `[Module] Description courte`
- Exemples :
  - `[Serialization] Ajout fonctions read/write pour uint16/32/64`
  - `[TaskTree] Implémentation load_task_from_dir`
  - `[Daemon] Ajout boucle principale avec select()`
  - `[Client] Implémentation cmd_list`

### Branches
- Format : `feature/nom-fonctionnalite`
- Créer depuis `develop`
- Une branche par fonctionnalité/tâche

### Avant merge
- Code compilé sans erreurs
- Tests passants
- Pas de fuites mémoire
- Code review par le chef de projet

## Références

- `ARCHITECTURE_T1.1.md` : Architecture détaillée
- `serialisation.md` : Format de sérialisation
- `arborescence.md` : Structure de stockage
- `protocole.md` : Protocole de communication

