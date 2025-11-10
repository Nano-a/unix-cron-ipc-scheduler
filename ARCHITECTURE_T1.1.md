# Architecture T1.1 - Structure du Projet

## 📐 Architecture Globale

Basée sur les spécifications du professeur dans `arborescence.md`, `serialisation.md` et `protocole.md`.

### Structure des répertoires du projet

```
projet-systeme-l3/
├── src/
│   ├── erraid.c          # Démon principal
│   ├── tadmor.c          # Client
│   ├── serialization.c   # Sérialisation/désérialisation
│   ├── task_tree.c       # Gestion de l'arborescence
│   └── protocol.c        # Protocole de communication
├── include/
│   ├── serialization.h   # Types et fonctions de sérialisation
│   ├── task_tree.h       # Types et fonctions pour l'arborescence
│   └── protocol.h        # Types et fonctions du protocole
├── Makefile              # Compilation
├── README.md             # Documentation utilisateur
├── ARCHITECTURE.md       # Architecture complète (à compléter)
├── AUTHORS.md            # Liste des membres
├── BACKLOG.md            # Tâches du projet
└── PLANNING.md           # Planning Scrum
```

### Structure de l'arborescence de stockage (sur disque)

D'après `arborescence.md`, le démon stocke ses données dans :

```
/tmp/$USER/erraid/          # RUN_DIRECTORY (par défaut)
├── pipes/                 # Répertoire des tubes nommés
│   ├── erraid-request-pipe
│   └── erraid-reply-pipe
└── tasks/                 # Arborescence des tâches
    ├── 0/                 # Tâche ID 0
    │   ├── cmd/           # Commande à exécuter
    │   │   ├── type       # uint16 : 'SI' (simple) ou autre (complexe)
    │   │   └── argv       # Format arguments (si simple)
    │   │   └── 0/         # Sous-commande 0 (si complexe)
    │   │       ├── type
    │   │       └── argv
    │   │   └── 1/         # Sous-commande 1 (si complexe)
    │   │       └── ...
    │   ├── timing         # Format timing (uint64 + uint32 + uint8)
    │   ├── times-exitcodes # Liste (timestamp int64 + exitcode uint16)
    │   ├── stdout         # Sortie standard dernière exécution
    │   └── stderr         # Sortie erreur dernière exécution
    ├── 1/
    │   └── ...
    └── ...
```

## 🔧 Modules à créer

### 1. Module Serialization (`serialization.h` / `serialization.c`)

**Responsabilité** : Conversion entre structures C et format binaire big-endian

