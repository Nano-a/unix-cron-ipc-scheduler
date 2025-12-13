# Dossier `include/`

Ce dossier contient tous les fichiers d'en-tête (`.h`) du projet. Ces fichiers définissent les interfaces publiques des modules et les structures de données partagées.

## 📁 Fichiers

### `serialization.h` - Interface de Sérialisation

**Rôle** : Définit les types de données et les fonctions pour la sérialisation/désérialisation binaire.

**Types définis** :

#### `timing_t`
Structure représentant la planification d'une tâche :
```c
typedef struct {
    uint64_t minutes;      // Masque de bits pour les minutes (0-59)
    uint32_t hours;        // Masque de bits pour les heures (0-23)
    uint8_t daysofweek;    // Masque de bits pour les jours (0-6, dimanche=0)
} timing_t;
```

**Algorithme de vérification** :
- Un bit à 1 signifie que la valeur correspondante est activée
- Exemple : `minutes = 0x0000000000000001` → minute 0 activée
- Exemple : `hours = 0x00000042` → heures 1 et 6 activées

#### `command_t`
Structure représentant une commande (simple ou séquence) :
```c
typedef struct command {
    uint16_t type;         // Type : "SI" (simple) ou "SQ" (séquence)
    
    // Pour commande simple
    uint32_t argc;         // Nombre d'arguments
    char **argv;           // Tableau d'arguments
    
    // Pour séquence
    uint32_t nb_cmds;      // Nombre de sous-commandes
    struct command **cmds; // Tableau de sous-commandes
} command_t;
```

**Fonctions de sérialisation d'entiers** :
- `write_uint8/16/32/64()` : Écriture en big-endian
- `read_uint8/16/32/64()` : Lecture en big-endian
- `write_int64()` / `read_int64()` : Gestion des entiers signés

**Fonctions de sérialisation de structures** :
- `write_timing()` / `read_timing()` : Sérialisation du timing
- `write_arguments()` / `read_arguments()` : Sérialisation d'un tableau d'arguments
- `write_command()` / `read_command()` : Sérialisation récursive d'une commande

**Fonctions utilitaires** :
- `create_simple_command()` : Crée une commande simple
- `create_sequence_command()` : Crée une commande séquence
- `free_command()` : Libère récursivement une commande
- `type_from_str()` : Convertit "SI"/"SQ" en uint16_t

---

### `task_tree.h` - Interface de Gestion des Tâches

**Rôle** : Définit la structure `task_t` et les fonctions pour gérer l'arborescence des tâches.

**Type défini** :

#### `task_t`
Structure représentant une tâche complète :
```c
typedef struct {
    uint64_t taskid;       // Identifiant unique de la tâche
    timing_t timing;       // Configuration de planification
    command_t *cmd;        // Commande à exécuter
} task_t;
```

**Fonctions de gestion du répertoire** :
- `init_task_directory()` : Initialise le répertoire de base des tâches
- `build_task_dir_path()` : Construit le chemin vers le répertoire d'une tâche
- `build_task_path()` : Construit le chemin vers un fichier d'une tâche

**Fonctions de chargement/sauvegarde** :
- `load_task_from_dir()` : Charge une tâche depuis le système de fichiers
- `save_task_to_dir()` : Sauvegarde une tâche dans le système de fichiers
- `free_task()` : Libère la mémoire d'une tâche

**Fonctions de journalisation** :
- `append_execution_log()` : Ajoute une entrée dans `times-exitcodes`
- `save_stdout()` : Sauvegarde la sortie standard
- `save_stderr()` : Sauvegarde la sortie d'erreur

**Constantes** :
- `MAX_PATH_LEN` : Longueur maximale d'un chemin (512)

---

### `execution.h` - Interface d'Exécution

**Rôle** : Définit les fonctions pour exécuter des commandes et capturer leurs sorties.

**Fonction principale** :

#### `execute_simple_command()`
Exécute une commande simple et capture stdout/stderr.

