# Dossier `src/`

Ce dossier contient tous les fichiers sources (`.c`) du projet. Chaque fichier implémente un module spécifique du système.

## 📁 Fichiers

### `erraid.c` - Démon Principal

**Rôle** : Implémente le démon qui exécute automatiquement les tâches planifiées.

**Fonctionnalités principales** :
- Boucle principale qui vérifie périodiquement les tâches à exécuter
- Chargement dynamique des tâches depuis le système de fichiers
- Vérification des conditions de timing (minutes, heures, jours de la semaine)
- Gestion des signaux (SIGINT, SIGTERM) pour arrêt propre
- Exécution des tâches via le module `execution`

**Algorithme principal (Jalon 1 + 2)** :
```
1. Initialiser le répertoire de travail et les tubes nommés
2. Enregistrer les handlers de signaux
3. Démarrer un thread d'exécution des tâches (vérifie toutes les secondes)
4. Ouvrir les tubes nommés pour la communication client
5. Boucle principale (daemon_loop) :
   a. Utiliser select() pour attendre les requêtes client (timeout 1 seconde)
   b. Si requête disponible : traiter et répondre
   c. Le thread d'exécution continue en parallèle
6. Sortir proprement (fermer pipes, arrêter threads)
```

**Thread d'exécution des tâches** :
```
1. Précharger toutes les tâches
2. Boucle infinie :
   a. Vérifier toutes les secondes quelles tâches doivent être exécutées
   b. Exécuter les tâches éligibles de manière asynchrone (chaque tâche dans son thread)
   c. Recharger les tâches périodiquement pour détecter les nouvelles
   d. Attendre 1 seconde
```

**Fonctions clés** :
- `main()` : Point d'entrée, parse les arguments, lance le thread et la boucle
- `task_execution_thread()` : Thread qui vérifie et exécute les tâches périodiquement
- `daemon_loop()` : Boucle principale qui gère les requêtes client (Jalon 2)
- `handle_request()` : Traite une requête client et génère la réponse
- `should_execute_task_simple()` : Vérifie si une tâche doit s'exécuter maintenant
- `execute_task_with_timestamp()` : Exécute une tâche avec un timestamp spécifique
- `async_task_executor()` : Thread qui exécute une tâche de manière asynchrone

---

### `execution.c` - Module d'Exécution

**Rôle** : Gère l'exécution des commandes (simples et séquences) avec capture des sorties.

**Fonctionnalités principales** :
- Exécution de commandes simples via `fork()` + `execvp()`
- Exécution séquentielle de commandes complexes
- Capture de stdout et stderr via pipes
- Gestion des codes de retour
- Concaténation des sorties pour les séquences

**Algorithme pour commande simple** :
```
1. Créer deux pipes (stdout et stderr)
2. Fork un processus fils
3. Dans le fils :
   - Rediriger stdout et stderr vers les pipes
   - Exécuter la commande avec execvp()
4. Dans le père :
   - Lire les pipes dans des buffers dynamiques
   - Attendre la fin du processus (waitpid)
   - Récupérer le code de retour
5. Retourner les buffers et le code de retour
```

**Algorithme pour séquence** :
```
1. Initialiser des buffers pour stdout et stderr
2. Pour chaque sous-commande (dans l'ordre 0, 1, 2, ...) :
   a. Exécuter la sous-commande (récursif)
   b. Concaténer stdout et stderr dans les buffers
   c. Mémoriser le code de retour (dernière commande)
3. Retourner les buffers concaténés et le dernier code de retour
```

**Structures internes** :
- `buffer_t` : Structure pour gérer des buffers dynamiques avec réallocation automatique

**Fonctions clés** :
- `execute_simple_command()` : Exécute une commande simple
- `execute_sequence_command()` : Exécute une séquence de commandes
- `buffer_*()` : Fonctions utilitaires pour la gestion de buffers

---

### `serialization.c` - Module de Sérialisation

**Rôle** : Convertit les structures C en format binaire big-endian et vice versa.

**Fonctionnalités principales** :
- Conversion d'entiers (uint8/16/32/64, int64) en big-endian
- Sérialisation/désérialisation de chaînes de caractères
- Sérialisation/désérialisation de structures `timing_t`
- Sérialisation/désérialisation de structures `command_t` (récursif)
- Gestion de la mémoire pour les chaînes et tableaux

**Format binaire** :
- Tous les entiers sont en **big-endian** (network byte order)
- Les chaînes sont préfixées par leur longueur (uint32)
- Les structures complexes sont sérialisées récursivement

**Algorithme de sérialisation d'une commande** :
```
Si commande simple (type == "SI") :
  1. Écrire le type (uint16)
  2. Écrire argc (uint32)
  3. Pour chaque argument :
     - Écrire la longueur (uint32)
     - Écrire les octets de la chaîne
Sinon (séquence) :
  1. Écrire le type (uint16)
  2. Écrire nb_cmds (uint32)
  3. Pour chaque sous-commande :
     - Sérialiser récursivement la sous-commande
```

**Fonctions clés** :
- `write_uint8/16/32/64()` : Écriture d'entiers en big-endian
- `read_uint8/16/32/64()` : Lecture d'entiers en big-endian
- `write_string()` / `read_string()` : Gestion des chaînes
- `write_timing()` / `read_timing()` : Gestion du timing
- `write_command()` / `read_command()` : Sérialisation récursive des commandes
- `write_arguments()` / `read_arguments()` : Gestion des tableaux d'arguments

---

### `task_tree.c` - Module de Gestion des Tâches

**Rôle** : Gère l'arborescence de fichiers pour stocker et charger les tâches.

