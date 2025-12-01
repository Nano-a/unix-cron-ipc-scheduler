# Backlog des Tâches - Projet erraid-tadmor

## 📋 Vue d'ensemble

Ce fichier contient toutes les tâches du projet organisées chronologiquement, avec leurs prérequis, durées, dates et branches Git. Les tâches peuvent être prises en charge par n'importe quel membre de l'équipe selon les compétences et disponibilités.

### 🎯 Niveaux de difficulté
- **\*** = Débutant en C (notions de base : variables, fonctions, structures)
- **\*\*** = Intermédiaire en C (pointeurs, allocation mémoire, appels système de base)
- **\*\*\*** = Expert en C (gestion avancée mémoire, appels système complexes, optimisation)

---

## 🎯 JALON 1 - 21 novembre 2025

### T1.1 - Architecture globale et structure du projet
- **Difficulté** : **\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 10 novembre 2025, 14h00
- **Deadline** : 12 novembre 2025, 18h00
- **Branche Git** : `feature/architecture`
- **Prérequis** : Aucun (tâche de démarrage)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  - Définir l'architecture des modules (sérialisation, task_tree, protocole, démon, client)
  - Créer la structure des répertoires (src/, include/)
  - Établir les interfaces entre modules (fichiers .h)
  - Créer le Makefile avec règles de compilation
  - Définir les conventions de code
- **Livrable** : Structure du projet complète, Makefile fonctionnel, fichiers .h avec structures de base

---

### T1.2 - Module de gestion de l'arborescence (partie structure)
- **Difficulté** : **\***
- **Jalon** : Jalon 1
- **Durée estimée** : 3h
- **Date de début** : 12 novembre 2025, 18h00
- **Deadline** : 13 novembre 2025, 18h00
- **Branche Git** : `feature/task-tree-structure`
- **Prérequis** : T1.1 (architecture globale)
- **Responsable** : Ahmed
- **Description** :
  - Créer la structure de données `task_t`
  - Implémenter `init_task_directory` (création récursive des répertoires)
  - Implémenter `build_task_path` et `build_task_dir_path` (fonctions utilitaires)
  - Définir les constantes (MAX_PATH_LEN, etc.)
- **Livrable** : `task_tree.h` avec structures complètes, fonctions de base dans `task_tree.c`

---

### T1.4 - Module de sérialisation (partie 1 : entiers)
- **Difficulté** : **\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 3h
- **Date de début** : 10 novembre 2025, 18h00
- **Deadline** : 12 novembre 2025, 18h00
- **Branche Git** : `feature/serialization-integers`
- **Prérequis** : T1.1 (architecture globale)
- **Responsable** : Ahmed
- **Description** :
  - Implémenter `write_uint8/16/32/64` avec conversion big-endian (htobe16/32/64)
  - Implémenter `read_uint8/16/32/64` avec conversion big-endian (be16toh/32toh/64toh)
  - Implémenter `write_int64` et `read_int64`
  - Gestion des erreurs (vérification des valeurs de retour de read/write)
- **Livrable** : `serialization.c` avec toutes les fonctions pour les types entiers

---

### T1.5 - Module de sérialisation (partie 2 : types complexes)
- **Difficulté** : **\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 12 novembre 2025, 18h00
- **Deadline** : 14 novembre 2025, 18h00
- **Branche Git** : `feature/serialization-complex`
- **Prérequis** : T1.4 (sérialisation entiers)
- **Responsable** : Pitel
- **Description** :
  - Implémenter `write_string` et `read_string` (format : LENGTH uint32 + DATA)
  - Implémenter `write_timing` et `read_timing` (uint64 + uint32 + uint8)
  - Implémenter `write_arguments` et `read_arguments` (ARGC + ARGV[])
  - Gestion mémoire (allocation/désallocation pour read_string)
- **Livrable** : `serialization.c` avec fonctions pour string, timing, arguments

---

### T1.6 - Module de sérialisation (partie 3 : commandes)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 14 novembre 2025, 18h00
- **Deadline** : 16 novembre 2025, 18h00
- **Branche Git** : `feature/serialization-complex`
- **Prérequis** : T1.5 (sérialisation types complexes)
- **Responsable** : Pitel
- **Description** :
  - Implémenter `write_command` et `read_command` (récursif)
  - Gestion des commandes simples (TYPE='SI' + arguments)
  - Gestion des commandes complexes (TYPE + NBCMDS + sous-commandes)
  - Implémenter `create_simple_command` et `create_sequence_command`
  - Implémenter `free_command` (libération récursive)
- **Livrable** : `serialization.c` complet avec gestion des commandes

---