**Paramètres** :
- `cmd` : Commande à exécuter (doit être de type "SI")
- `stdout_buf` : Pointeur vers le buffer de sortie (alloué par la fonction)
- `stdout_len` : Longueur de la sortie
- `stderr_buf` : Pointeur vers le buffer d'erreur (alloué par la fonction)
- `stderr_len` : Longueur de l'erreur
- `exitcode` : Code de retour du processus

**Retour** : 0 en cas de succès, -1 en cas d'erreur

#### `execute_sequence_command()`
Exécute une séquence de commandes de manière séquentielle.

**Comportement** :
- Exécute les sous-commandes dans l'ordre (0, 1, 2, ...)
- Concatène stdout et stderr de toutes les sous-commandes
- Le code de retour est celui de la **dernière** commande exécutée

**Algorithme** :
```
Pour chaque sous-commande i de 0 à nb_cmds-1 :
  1. Exécuter la sous-commande
  2. Concaténer stdout dans le buffer global
  3. Concaténer stderr dans le buffer global
  4. Mémoriser le code de retour (last_exit = sub_exit)
Retourner les buffers concaténés et last_exit
```

---

### `protocol.h` - Interface de Protocole

**Rôle** : Définit les structures et fonctions pour la communication client-démon via tubes nommés.

**Fonctionnalités (Jalon 2)** :
- Structures de requêtes/réponses (request_t, response_t)
- Codes d'opération : LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE
- Codes d'erreur : NOT_FOUND, NOT_RUN
- Fonctions d'envoi/réception de messages
- Gestion des tubes nommés (FIFO)

**Fonctionnalités prévues (Rendu Final)** :
- Codes d'opération : CREATE, REMOVE, COMBINE

**Structures principales** :
- `request_t` : Structure de requête client → démon
- `response_t` : Structure de réponse démon → client
- `OPCODE_*` : Codes d'opération
- `ANSTYPE_*` : Types de réponse (OK, ERROR)
- `ERRCODE_*` : Codes d'erreur

**Fonctions clés** :
- `send_request()` / `receive_request()` : Envoi/réception de requêtes
- `send_response()` / `receive_response()` : Envoi/réception de réponses
- `open_pipes_daemon()` / `open_pipes_client()` : Ouverture des tubes nommés
- `init_pipes()` : Initialisation des pipes
- `free_request()` / `free_response()` : Libération mémoire

---

## 🔗 Dépendances entre Headers

```
task_tree.h
  └── serialization.h

execution.h
  └── serialization.h

protocol.h
  └── (indépendant pour l'instant)
```

## 📝 Conventions

- Tous les headers utilisent des **guards d'inclusion** (`#ifndef ... #define ... #endif`)
- Les fonctions retournent `0` en cas de succès, `-1` en cas d'erreur (avec `errno` positionné)
- Les structures allouées dynamiquement doivent être libérées avec les fonctions `free_*()` correspondantes
- Les chemins de fichiers sont limités à `MAX_PATH_LEN` caractères
- Tous les entiers sont sérialisés en **big-endian** pour la portabilité

## 🔍 Format de Sérialisation

### Timing
```
MINUTES <uint64> (8 octets)
HOURS <uint32> (4 octets)
DAYSOFWEEK <uint8> (1 octet)
Total : 13 octets
```

### Arguments
```
ARGC <uint32> (4 octets)
Pour chaque argument i de 0 à ARGC-1 :
  LENGTH[i] <uint32> (4 octets)
  DATA[i] <char[LENGTH[i]]> (LENGTH[i] octets)
```

### Commande Simple
```
TYPE <uint16> (2 octets, valeur "SI")
ARGC <uint32> (4 octets)
ARGV[] (selon format arguments ci-dessus)
```

### Commande Séquence
```
TYPE <uint16> (2 octets, valeur "SQ")
NB_CMDS <uint32> (4 octets)
Pour chaque sous-commande i de 0 à NB_CMDS-1 :
  COMMAND[i] (sérialisation récursive)
```

### Log d'Exécution (times-exitcodes)
```
TIMESTAMP <int64> (8 octets)
EXITCODE <uint16> (2 octets)
(Répété pour chaque exécution)
```