**Fonctionnalités principales** :
- Création récursive de répertoires
- Chargement/sauvegarde de tâches depuis/vers le système de fichiers
- Gestion de l'arborescence des commandes (simples et complexes)
- Enregistrement des logs d'exécution
- Sauvegarde de stdout/stderr

**Structure de stockage** :
```
$run_dir/tasks/<taskid>/
├── timing          # Fichier binaire (uint64 + uint32 + uint8)
├── cmd/            # Répertoire de la commande
│   ├── type        # Fichier uint16 (type de commande)
│   ├── argv        # Fichier binaire (si commande simple)
│   └── 0/, 1/, ... # Sous-répertoires (si séquence)
├── times-exitcodes # Logs binaires (int64 + uint16 répétés)
├── stdout          # Sortie standard (texte)
└── stderr          # Sortie erreur (texte)
```

**Algorithme de chargement d'une commande** :
```
1. Lire le fichier "type" pour déterminer le type
2. Si commande simple :
   a. Lire le fichier "argv"
   b. Désérialiser les arguments
3. Si séquence :
   a. Parcourir les sous-répertoires numériques (0, 1, 2, ...)
   b. Pour chaque sous-répertoire :
      - Charger récursivement la sous-commande
   c. Vérifier que tous les indices de 0 à MAX existent
```

**Algorithme de sauvegarde d'une commande** :
```
1. Créer le répertoire cmd/ si nécessaire
2. Écrire le fichier "type"
3. Si commande simple :
   a. Écrire le fichier "argv" avec les arguments sérialisés
4. Si séquence :
   a. Pour chaque sous-commande (i = 0 à nb_cmds-1) :
      - Créer le répertoire cmd/i/
      - Sauvegarder récursivement la sous-commande
```

**Fonctions clés** :
- `init_task_directory()` : Initialise le répertoire de base
- `load_task_from_dir()` : Charge une tâche complète
- `save_task_to_dir()` : Sauvegarde une tâche complète
- `load_command_from_dir()` : Charge une commande (récursif)
- `save_command_to_dir()` : Sauvegarde une commande (récursif)
- `append_execution_log()` : Ajoute une entrée dans times-exitcodes
- `save_stdout()` / `save_stderr()` : Sauvegarde les sorties

---

### `protocol.c` - Module de Communication

**Rôle** : Gère la communication entre le client `tadmor` et le démon `erraid` via tubes nommés (FIFO).

**Fonctionnalités principales** :
- Création et ouverture des tubes nommés (request-pipe, reply-pipe)
- Sérialisation/désérialisation des requêtes (request_t)
- Sérialisation/désérialisation des réponses (response_t)
- Gestion des codes d'opération (LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE)
- Gestion des codes d'erreur (NOT_FOUND, NOT_RUN)

**Tubes nommés** :
- `erraid-request-pipe` : Client → Démon (requêtes)
- `erraid-reply-pipe` : Démon → Client (réponses)

**Format des messages** :
- Tous les messages sont sérialisés en **big-endian**
- Format défini dans `sy5-2025-2026/Projet/protocole.md`

**Fonctions clés** :
- `init_pipes()` : Crée les tubes nommés s'ils n'existent pas
- `open_pipes_daemon()` : Ouvre les pipes pour le démon
- `open_pipes_client()` : Ouvre les pipes pour le client
- `send_request()` / `receive_request()` : Envoi/réception de requêtes
- `send_response()` / `receive_response()` : Envoi/réception de réponses
- `free_request()` / `free_response()` : Libération mémoire

---

### `tadmor.c` - Client de Gestion

**Rôle** : Client en ligne de commande pour communiquer avec le démon et gérer les tâches.

**Fonctionnalités (Jalon 2)** :
- `LIST` : Lister toutes les tâches
- `TIMES_EXITCODES` : Historique d'exécution d'une tâche
- `STDOUT` : Sortie standard de la dernière exécution
- `STDERR` : Sortie d'erreur de la dernière exécution
- `TERMINATE` : Arrêter le démon

**Fonctionnalités prévues (Rendu Final)** :
- `CREATE` : Créer une nouvelle tâche simple
- `REMOVE` : Supprimer une tâche
- `COMBINE` : Créer une tâche par combinaison

**Communication** :
- Utilise des tubes nommés (FIFO) pour communiquer avec le démon
- Format binaire big-endian selon le protocole défini

**Fonctions clés** :
- `main()` : Point d'entrée, parse les arguments, envoie les requêtes
- `handle_list_response()` : Affiche la réponse LIST
- `handle_times_exitcodes_response()` : Affiche l'historique
- `handle_output_response()` : Affiche stdout/stderr

---

## 🔗 Dépendances entre Modules

```
erraid.c
  ├── execution.h → execution.c
  └── task_tree.h → task_tree.c
      └── serialization.h → serialization.c

tadmor.c
  ├── protocol.h → protocol.c
  ├── task_tree.h → task_tree.c
  └── serialization.h → serialization.c

protocol.c
  ├── serialization.h → serialization.c
  └── task_tree.h → task_tree.c

execution.c
  └── serialization.h → serialization.c

task_tree.c
  └── serialization.h → serialization.c
```

## 📝 Notes d'Implémentation

- Tous les fichiers utilisent `_POSIX_C_SOURCE 200809L` pour garantir la compatibilité POSIX
- La gestion d'erreurs utilise `errno` et retourne -1 en cas d'erreur
- La mémoire est gérée manuellement (malloc/free) avec vérification systématique
- Les chemins de fichiers sont limités à `MAX_PATH_LEN` (512 caractères)
- L'ordre des séquences est garanti numérique (0, 1, 2, 3...) lors du chargement