### T1.7 - Module de gestion de l'arborescence (partie lecture)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 13 novembre 2025, 18h00
- **Deadline** : 15 novembre 2025, 18h00
- **Branche Git** : `feature/task-tree`
- **Prérequis** : T1.2 (structure), T1.6 (sérialisation commandes)
- **Responsable** : Ahmed
- **Description** :
  - Implémenter `load_task_from_dir` (chargement d'une tâche complète)
  - Implémenter `load_complex_command` (récursif pour commandes complexes)
  - Lire timing depuis fichier
  - Lire commande depuis répertoire cmd/
  - Gestion des erreurs (fichiers manquants, format invalide)
- **Livrable** : Fonctions de chargement opérationnelles, tests avec exemples fournis

---

### T1.8 - Module de gestion de l'arborescence (partie écriture)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 10 novembre 2025, 18h00
- **Deadline** : 12 novembre 2025, 18h00
- **Branche Git** : `feature/task-tree`
- **Prérequis** : T1.2 (structure), T1.6 (sérialisation commandes)
- **Responsable** : Ahmed
- **Description** :
  - Implémenter `save_task_to_dir` (sauvegarde d'une tâche complète)
  - Implémenter `save_command_to_dir` (récursif pour commandes complexes)
  - Écrire timing dans fichier
  - Écrire commande dans répertoire cmd/
  - Création récursive des répertoires si nécessaire
- **Livrable** : Fonctions de sauvegarde opérationnelles

---

### T1.9 - Module d'exécution de commandes simples
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 5h
- **Date de début** : 15 novembre 2025, 14h00
- **Deadline** : 17 novembre 2025, 18h00
- **Branche Git** : `feature/execute-simple`
- **Prérequis** : T1.7 (lecture arborescence)
- **Responsable** : Abderrahman AJINOU
- **Description** :
  - Implémenter `execute_simple_command`
  - Utiliser `fork()` pour créer processus enfant
  - Utiliser `pipe()` pour capturer stdout et stderr
  - Utiliser `dup2()` pour rediriger les sorties
  - Utiliser `execvp()` pour exécuter la commande
  - Utiliser `waitpid()` pour récupérer le code de retour
  - Lire les pipes et stocker dans buffers
- **Livrable** : Exécution de commandes simples fonctionnelle avec capture des sorties

---

### T1.10 - Module d'exécution de séquences
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 4h
- **Date de début** : 17 novembre 2025, 18h00
- **Deadline** : 18 novembre 2025, 18h00
- **Branche Git** : `feature/execute-sequence`
- **Prérequis** : T1.9 (exécution commandes simples)
- **Responsable** : Pitel
- **Description** :
  - Implémenter `execute_sequence_command`
  - Exécuter les sous-commandes séquentiellement
  - Concaténer les sorties stdout et stderr
  - Garder le dernier code de retour
  - Gestion récursive pour séquences imbriquées
- **Livrable** : Exécution de séquences fonctionnelle

---

### T1.11 - Module de planification et boucle principale
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 5h
- **Date de début** : 18 novembre 2025, 18h00
- **Deadline** : 20 novembre 2025, 18h00
- **Branche Git** : `feature/daemon-loop`
- **Prérequis** : T1.10 (exécution séquences), T1.12 (logs)
- **Responsable** : Abderrahman AJINOU
- **Description** :
  - Implémenter `should_execute_task` (vérification timing avec bits)
  - Utiliser `time()` et `localtime()` pour obtenir date/heure actuelle
  - Implémenter `execute_task` (orchestration complète)
  - Implémenter `daemon_loop` (boucle principale)
  - Vérifier périodiquement (toutes les secondes) quelles tâches doivent s'exécuter
  - Charger toutes les tâches depuis le disque
  - Gérer les signaux (SIGINT, SIGTERM) pour arrêt propre
- **Livrable** : Démon fonctionnel exécutant les tâches aux dates prescrites

---

### T1.12 - Module de logs d'exécution
- **Difficulté** : **\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 3h
- **Date de début** : 15 novembre 2025, 18h00
- **Deadline** : 16 novembre 2025, 18h00
- **Branche Git** : `feature/execution-logs`
- **Prérequis** : T1.8 (écriture arborescence)
- **Responsable** : Pitel
- **Description** :
  - Implémenter `append_execution_log` (ajout dans times-exitcodes)
  - Implémenter `save_stdout` et `save_stderr` (écriture fichiers)
  - Format : timestamp (int64) + exitcode (uint16) pour chaque exécution
  - Gestion des erreurs d'écriture
- **Livrable** : Système de logs opérationnel

---

### T1.3 - Intégration et tests du jalon 1
- **Difficulté** : **\*\***
- **Jalon** : Jalon 1
- **Durée estimée** : 6h
- **Date de début** : 20 novembre 2025, 18h00
- **Deadline** : 21 novembre 2025, 12h00
- **Branche Git** : `integration/jalon-1`
- **Prérequis** : T1.11 (boucle principale), T1.12 (logs), T1.7 (lecture)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  - Tests d'intégration de toutes les fonctionnalités
  - Tests avec les exemples d'arborescences fournis
  - Vérification compilation sans erreurs ni warnings
  - Tests d'exécution de tâches simples
  - Tests d'exécution de séquences
  - Vérification des logs (times-exitcodes, stdout, stderr)
  - Corrections de bugs
- **Livrable** : Projet fonctionnel pour jalon 1, tag `jalon-1` créé

---

## 🎯 JALON 2 - 8 décembre 2025

### T2.1 - Architecture du protocole de communication
- **Difficulté** : **\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 27 novembre 2025, 18h00
- **Deadline** : 27 novembre 2025, 18h00
- **Branche Git** : `feature/protocol-architecture`
- **Prérequis** : T1.3 (jalon 1 terminé)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  Cette tâche consiste à définir l'architecture complète du protocole de communication entre le client (`tadmor`) et le démon (`erraid`). C'est la fondation de tout le Jalon 2.
  
  **Ce que tu dois faire :**
  1. **Créer le fichier `include/protocol.h`** avec toutes les définitions nécessaires
  2. **Définir les structures de données** :
     - `request_t` : structure qui encapsule toutes les requêtes possibles du client vers le démon
     - `response_t` : structure qui encapsule toutes les réponses possibles du démon vers le client
     - Utiliser des unions pour gérer les différents types de requêtes/réponses
  3. **Définir les opcodes (codes d'opération)** comme constantes `uint16_t` :
     - `OPCODE_LIST` (0x4c53, 'LS') : lister toutes les tâches
     - `OPCODE_CREATE` (0x4352, 'CR') : créer une nouvelle tâche simple
     - `OPCODE_COMBINE` (0x4342, 'CB') : créer une tâche par combinaison
     - `OPCODE_REMOVE` (0x524d, 'RM') : supprimer une tâche
     - `OPCODE_TIMES_EXITCODES` (0x5458, 'TX') : obtenir l'historique d'exécution
     - `OPCODE_STDOUT` (0x534f, 'SO') : obtenir la sortie standard
     - `OPCODE_STDERR` (0x5345, 'SE') : obtenir la sortie d'erreur
     - `OPCODE_TERMINATE` (0x544d, 'TM') : terminer le démon
  4. **Définir les types de réponse** :
     - `ANSTYPE_OK` (0x4f4b, 'OK') : requête réussie
     - `ANSTYPE_ERROR` (0x4552, 'ER') : requête échouée
  5. **Définir les codes d'erreur** :
     - `ERRCODE_NOT_FOUND` (0x4e46, 'NF') : tâche non trouvée
     - `ERRCODE_NOT_RUN` (0x4e52, 'NR') : tâche jamais exécutée
  6. **Définir les prototypes de fonctions** pour la sérialisation (seront implémentés dans T2.3) :
     - `send_request`, `receive_request`
     - `send_response`, `receive_response`
     - `free_request`, `free_response`
  7. **Documenter chaque structure et constante** avec des commentaires clairs
  
  **Pourquoi cette tâche est importante :** Sans cette architecture, personne ne peut implémenter la communication client-serveur. C'est comme construire une maison : il faut d'abord les plans avant de commencer à construire. Toutes les autres tâches du Jalon 2 dépendent de cette architecture.
  
  **Exemple de structure `request_t` :**
  - Contient un `opcode` (uint16_t) qui indique le type de requête
  - Contient une union `u` qui varie selon l'opcode :
    - Pour CREATE : timing + argc + argv
    - Pour COMBINE : timing + type + nbtasks + taskids
    - Pour REMOVE/TIMES_EXITCODES/STDOUT/STDERR : taskid
    - Pour LIST/TERMINATE : pas de données supplémentaires
  
  **Exemple de structure `response_t` :**
  - Contient un `anstype` (uint16_t) qui indique si c'est OK ou ERROR
  - Contient une union `u` qui varie selon le type et l'opcode :
    - Pour CREATE/COMBINE OK : taskid créé
    - Pour LIST OK : nbtasks + tableau de pointeurs vers tasks
    - Pour TIMES_EXITCODES OK : nbruns + timestamps + exitcodes
    - Pour STDOUT/STDERR OK : output (string) + len
    - Pour ERROR : errcode
- **Livrable** : `include/protocol.h` complet avec toutes les structures, constantes, prototypes et documentation, prêt à être utilisé par les autres tâches

---

### T2.2 - Gestion des tubes nommés (FIFO)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 27 novembre 2025, 23h00
- **Deadline** : 29 novembre 2025, 18h00
- **Branche Git** : `feature/named-pipes`
- **Prérequis** : T2.1 (architecture protocole)
- **Responsable** : Ahmed
- **Justification** : Ahmed a travaillé sur les appels système de base au Jalon 1 (task_tree avec gestion fichiers/répertoires). Les tubes nommés (FIFO) sont un mécanisme système similaire, donc cohérent avec ses compétences. Cette tâche est la base de la communication, donc logique qu'elle soit faite tôt par quelqu'un qui maîtrise les appels système.
- **Description** :
  Cette tâche consiste à implémenter la communication entre le client (`tadmor`) et le démon (`erraid`) en utilisant des **tubes nommés (FIFO)**. Les FIFO sont des fichiers spéciaux qui permettent la communication inter-processus (IPC).
  
  **Ce que tu dois faire :**
  1. **Implémenter `init_pipes(const char *run_dir)`** dans `src/protocol.c` :
     - Créer deux FIFO dans le répertoire `run_dir` : `run_dir/request` et `run_dir/reply`
     - Utiliser `mkfifo()` pour créer les FIFO avec les permissions 0666
     - Vérifier si les FIFO existent déjà (avec `stat()`) : si c'est déjà un FIFO, c'est bon ; si c'est un fichier normal, erreur
     - Gérer les erreurs (errno positionné correctement)
     - **Important** : Les chemins doivent être exactement `run_dir/request` et `run_dir/reply` (pas de sous-dossier `pipes/`, pas d'extension `.fifo`)
  
  2. **Implémenter `open_pipes_daemon(const char *run_dir, int *request_fd_out, int *reply_fd_out)`** :
     - Le démon lit les requêtes du client, donc il ouvre `request` en **lecture seule** (`O_RDONLY`)
     - Le démon écrit les réponses au client, donc il ouvre `reply` en **écriture seule** (`O_WRONLY`)
     - Stocker les descripteurs de fichiers dans les pointeurs fournis
     - Gérer les erreurs (fermer les descripteurs déjà ouverts en cas d'erreur)
     - **Important** : Ne pas utiliser `O_RDWR` (lecture+écriture) car ce n'est pas conforme aux spécifications
  
  3. **Implémenter `open_pipes_client(const char *run_dir, int *request_fd_out, int *reply_fd_out)`** :
     - Le client écrit les requêtes au démon, donc il ouvre `request` en **écriture seule** (`O_WRONLY`)
     - Le client lit les réponses du démon, donc il ouvre `reply` en **lecture seule** (`O_RDONLY`)
     - Stocker les descripteurs de fichiers dans les pointeurs fournis
     - Gérer les erreurs (fermer les descripteurs déjà ouverts en cas d'erreur)
  
  4. **Ajouter les prototypes dans `include/protocol.h`** :
     - Les prototypes doivent correspondre exactement aux signatures ci-dessus
     - Documenter chaque fonction avec des commentaires
  
  5. **Mettre à jour le Makefile** :
     - Ajouter `src/protocol.c` à `COMMON_SRCS` pour qu'il soit compilé avec `erraid` et `tadmor`
  
  **Pourquoi cette tâche est importante :** C'est la base de la communication. Sans les FIFO, le client et le démon ne peuvent pas communiquer. C'est comme installer les câbles avant de brancher les appareils. Toutes les autres tâches de communication dépendent de cette implémentation.
  
  **Points d'attention :**
  - Les FIFO bloquent : si on ouvre un FIFO en lecture, on bloque jusqu'à ce qu'un autre processus l'ouvre en écriture (et vice versa). C'est normal et attendu.
  - Les chemins doivent être construits avec `snprintf()` pour éviter les débordements
  - Toujours vérifier les valeurs de retour des appels système (`mkfifo()`, `open()`, etc.)
  - Utiliser `errno` pour diagnostiquer les erreurs
- **Livrable** : Fonctions `init_pipes`, `open_pipes_daemon`, `open_pipes_client` implémentées dans `src/protocol.c`, prototypes dans `include/protocol.h`, Makefile mis à jour, compilation réussie

---

### T2.3 - Module protocole (sérialisation messages)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 29 novembre 2025, 18h00
- **Deadline** : 1 décembre 2025, 18h00
- **Branche Git** : `feature/protocol-serialization`
- **Prérequis** : T2.1 (architecture protocole), T1.6 (sérialisation)
- **Responsable** : Pitel
- **Justification** : Pitel a implémenté toute la sérialisation au Jalon 1 (T1.5 et T1.6). Cette tâche réutilise directement ces compétences pour sérialiser les messages du protocole. C'est une continuité naturelle de son travail précédent.
- **Description** :
  Cette tâche consiste à implémenter la sérialisation et la désérialisation des messages du protocole. Tu dois convertir les structures `request_t` et `response_t` en séquences d'octets pour les envoyer sur les FIFO, et inversement.
  
  **Ce que tu dois faire :**
  1. **Implémenter `send_request(int fd, const request_t *req)`** dans `src/protocol.c` :
     - Écrire l'opcode (uint16_t, big-endian) sur le descripteur de fichier `fd`
     - Selon l'opcode, sérialiser les données correspondantes :
       - **OPCODE_LIST** ou **OPCODE_TERMINATE** : rien d'autre à envoyer
       - **OPCODE_CREATE** : timing (utiliser `write_timing()`), argc (uint32), puis chaque argument avec `write_string()`
       - **OPCODE_COMBINE** : timing, type (uint16), nbtasks (uint32), puis chaque taskid (uint64)
       - **OPCODE_REMOVE**, **OPCODE_TIMES_EXITCODES**, **OPCODE_STDOUT**, **OPCODE_STDERR** : taskid (uint64)
     - Utiliser les fonctions de sérialisation du Jalon 1 (`write_uint16`, `write_uint32`, `write_uint64`, `write_timing`, `write_string`)
     - Gérer les erreurs (vérifier les valeurs de retour de `write()`)
  
  2. **Implémenter `receive_request(int fd, request_t **req)`** :
     - Lire l'opcode (uint16_t, big-endian) depuis `fd`
     - Allouer une structure `request_t` avec `malloc()`
     - Initialiser l'opcode
     - Selon l'opcode, désérialiser les données :
       - **OPCODE_LIST** ou **OPCODE_TERMINATE** : rien d'autre à lire
       - **OPCODE_CREATE** : lire timing, argc, puis allouer et lire chaque argument
       - **OPCODE_COMBINE** : lire timing, type, nbtasks, puis allouer et lire chaque taskid
       - **OPCODE_REMOVE**, etc. : lire taskid
     - Stocker le pointeur dans `*req`
     - Gérer les erreurs (libérer la mémoire en cas d'échec)
  
  3. **Implémenter `send_response(int fd, const response_t *resp)`** :
     - Écrire l'anstype (uint16_t, big-endian)
     - Selon l'anstype et le contexte :
       - **ANSTYPE_OK** pour CREATE/COMBINE : taskid (uint64)
       - **ANSTYPE_OK** pour LIST : nbtasks (uint32), puis chaque tâche (utiliser `write_task()` ou sérialiser manuellement)
       - **ANSTYPE_OK** pour TIMES_EXITCODES : nbruns (uint32), puis chaque timestamp (int64) et exitcode (uint16)
       - **ANSTYPE_OK** pour STDOUT/STDERR : output comme string (longueur uint32 + données)
       - **ANSTYPE_OK** pour REMOVE/TERMINATE : rien d'autre
       - **ANSTYPE_ERROR** : errcode (uint16)
  
  4. **Implémenter `receive_response(int fd, response_t **resp)`** :
     - Lire l'anstype (uint16_t, big-endian)
     - Allouer une structure `response_t` avec `malloc()`
     - Initialiser l'anstype
     - Selon l'anstype et le contexte, désérialiser les données correspondantes
     - Stocker le pointeur dans `*resp`
     - Gérer les erreurs (libérer la mémoire en cas d'échec)
  
  5. **Implémenter `free_request(request_t *req)`** :
     - Libérer récursivement toute la mémoire allouée dans la requête
     - Pour CREATE : libérer le tableau argv et chaque string
     - Pour COMBINE : libérer le tableau taskids
     - Libérer la structure request_t elle-même
  
  6. **Implémenter `free_response(response_t *resp)`** :
     - Libérer récursivement toute la mémoire allouée dans la réponse
     - Pour LIST : libérer le tableau tasks (mais pas les tâches elles-mêmes, elles sont gérées ailleurs)
     - Pour TIMES_EXITCODES : libérer les tableaux timestamps et exitcodes
     - Pour STDOUT/STDERR : libérer la string output
     - Libérer la structure response_t elle-même
  
  **Pourquoi cette tâche est importante :** C'est le pont entre les structures de données en mémoire et les octets sur les FIFO. Sans cette sérialisation, les messages ne peuvent pas être transmis entre le client et le démon.
  
  **Points d'attention :**
  - Toujours utiliser le format big-endian pour la portabilité
  - Gérer correctement la mémoire : toute allocation doit avoir une libération correspondante
  - Vérifier les valeurs de retour de `read()` et `write()` (peuvent lire/écrire moins d'octets que demandé)
  - Pour les strings, envoyer d'abord la longueur (uint32) puis les données
  - Les fonctions doivent être robustes : vérifier les pointeurs NULL, gérer les erreurs proprement
- **Livrable** : Fonctions `send_request`, `receive_request`, `send_response`, `receive_response`, `free_request`, `free_response` implémentées dans `src/protocol.c`, compilation réussie, tests de base fonctionnels

---

### T2.4 - Démon - Intégration requêtes dans la boucle
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 1 décembre 2025, 18h00
- **Deadline** : 2 décembre 2025, 18h00
- **Branche Git** : `feature/request-handling`
- **Prérequis** : T2.2 (tubes nommés), T2.3 (sérialisation), T1.11 (boucle principale)
- **Responsable** : Ahmed
- **Justification** : Ahmed a fait T2.2 (tubes nommés), donc il connaît déjà le mécanisme de communication. Cette tâche modifie `daemon_loop` et utilise `select()` pour gérer les requêtes de manière non-bloquante, ce qui nécessite une bonne compréhension des appels système. C'est une extension logique de son travail sur les tubes nommés.
- **Description** :
  Cette tâche consiste à intégrer la gestion des requêtes client dans la boucle principale du démon. Le démon doit pouvoir répondre aux requêtes des clients tout en continuant à exécuter les tâches planifiées.
  
  **Ce que tu dois faire :**
  1. **Modifier `daemon_loop()` dans `src/erraid.c`** :
     - Au démarrage du démon, appeler `init_pipes(run_dir)` pour créer les FIFO
     - Ouvrir les FIFO avec `open_pipes_daemon()` pour obtenir les descripteurs
     - Utiliser `select()` pour gérer de manière non-bloquante :
       - Surveiller le descripteur de requêtes (si une requête arrive, `select()` retourne)
       - Surveiller avec un timeout de 1 seconde (pour vérifier périodiquement les tâches à exécuter)
     - Dans la boucle principale :
       - Si `select()` indique qu'une requête est disponible : appeler `handle_request()`
       - Si le timeout est atteint : vérifier et exécuter les tâches planifiées (comme avant)
     - Gérer proprement la fermeture des FIFO à l'arrêt du démon
  
  2. **Implémenter `handle_request(int request_fd, int reply_fd, const char *run_dir)`** :
     - Recevoir la requête avec `receive_request(request_fd, &req)`
     - Selon `req->opcode`, appeler la fonction de traitement appropriée :
       - **OPCODE_LIST** : `handle_list_request()` (sera implémenté dans T2.7)
       - **OPCODE_TIMES_EXITCODES** : `handle_times_request()` (sera implémenté dans T2.8)
       - **OPCODE_STDOUT** : `handle_stdout_request()` (sera implémenté dans T2.9)
       - **OPCODE_STDERR** : `handle_stderr_request()` (sera implémenté dans T2.9)
       - **OPCODE_TERMINATE** : arrêter le démon proprement (fermer FIFO, libérer mémoire, `_exit()`)
       - Pour l'instant, pour les requêtes non implémentées, renvoyer une réponse ERROR
     - Construire la réponse appropriée dans une structure `response_t`
     - Envoyer la réponse avec `send_response(reply_fd, &resp)`
     - Libérer la requête avec `free_request(req)`
     - Libérer la réponse avec `free_response(resp)`
     - Gérer les erreurs (si la réception/émission échoue, libérer la mémoire et continuer)
  
  3. **Gestion des signaux** :
     - Le démon doit toujours pouvoir être arrêté avec SIGINT/SIGTERM
     - À l'arrêt, fermer proprement les FIFO avant de quitter
  
  **Pourquoi cette tâche est importante :** C'est le cœur de la communication. Le démon doit être capable de gérer plusieurs activités simultanément : exécuter les tâches planifiées ET répondre aux requêtes des clients. Sans cette intégration, le démon ne peut pas communiquer avec les clients.
  
  **Points d'attention :**
  - `select()` permet de gérer plusieurs descripteurs de fichiers de manière non-bloquante
  - Le timeout de 1 seconde permet de vérifier périodiquement les tâches à exécuter
  - Toujours libérer la mémoire après avoir traité une requête
  - Gérer les erreurs de manière robuste : si une requête échoue, le démon doit continuer à fonctionner
  - Les requêtes non implémentées (CREATE, REMOVE, COMBINE) seront ajoutées au Jalon 3, pour l'instant renvoyer ERROR
- **Livrable** : `daemon_loop()` modifié pour gérer les requêtes avec `select()`, fonction `handle_request()` implémentée, démon répondant aux requêtes tout en exécutant les tâches planifiées

---

### T2.5 - Client - Parsing des arguments
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 2 décembre 2025, 18h00
- **Deadline** : 3 décembre 2025, 18h00
- **Branche Git** : `feature/client-parsing`
- **Prérequis** : T2.3 (protocole sérialisation)
- **Responsable** : Pitel
- **Justification** : Pitel a fait T2.3 (sérialisation messages), donc il connaît le format des requêtes. Le parsing des arguments client doit construire ces requêtes, donc c'est une continuité logique. De plus, cette tâche est indépendante côté client et peut être faite en parallèle avec T2.4.
- **Description** :
  Cette tâche consiste à implémenter le parsing des arguments de la ligne de commande du client `tadmor`. Le client doit pouvoir interpréter toutes les options et construire les requêtes appropriées.
  
  **Ce que tu dois faire :**
  1. **Implémenter le parsing avec `getopt()` dans `main()` de `src/tadmor.c`** :
     - Utiliser `getopt()` pour parser les options de manière standard
     - Gérer les options courtes (`-l`, `-x`, etc.) et longues (`--list`, `--times-exitcodes`, etc.) si nécessaire
     - Stocker les résultats du parsing dans des variables appropriées
  
  2. **Parser les options consultatives** (pour le Jalon 2) :
     - **`-l` ou `--list`** : lister toutes les tâches (OPCODE_LIST)
     - **`-x <taskid>` ou `--times-exitcodes <taskid>`** : obtenir l'historique d'exécution (OPCODE_TIMES_EXITCODES)
     - **`-o <taskid>` ou `--stdout <taskid>`** : obtenir la sortie standard (OPCODE_STDOUT)
     - **`-e <taskid>` ou `--stderr <taskid>`** : obtenir la sortie d'erreur (OPCODE_STDERR)
     - **`-q` ou `--terminate`** : terminer le démon (OPCODE_TERMINATE)
  
  3. **Parser les options de timing** (pour le Jalon 3, mais préparer la structure) :
     - **`-m <minutes>`** : minutes (ex: `0,30` ou `*` pour toutes)
     - **`-H <heures>`** : heures (ex: `0,12` ou `*` pour toutes)
     - **`-d <jours>`** : jours de la semaine (ex: `0,3,6` ou `*` pour tous)
     - **`-n`** : pas de timing (tâche abstraite, tous les bits à 0)
     - Convertir les chaînes en structures `timing_t` (utiliser les fonctions du Jalon 1)
  
  4. **Parser les arguments de commande** :
     - **`-c <command> [args...]`** : commande simple à créer
     - Tout ce qui suit `-c` jusqu'à la fin ou jusqu'à la prochaine option est la commande
     - Stocker dans un tableau `argv` avec `argc`
  
  5. **Parser les listes de taskids** :
     - **`-s <taskid1> <taskid2> ...`** : liste de taskids pour COMBINE
     - Parser chaque taskid comme un `uint64_t` (utiliser `strtoull()`)
     - Valider que ce sont des nombres valides
  
  6. **Validation des arguments** :
     - Vérifier que les options sont cohérentes (pas de `-l` avec `-x`, etc.)
     - Vérifier que les taskids sont valides (nombres positifs)
     - Vérifier que les timings sont valides (minutes 0-59, heures 0-23, jours 0-6)
     - Afficher un message d'erreur explicite si les arguments sont invalides
     - Afficher l'usage avec `--help` ou `-h`
  
  7. **Construire la structure `request_t`** :
     - Selon les options parsées, construire la requête appropriée
     - Pour l'instant, se concentrer sur les requêtes consultatives (LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE)
     - Les requêtes de modification (CREATE, REMOVE, COMBINE) seront implémentées au Jalon 3
  
  **Pourquoi cette tâche est importante :** C'est l'interface utilisateur du client. Sans un bon parsing, l'utilisateur ne peut pas utiliser le client correctement. Le parsing doit être robuste et gérer tous les cas d'erreur.
  
  **Points d'attention :**
  - Utiliser `getopt()` de manière standard (boucle `while((opt = getopt(...)) != -1)`)
  - Gérer les arguments positionnels (taskids, commandes) après les options
  - Valider toutes les entrées utilisateur avant de construire les requêtes
  - Afficher des messages d'erreur clairs et utiles
  - Gérer `--help` pour afficher l'usage
- **Livrable** : Parsing complet des arguments dans `src/tadmor.c`, validation des arguments, construction des structures `request_t` selon les options, affichage de l'usage avec `--help`

---

### T2.6 - Client - Requêtes consultatives
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 3 décembre 2025, 18h00
- **Deadline** : 4 décembre 2025, 18h00
- **Branche Git** : `feature/client-queries`
- **Prérequis** : T2.5 (parsing arguments), T2.3 (protocole)
- **Responsable** : Pitel
- **Justification** : Pitel a fait T2.5 (parsing), donc il connaît déjà comment les arguments sont parsés. Cette tâche utilise ces arguments parsés pour envoyer les requêtes et formater les réponses. C'est une suite logique de son travail sur le client. Cela regroupe toutes les fonctionnalités client dans un même flux de travail.
- **Description** :
  Cette tâche consiste à implémenter toutes les commandes consultatives du client. Le client doit pouvoir envoyer des requêtes au démon, recevoir les réponses, et les afficher de manière lisible pour l'utilisateur.
  
  **Ce que tu dois faire :**
  1. **Implémenter la fonction principale `main()` dans `src/tadmor.c`** :
     - Parser les arguments (déjà fait dans T2.5)
     - Ouvrir les FIFO avec `open_pipes_client(run_dir, &request_fd, &reply_fd)`
     - Selon l'option parsée, appeler la fonction de commande appropriée
     - Fermer les FIFO après utilisation
     - Gérer les erreurs (afficher un message et quitter avec le code approprié)
  
  2. **Implémenter `cmd_list(int request_fd, int reply_fd)`** :
     - Construire une requête `request_t` avec `OPCODE_LIST`
     - Envoyer la requête avec `send_request(request_fd, &req)`
     - Recevoir la réponse avec `receive_response(reply_fd, &resp)`
     - Vérifier `resp->anstype` :
       - Si `ANSTYPE_OK` : afficher la liste des tâches de manière lisible
         - Pour chaque tâche : afficher l'ID, le timing formaté, et la commande formatée
         - Utiliser `format_timing_display()` et `format_command_line()` si disponibles
       - Si `ANSTYPE_ERROR` : afficher le message d'erreur approprié
     - Libérer la requête et la réponse
  
  3. **Implémenter `cmd_times_exitcodes(int request_fd, int reply_fd, uint64_t taskid)`** :
     - Construire une requête `request_t` avec `OPCODE_TIMES_EXITCODES` et le taskid
     - Envoyer et recevoir comme pour `cmd_list`
     - Si `ANSTYPE_OK` : afficher l'historique d'exécution
       - Pour chaque exécution : afficher le timestamp (format lisible, ex: "2025-11-24 14:30:00") et le code de retour
       - Si aucune exécution : afficher un message approprié
     - Si `ANSTYPE_ERROR` : afficher le message d'erreur (NOT_FOUND, NOT_RUN, etc.)
  
  4. **Implémenter `cmd_stdout(int request_fd, int reply_fd, uint64_t taskid)`** :
     - Construire une requête avec `OPCODE_STDOUT` et le taskid
     - Envoyer et recevoir
     - Si `ANSTYPE_OK` : afficher le contenu de `resp->u.output_ok.output`
       - Afficher tel quel (c'est déjà une string)
       - Si vide, afficher un message approprié
     - Si `ANSTYPE_ERROR` : afficher le message d'erreur
  
  5. **Implémenter `cmd_stderr(int request_fd, int reply_fd, uint64_t taskid)`** :
     - Similaire à `cmd_stdout`, mais avec `OPCODE_STDERR`
  
  6. **Implémenter `cmd_terminate(int request_fd, int reply_fd)`** :
     - Construire une requête avec `OPCODE_TERMINATE`
     - Envoyer la requête
     - Recevoir la réponse (le démon devrait répondre OK avant de s'arrêter)
     - Afficher un message de confirmation
  
  7. **Formatage de l'affichage** :
     - Les timestamps doivent être affichés de manière lisible (ex: "2025-11-24 14:30:00")
     - Les commandes doivent être formatées de manière lisible (ex: "echo hello" ou "(cmd1; cmd2; cmd3)")
     - Les timings doivent être formatés (ex: "* * *" ou "0,30 * *")
     - Utiliser des couleurs ou une mise en forme claire si possible (optionnel)
  
  8. **Gestion des erreurs** :
     - Si l'ouverture des FIFO échoue : afficher "Erreur: impossible de se connecter au démon"
     - Si l'envoi/réception échoue : afficher "Erreur: communication avec le démon échouée"
     - Si la réponse est ERROR : afficher le message d'erreur approprié selon `errcode`
     - Toujours libérer la mémoire même en cas d'erreur
  
  **Pourquoi cette tâche est importante :** C'est l'interface utilisateur finale. L'utilisateur interagit avec le client, et le client doit afficher les résultats de manière claire et lisible. Sans cette implémentation, le client ne peut pas être utilisé.
  
  **Points d'attention :**
  - Toujours vérifier les valeurs de retour des fonctions (send, receive, open)
  - Libérer la mémoire après chaque requête/réponse
  - Gérer les cas où le démon n'est pas démarré (erreur d'ouverture des FIFO)
  - Afficher des messages d'erreur clairs et utiles pour l'utilisateur
  - Le formatage doit être cohérent et professionnel
- **Livrable** : Toutes les fonctions `cmd_*` implémentées dans `src/tadmor.c`, client fonctionnel pour toutes les commandes consultatives, affichage lisible et professionnel, gestion d'erreurs complète

---

### T2.7 - Démon - Traitement requête LIST
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 4 décembre 2025, 18h00
- **Deadline** : 5 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-list`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.7 (lecture arborescence)
- **Responsable** : Ahmed
- **Justification** : Ahmed a fait T2.4 (intégration requêtes dans la boucle), donc il connaît déjà `handle_request` et le mécanisme de traitement. Cette tâche implémente une requête spécifique dans ce framework qu'il a mis en place. De plus, Ahmed a travaillé sur la lecture d'arborescence au Jalon 1 (T1.7), donc il maîtrise `load_task_from_dir` et `list_all_tasks`.
- **Description** :
  Cette tâche consiste à implémenter le traitement de la requête LIST dans le démon. Quand un client demande la liste de toutes les tâches, le démon doit charger toutes les tâches depuis le disque et les renvoyer au client.
  
  **Ce que tu dois faire :**
  1. **Implémenter `handle_list_request(int reply_fd, const char *run_dir)` dans `src/erraid.c`** :
     - Cette fonction est appelée depuis `handle_request()` quand `req->opcode == OPCODE_LIST`
     - Charger toutes les tâches depuis le répertoire `run_dir/tasks/` avec `list_all_tasks()` (fonction du Jalon 1)
     - Si aucune tâche n'est trouvée : `nbtasks = 0`, tableau vide
     - Si une erreur survient lors du chargement : renvoyer une réponse ERROR
  
  2. **Construire la réponse `response_t`** :
     - Créer une structure `response_t` avec `malloc()`
     - Initialiser `anstype = ANSTYPE_OK`
     - Initialiser `u.list_ok.nbtasks` avec le nombre de tâches chargées
     - Allouer un tableau `task_t **tasks` de taille `nbtasks`
     - Remplir le tableau avec les pointeurs vers les tâches chargées
     - **Important** : Ne pas libérer les tâches ici, elles seront libérées ailleurs (ou pas du tout si elles sont utilisées ailleurs)
  
  3. **Envoyer la réponse** :
     - Appeler `send_response(reply_fd, &resp)`
     - La fonction `send_response()` doit sérialiser :
       - `anstype` (uint16)
       - `nbtasks` (uint32)
       - Pour chaque tâche : sérialiser la tâche complète (utiliser les fonctions de sérialisation du Jalon 1)
     - Gérer les erreurs d'envoi
  
  4. **Libérer la mémoire** :
     - Après l'envoi, libérer le tableau `tasks` (mais pas les tâches elles-mêmes, elles peuvent être utilisées ailleurs)
     - Libérer la structure `response_t` avec `free_response()`
     - **Note** : Les tâches chargées peuvent être gardées en mémoire pour éviter de les recharger à chaque requête (optimisation future)
  
  5. **Gestion des erreurs** :
     - Si `list_all_tasks()` échoue : renvoyer ERROR avec un message approprié
     - Si l'allocation mémoire échoue : libérer ce qui a été alloué et renvoyer ERROR
     - Si `send_response()` échoue : libérer la mémoire et retourner une erreur
  
  **Pourquoi cette tâche est importante :** C'est la première requête consultative implémentée. Elle permet au client de voir toutes les tâches gérées par le démon. C'est une fonctionnalité de base essentielle.
  
  **Points d'attention :**
  - Utiliser `list_all_tasks()` du Jalon 1 pour charger les tâches
  - Gérer correctement la mémoire : allouer le tableau de pointeurs, mais ne pas libérer les tâches
  - La sérialisation des tâches doit utiliser les fonctions du Jalon 1 (`write_task()` ou équivalent)
  - Gérer le cas où il n'y a aucune tâche (nbtasks = 0)
  - Les erreurs doivent être gérées proprement sans faire planter le démon
- **Livrable** : Fonction `handle_list_request()` implémentée, requête LIST fonctionnelle, démon renvoyant la liste complète des tâches au client

---

### T2.8 - Démon - Traitement requête TIMES_EXITCODES
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 5 décembre 2025, 18h00
- **Deadline** : 6 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-times`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.12 (logs)
- **Responsable** : AJINOU Abderrahman
- **Justification** : AJINOU a fait T1.11 (boucle principale du démon) au Jalon 1, donc il connaît bien l'architecture du démon et l'orchestration des tâches. Cette tâche traite les logs d'exécution (times-exitcodes) qui sont directement liés à l'exécution des tâches qu'il a orchestrée. Cela lui permet d'avoir une vue d'ensemble du démon (architecture, logs, intégration) avant l'intégration finale (T2.10). C'est une extension naturelle de son travail sur le démon.
- **Description** :
  Cette tâche consiste à implémenter le traitement de la requête TIMES_EXITCODES dans le démon. Quand un client demande l'historique d'exécution d'une tâche, le démon doit lire le fichier `times-exitcodes` de cette tâche et renvoyer les données au client.
  
  **Ce que tu dois faire :**
  1. **Implémenter `read_execution_logs(uint64_t taskid, const char *run_dir, int64_t **timestamps_out, uint16_t **exitcodes_out, uint32_t *nbruns_out)` dans `src/erraid.c`** :
     - Construire le chemin du fichier : `run_dir/tasks/<taskid>/times-exitcodes`
     - Ouvrir le fichier en lecture
     - Si le fichier n'existe pas : `*nbruns_out = 0`, retourner 0 (pas d'erreur, juste aucune exécution)
     - Lire le fichier binaire :
       - Format : chaque entrée est `int64_t timestamp` (big-endian) suivi de `uint16_t exitcode` (big-endian)
       - Lire toutes les entrées jusqu'à la fin du fichier
       - Allouer deux tableaux : un pour les timestamps, un pour les exitcodes
       - Convertir du big-endian vers l'endianness native avec `be64toh()` et `be16toh()`
     - Stocker les résultats dans les pointeurs fournis
     - Retourner 0 en cas de succès, -1 en cas d'erreur
  
  2. **Implémenter `handle_times_request(int reply_fd, uint64_t taskid, const char *run_dir)`** :
     - Cette fonction est appelée depuis `handle_request()` quand `req->opcode == OPCODE_TIMES_EXITCODES`
     - Vérifier que la tâche existe : construire le chemin `run_dir/tasks/<taskid>/` et vérifier avec `stat()`
     - Si la tâche n'existe pas : renvoyer une réponse ERROR avec `ERRCODE_NOT_FOUND`
     - Appeler `read_execution_logs()` pour lire les logs
     - Si `nbruns == 0` : renvoyer une réponse ERROR avec `ERRCODE_NOT_RUN` (tâche jamais exécutée)
     - Sinon, construire la réponse `response_t` :
       - `anstype = ANSTYPE_OK`
       - `u.times_exitcodes_ok.nbruns = nbruns`
       - `u.times_exitcodes_ok.timestamps = timestamps` (tableau alloué)
       - `u.times_exitcodes_ok.exitcodes = exitcodes` (tableau alloué)
  
  3. **Envoyer la réponse** :
     - Appeler `send_response(reply_fd, &resp)`
     - La fonction `send_response()` doit sérialiser :
       - `anstype` (uint16)
       - `nbruns` (uint32)
       - Pour chaque exécution : timestamp (int64, big-endian) puis exitcode (uint16, big-endian)
     - Gérer les erreurs d'envoi
  
  4. **Libérer la mémoire** :
     - Après l'envoi, libérer les tableaux `timestamps` et `exitcodes`
     - Libérer la structure `response_t` avec `free_response()`
  
  5. **Gestion des erreurs** :
     - Si la tâche n'existe pas : ERROR avec `ERRCODE_NOT_FOUND`
     - Si la tâche n'a jamais été exécutée (fichier inexistant ou vide) : ERROR avec `ERRCODE_NOT_RUN`
     - Si la lecture du fichier échoue : ERROR avec un message approprié
     - Si l'allocation mémoire échoue : libérer et renvoyer ERROR
  
  **Pourquoi cette tâche est importante :** C'est une fonctionnalité essentielle pour le débogage et le suivi. L'utilisateur doit pouvoir voir l'historique d'exécution de ses tâches pour comprendre ce qui s'est passé.
  
  **Points d'attention :**
  - Le format binaire `times-exitcodes` est défini au Jalon 1 : `int64_t` (big-endian) + `uint16_t` (big-endian) par entrée
  - Utiliser `be64toh()` et `be16toh()` pour la conversion d'endianness
  - Gérer le cas où le fichier n'existe pas (tâche jamais exécutée) : ce n'est pas une erreur fatale, mais `ERRCODE_NOT_RUN`
  - Vérifier que la tâche existe avant de lire les logs
  - Les erreurs doivent être gérées proprement sans faire planter le démon
- **Livrable** : Fonctions `read_execution_logs()` et `handle_times_request()` implémentées, requête TIMES_EXITCODES fonctionnelle, démon renvoyant l'historique d'exécution au client

---

### T2.9 - Démon - Traitement requêtes STDOUT/STDERR
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 6 décembre 2025, 18h00
- **Deadline** : 7 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-outputs`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.12 (logs)
- **Responsable** : AJINOU Abderrahman
- **Justification** : AJINOU a fait T1.9 (exécution commandes simples) et T1.11 (boucle principale) au Jalon 1, donc il connaît bien comment les sorties stdout/stderr sont capturées et sauvegardées lors de l'exécution. Cette tâche lit ces fichiers sauvegardés pour les renvoyer au client, ce qui est cohérent avec son travail précédent sur l'exécution. Cela complète sa série de tâches sur le démon (T2.1 architecture, T2.8 logs, T2.9 sorties, T2.10 intégration) et lui donne une vue complète avant l'intégration finale.
- **Description** :
  Cette tâche consiste à implémenter le traitement des requêtes STDOUT et STDERR dans le démon. Quand un client demande la sortie standard ou d'erreur d'une tâche, le démon doit lire le fichier correspondant et renvoyer son contenu au client.
  
  **Ce que tu dois faire :**
  1. **Implémenter `read_stdout(uint64_t taskid, const char *run_dir, char **output_out, size_t *len_out)` dans `src/erraid.c`** :
     - Construire le chemin du fichier : `run_dir/tasks/<taskid>/stdout`
     - Ouvrir le fichier en lecture
     - Si le fichier n'existe pas : `*len_out = 0`, `*output_out = NULL`, retourner -1 (tâche jamais exécutée)
     - Lire tout le contenu du fichier :
       - Utiliser `stat()` pour obtenir la taille du fichier
       - Allouer un buffer de la taille appropriée
       - Lire tout le contenu avec `read()`
       - Stocker le contenu dans `*output_out` et la longueur dans `*len_out`
       - **Important** : Le contenu est binaire (peut contenir des '\0'), donc utiliser `len` pour connaître la taille réelle
     - Retourner 0 en cas de succès, -1 en cas d'erreur
  
  2. **Implémenter `read_stderr(uint64_t taskid, const char *run_dir, char **output_out, size_t *len_out)`** :
     - Similaire à `read_stdout`, mais lit le fichier `run_dir/tasks/<taskid>/stderr`
  
  3. **Implémenter `handle_stdout_request(int reply_fd, uint64_t taskid, const char *run_dir)`** :
     - Cette fonction est appelée depuis `handle_request()` quand `req->opcode == OPCODE_STDOUT`
     - Vérifier que la tâche existe : construire le chemin `run_dir/tasks/<taskid>/` et vérifier avec `stat()`
     - Si la tâche n'existe pas : renvoyer une réponse ERROR avec `ERRCODE_NOT_FOUND`
     - Appeler `read_stdout()` pour lire la sortie
     - Si `read_stdout()` retourne -1 (fichier inexistant) : renvoyer ERROR avec `ERRCODE_NOT_RUN`
     - Sinon, construire la réponse `response_t` :
       - `anstype = ANSTYPE_OK`
       - `u.output_ok.output = output` (buffer alloué)
       - `u.output_ok.len = len` (longueur en octets, sans '\0' final)
  
  4. **Implémenter `handle_stderr_request(int reply_fd, uint64_t taskid, const char *run_dir)`** :
     - Similaire à `handle_stdout_request`, mais utilise `read_stderr()` et `OPCODE_STDERR`
  
  5. **Envoyer la réponse** :
     - Appeler `send_response(reply_fd, &resp)`
     - La fonction `send_response()` doit sérialiser :
       - `anstype` (uint16)
       - `len` (uint32) : longueur en octets
       - `output` : les `len` octets du contenu (peut contenir des '\0')
     - Gérer les erreurs d'envoi
  
  6. **Libérer la mémoire** :
     - Après l'envoi, libérer le buffer `output`
     - Libérer la structure `response_t` avec `free_response()`
  
  7. **Gestion des erreurs** :
     - Si la tâche n'existe pas : ERROR avec `ERRCODE_NOT_FOUND`
     - Si la tâche n'a jamais été exécutée (fichier inexistant) : ERROR avec `ERRCODE_NOT_RUN`
     - Si la lecture du fichier échoue : ERROR avec un message approprié
     - Si l'allocation mémoire échoue : libérer et renvoyer ERROR
  
  **Pourquoi cette tâche est importante :** C'est essentiel pour le débogage. L'utilisateur doit pouvoir voir ce que ses tâches ont produit comme sortie pour comprendre ce qui s'est passé lors de l'exécution.
  
  **Points d'attention :**
  - Les fichiers `stdout` et `stderr` sont des fichiers binaires (peuvent contenir des '\0')
  - Utiliser `len` pour connaître la taille réelle, ne pas utiliser `strlen()` qui s'arrête au premier '\0'
  - Gérer le cas où le fichier n'existe pas (tâche jamais exécutée) : `ERRCODE_NOT_RUN`
  - Vérifier que la tâche existe avant de lire les fichiers
  - Les erreurs doivent être gérées proprement sans faire planter le démon
  - Le contenu peut être volumineux, donc allouer dynamiquement selon la taille du fichier
- **Livrable** : Fonctions `read_stdout()`, `read_stderr()`, `handle_stdout_request()`, `handle_stderr_request()` implémentées, requêtes STDOUT/STDERR fonctionnelles, démon renvoyant les sorties au client

---

### T2.10 - Intégration et tests du jalon 2
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 6 décembre 2025, 18h00
- **Deadline** : 8 décembre 2025, 18h00
- **Branche Git** : `integration/jalon-2`
- **Prérequis** : T2.6 (client consultatif), T2.7, T2.8, T2.9 (toutes requêtes démon)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  Cette tâche consiste à intégrer toutes les fonctionnalités du Jalon 2 et à effectuer des tests complets pour s'assurer que tout fonctionne correctement ensemble.
  
  **Ce que tu dois faire :**
  1. **Vérifier l'intégration complète** :
     - S'assurer que toutes les branches sont mergées dans `jalon2`
     - Vérifier que le code compile sans erreurs ni warnings
     - Vérifier que tous les modules sont correctement liés dans le Makefile
  
  2. **Tests de toutes les requêtes consultatives** :
     - **Test LIST** : démarrer le démon, créer quelques tâches manuellement, lancer `tadmor -l`, vérifier que toutes les tâches sont listées correctement
     - **Test TIMES_EXITCODES** : attendre qu'une tâche s'exécute, lancer `tadmor -x <taskid>`, vérifier que l'historique est correct
     - **Test STDOUT** : attendre qu'une tâche s'exécute, lancer `tadmor -o <taskid>`, vérifier que la sortie est correcte
     - **Test STDERR** : attendre qu'une tâche s'exécute, lancer `tadmor -e <taskid>`, vérifier que la sortie d'erreur est correcte
     - **Test TERMINATE** : lancer `tadmor -q`, vérifier que le démon s'arrête proprement
  
  3. **Tests de communication client-serveur** :
     - Tester que plusieurs requêtes consécutives fonctionnent
     - Tester que le démon continue à exécuter les tâches planifiées pendant qu'il répond aux requêtes
     - Tester avec plusieurs clients séquentiels (un après l'autre)
     - Vérifier que les FIFO sont correctement gérés (création, ouverture, fermeture)
  
  4. **Tests de robustesse** :
     - Tester avec un taskid inexistant : doit renvoyer `ERRCODE_NOT_FOUND`
     - Tester avec une tâche jamais exécutée : doit renvoyer `ERRCODE_NOT_RUN`
     - Tester avec le démon non démarré : le client doit afficher un message d'erreur clair
     - Tester l'arrêt propre du démon (SIGINT, SIGTERM, TERMINATE)
     - Vérifier qu'il n'y a pas de fuites mémoire (utiliser `valgrind` si possible)
  
  5. **Tests de cas limites** :
     - Tester avec une tâche qui n'a aucune exécution (fichier times-exitcodes vide)
     - Tester avec des sorties vides
     - Tester avec des sorties contenant des '\0' (binaires)
     - Tester avec un grand nombre de tâches (LIST)
     - Tester avec un grand nombre d'exécutions (TIMES_EXITCODES)
  
  6. **Corrections de bugs** :
     - Corriger tous les bugs trouvés lors des tests
     - Vérifier que les corrections n'introduisent pas de régressions
     - Re-tester après chaque correction
  
  7. **Vérification finale** :
     - Compilation sans warnings
     - Tous les tests passent
     - Code propre et commenté
     - Documentation à jour
  
  8. **Création du tag** :
     - Merger toutes les branches dans `jalon2`
     - Créer le tag `jalon-2` : `git tag jalon-2`
     - Pousser le tag : `git push origin jalon-2`
  
  **Pourquoi cette tâche est importante :** C'est la validation finale du Jalon 2. Sans des tests complets, on ne peut pas être sûr que tout fonctionne correctement. Cette tâche garantit la qualité et la robustesse du système.
  
  **Points d'attention :**
  - Tester systématiquement chaque fonctionnalité
  - Documenter les bugs trouvés et leurs corrections
  - Vérifier la gestion mémoire (pas de fuites)
  - S'assurer que le démon reste stable même en cas d'erreurs
  - Les tests doivent être reproductibles
- **Livrable** : Projet fonctionnel pour jalon 2, tous les tests passent, code propre, tag `jalon-2` créé et pushé

---

## 🎯 RENDU FINAL - 10 janvier 2026

### T3.1 - Requêtes de modification (CREATE, REMOVE, COMBINE)
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 6h
- **Date de début** : 12 décembre 2025, 14h00
- **Deadline** : 15 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-modify`
- **Prérequis** : T2.3 (jalon 2 terminé), T1.8 (écriture arborescence)
- **Responsable** : À assigner
- **Description** :
  - Implémenter traitement `OPCODE_CREATE` (création tâche simple)
  - Implémenter traitement `OPCODE_REMOVE` (suppression tâche)
  - Implémenter traitement `OPCODE_COMBINE` (combinaison tâches)
  - Gestion des erreurs (tâche non trouvée, etc.)
  - Génération d'IDs uniques
- **Livrable** : Toutes les requêtes de modification fonctionnelles

---

### T3.2 - Client - Commandes de modification
- **Difficulté** : **\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 5h
- **Date de début** : 15 décembre 2025, 18h00
- **Deadline** : 17 décembre 2025, 18h00
- **Branche Git** : `feature/client-modify`
- **Prérequis** : T3.1 (requêtes modification), T2.5 (parsing)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `cmd_create` (création de tâche)
  - Implémenter `cmd_remove` (suppression)
  - Implémenter `cmd_combine` (combinaison)
  - Implémenter `cmd_terminate` (arrêt du démon)
  - Parser les timings pour CREATE et COMBINE
- **Livrable** : Toutes les commandes client fonctionnelles

---

### T3.3 - Gestion de la persistance et reprise
- **Difficulté** : **\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 4h
- **Date de début** : 17 décembre 2025, 18h00
- **Deadline** : 19 décembre 2025, 18h00
- **Branche Git** : `feature/persistence`
- **Prérequis** : T3.1 (requêtes modification), T1.7 (lecture)
- **Responsable** : À assigner
- **Description** :
  - Vérifier que le démon peut reprendre après redémarrage
  - Tests de persistance des données
  - Gestion des IDs uniques (non réutilisation après suppression)
  - Implémenter `generate_task_id` correctement
- **Livrable** : Système de persistance robuste

---

### T3.5 - Gestion des tâches abstraites
- **Difficulté** : **\***
- **Jalon** : Rendu final
- **Durée estimée** : 3h
- **Date de début** : 15 décembre 2025, 18h00
- **Deadline** : 16 décembre 2025, 18h00
- **Branche Git** : `feature/abstract-tasks`
- **Prérequis** : T3.2 (client modification), T1.11 (planification)
- **Responsable** : À assigner
- **Description** :
  - Support de l'option `-n` (pas de timing)
  - Tâches sans horaire d'exécution (timing tous à 0)
  - Utilisation dans les combinaisons
  - Vérifier que les tâches abstraites ne s'exécutent pas automatiquement
- **Livrable** : Tâches abstraites fonctionnelles

---

### T3.6 - Formatage et affichage avancé
- **Difficulté** : **\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 4h
- **Date de début** : 16 décembre 2025, 18h00
- **Deadline** : 18 décembre 2025, 18h00
- **Branche Git** : `feature/formatting`
- **Prérequis** : T3.2 (client modification)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `format_timing_display` (affichage timing lisible)
  - Implémenter `format_command_line` (affichage commandes)
  - Formatage des séquences : `(cmd1; cmd2; cmd3)`
  - Formatage timing : `* * *` ou `0,30 * *` ou `- - -`
- **Livrable** : Affichage professionnel et lisible

---

### T3.7 - Gestion des erreurs et robustesse
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 5h
- **Date de début** : 2 janvier 2026, 14h00
- **Deadline** : 4 janvier 2026, 18h00
- **Branche Git** : `feature/error-handling`
- **Prérequis** : T3.2 (toutes fonctionnalités)
- **Responsable** : À assigner
- **Description** :
  - Gestion complète des erreurs système
  - Messages d'erreur explicites pour l'utilisateur
  - Gestion de la mémoire (pas de fuites, vérification avec valgrind)
  - Validation des entrées utilisateur
  - Gestion des cas limites
- **Livrable** : Code robuste et sécurisé

---

### T3.8 - Tests exhaustifs
- **Difficulté** : **\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 6h
- **Date de début** : 19 décembre 2025, 18h00
- **Deadline** : 22 décembre 2025, 18h00
- **Branche Git** : `feature/comprehensive-tests`
- **Prérequis** : T3.2 (toutes fonctionnalités)
- **Responsable** : À assigner
- **Description** :
  - Tests unitaires pour chaque module
  - Tests d'intégration complets
  - Tests avec les exemples fournis
  - Tests de cas limites et d'erreurs
  - Tests de performance
- **Livrable** : Suite de tests complète

---

### T3.9 - Optimisations et nettoyage
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 4h
- **Date de début** : 4 janvier 2026, 18h00
- **Deadline** : 6 janvier 2026, 18h00
- **Branche Git** : `feature/optimization`
- **Prérequis** : T3.8 (tests)
- **Responsable** : À assigner
- **Description** :
  - Optimisation de la gestion mémoire
  - Nettoyage du code (commentaires, style uniforme)
  - Vérification des warnings de compilation
  - Optimisation des performances si nécessaire
  - Refactoring si besoin
- **Livrable** : Code propre et optimisé

---

### T3.10 - Gestion Git et branches
- **Difficulté** : **\***
- **Jalon** : Rendu final
- **Durée estimée** : 2h
- **Date de début** : 6 janvier 2026, 18h00
- **Deadline** : 7 janvier 2026, 18h00
- **Branche Git** : `feature/git-cleanup`
- **Prérequis** : T3.9 (optimisations)
- **Responsable** : À assigner
- **Description** :
  - Nettoyage de l'historique Git
  - Vérification que toutes les branches sont mergées
  - Messages de commit clairs et cohérents
  - Préparation pour les merges finaux
- **Livrable** : Historique Git propre

---

### T3.4 - Documentation et livrables finaux
- **Difficulté** : **\***
- **Jalon** : Rendu final
- **Durée estimée** : 6h
- **Date de début** : 2 janvier 2026, 14h00
- **Deadline** : 5 janvier 2026, 18h00
- **Branche Git** : `feature/documentation`
- **Prérequis** : T3.2 (toutes fonctionnalités)
- **Responsable** : À assigner
- **Description** :
  - Rédiger `README.md` complet avec exemples
  - Rédiger `ARCHITECTURE.md` détaillé
  - Créer `AUTHORS.md` avec liste des membres
  - Vérifier le Makefile (make, make clean, make distclean)
  - Documentation du code (commentaires)
- **Livrable** : Documentation complète

---

### Intégration finale
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu final
- **Durée estimée** : 8h
- **Date de début** : 7 janvier 2026, 18h00
- **Deadline** : 9 janvier 2026, 18h00
- **Branche Git** : `integration/rendu-final`
- **Prérequis** : Toutes les tâches précédentes
- **Responsable** : À assigner
- **Description** :
  - Intégration de tous les modules
  - Tests finaux complets
  - Vérification sur lulu
  - Corrections de derniers bugs
  - Création du tag `rendu-final`
- **Livrable** : Projet complet et fonctionnel, tag `rendu-final` créé

---

## 📊 Légende

- **Prérequis** : Tâches qui doivent être terminées avant de commencer celle-ci
- **Durée estimée** : Temps de développement prévu (peut varier selon l'expérience)
- **Deadline** : Date limite pour terminer la tâche
- **Branche Git** : Nom de la branche à créer pour cette tâche

## 🔄 Workflow recommandé

1. Choisir une tâche disponible (vérifier les prérequis)
2. Créer la branche Git correspondante
3. Développer et commiter régulièrement
4. Demander un merge via Pull Request
5. Le chef de projet review et merge
6. Passer à la tâche suivante

## ⚠️ Notes importantes

- Les durées sont des estimations, ajustez selon votre rythme
- Si une tâche prend plus de temps, communiquez avec l'équipe
- Les prérequis sont importants : ne pas commencer une tâche si ses prérequis ne sont pas terminés
- Tester régulièrement votre code avant de demander un merge