**Types à définir** (d'après `serialisation.md`) :
```c
// Timing : 3 champs
typedef struct {
    uint64_t minutes;      // bits 0-59
    uint32_t hours;         // bits 0-23
    uint8_t daysofweek;     // bits 0-6 (0=dimanche, 6=samedi)
} timing_t;

// Arguments : ARGC + ARGV[]
typedef struct {
    uint32_t argc;
    char **argv;            // Tableau de chaînes
} arguments_t;

// Commande : simple ou complexe (récursive)
typedef struct command {
    uint16_t type;          // 'SI' (0x5349) pour simple, 'SQ' (0x5351) pour séquence
    union {
        arguments_t args;   // Si type == 'SI'
        struct {
            uint32_t nbcmds;
            struct command **cmds;  // Tableau de pointeurs vers commandes
        } complex;          // Si type != 'SI'
    } u;
} command_t;
```

**Fonctions à implémenter** :
- `write_uint8/16/32/64`, `read_uint8/16/32/64` (avec htobe/be16toh)
- `write_int64`, `read_int64`
- `write_string`, `read_string` (LENGTH uint32 + DATA)
- `write_timing`, `read_timing`
- `write_arguments`, `read_arguments`
- `write_command`, `read_command` (récursif)
- `free_command`, `free_arguments` (libération mémoire)

### 2. Module Task Tree (`task_tree.h` / `task_tree.c`)

**Responsabilité** : Gestion de l'arborescence de stockage sur disque

**Types à définir** :
```c
// Tâche complète
typedef struct task {
    uint64_t taskid;        // Identifiant unique
    timing_t timing;         // Horaire d'exécution
    command_t *cmd;          // Commande à exécuter
} task_t;
```

**Fonctions à implémenter** :
- `init_task_directory(run_dir)` : Créer répertoire tasks/
- `load_task_from_dir(run_dir, taskid, task)` : Charger une tâche
- `save_task_to_dir(run_dir, task)` : Sauvegarder une tâche
- `delete_task_from_dir(run_dir, taskid)` : Supprimer une tâche
- `list_all_tasks(run_dir, tasks, count)` : Lister toutes les tâches
- `generate_task_id(run_dir)` : Générer ID unique
- `append_execution_log(run_dir, taskid, timestamp, exitcode)` : Ajouter log
- `read_execution_logs(run_dir, taskid, timestamps, exitcodes, count)` : Lire logs
- `save_stdout/stderr`, `read_stdout/stderr` : Gestion sorties
- `format_command_line(cmd)` : Formater pour affichage

### 3. Module Protocol (`protocol.h` / `protocol.c`)

**Responsabilité** : Communication client-serveur via tubes nommés

**Types à définir** (d'après `protocole.md`) :
```c
// Requêtes
typedef struct {
    uint16_t opcode;        // 'LS', 'CR', 'CB', 'RM', 'TX', 'SO', 'SE', 'TM'
    union {
        struct {
            timing_t timing;
            arguments_t args;
        } create;
        struct {
            timing_t timing;
            uint16_t type;
            uint32_t nbtasks;
            uint64_t *taskids;
        } combine;
        struct {
            uint64_t taskid;
        } query;
    } u;
} request_t;

// Réponses
typedef struct {
    uint16_t anstype;       // 'OK' ou 'ER'
    union {
        uint64_t taskid;
        struct {
            uint32_t nbtasks;
            task_t **tasks;
        } list_ok;
        // ... autres types de réponses
    } u;
} response_t;
```

**Fonctions à implémenter** :
- `send_request(fd, req)` : Envoyer requête
- `receive_request(fd, req)` : Recevoir requête
- `send_response(fd, resp)` : Envoyer réponse
- `receive_response(fd, resp)` : Recevoir réponse
- `free_request(req)`, `free_response(resp)` : Libération mémoire

### 4. Module Démon (`erraid.c`)

**Responsabilité** : Exécution des tâches et traitement des requêtes

**Fonctions principales** :
- `main()` : Point d'entrée, parsing arguments
- `init_pipes(pipes_dir)` : Créer tubes nommés (mkfifo)
- `open_pipes(pipes_dir)` : Ouvrir tubes pour communication
- `daemon_loop()` : Boucle principale
- `should_execute_task(timing, now)` : Vérifier si tâche à exécuter
- `execute_simple_command(args, stdout, stderr)` : Exécuter commande simple
- `execute_sequence_command(cmd, stdout, stderr)` : Exécuter séquence
- `execute_task(task)` : Orchestrer exécution complète
- `handle_request(req)` : Traiter requête client

### 5. Module Client (`tadmor.c`)

**Responsabilité** : Interface utilisateur et communication avec démon

**Fonctions principales** :
- `main()` : Point d'entrée, parsing arguments (getopt)
- `open_pipes(pipes_dir)` : Ouvrir tubes pour communication
- `parse_timing(minutes, hours, days, no_timing)` : Parser options timing
- `format_timing_display(timing)` : Formater timing pour affichage
- `cmd_list()`, `cmd_create()`, `cmd_combine()`, etc. : Commandes utilisateur

## 📋 Conventions de code

### Style
- Indentation : 4 espaces (pas de tabs)
- Noms de fonctions : `snake_case`
- Noms de types : `snake_case` avec `_t` suffix
- Constantes : `UPPER_CASE`

### Gestion mémoire
- Toujours vérifier les retours de `malloc()`
- Libérer toute mémoire allouée
- Utiliser `free_command()`, `free_task()`, etc.

### Gestion erreurs
- Vérifier toutes les valeurs de retour des appels système
- Retourner -1 en cas d'erreur, 0 en cas de succès
- Messages d'erreur explicites avec `perror()` si nécessaire

### Fichiers d'en-tête
- Protection contre inclusion multiple : `#ifndef XXX_H` / `#define XXX_H` / `#endif`
- Inclure seulement ce qui est nécessaire
- Ordre : système, puis projet

## 🔗 Dépendances entre modules

```
erraid.c
  ├── protocol.h (communication)
  ├── task_tree.h (gestion tâches)
  └── serialization.h (sérialisation)

tadmor.c
  ├── protocol.h (communication)
  └── serialization.h (parsing timing)

task_tree.c
  └── serialization.h (lecture/écriture fichiers)

protocol.c
  └── serialization.h (sérialisation messages)

serialization.c
  └── (aucune dépendance projet, seulement stdlib)
```

## 📝 Makefile

Structure recommandée :
```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INCLUDES = -Iinclude

SRCDIR = src
COMMON_SRCS = $(SRCDIR)/serialization.c $(SRCDIR)/task_tree.c $(SRCDIR)/protocol.c

erraid: $(SRCDIR)/erraid.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o erraid $^

tadmor: $(SRCDIR)/tadmor.c $(COMMON_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o tadmor $^

clean:
	rm -f erraid tadmor src/*.o

distclean: clean
	rm -rf /tmp/$$USER/erraid
```

## ✅ Checklist pour T1.1

- [ ] Créer structure répertoires `src/` et `include/`
- [ ] Créer fichiers `.h` avec structures de base
- [ ] Créer fichiers `.c` vides avec includes
- [ ] Créer Makefile fonctionnel
- [ ] Définir toutes les structures de données
- [ ] Documenter les interfaces dans les `.h`
- [ ] Vérifier compilation sans erreurs
- [ ] Tester que `make clean` fonctionne

## 📚 Références

- `arborescence.md` : Structure de stockage sur disque
- `serialisation.md` : Format de sérialisation binaire
- `protocole.md` : Format des messages client-serveur
- Exemples dans `exemples-arborescences/` : Arborescences de test

