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

## 🎯 JALON 2 - 12 décembre 2025

### T2.1 - Architecture du protocole de communication
- **Difficulté** : **\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 22 novembre 2025, 14h00
- **Deadline** : 24 novembre 2025, 18h00
- **Branche Git** : `feature/protocol-architecture`
- **Prérequis** : T1.3 (jalon 1 terminé)
- **Responsable** : À assigner
- **Description** :
  - Définir les structures `request_t` et `response_t`
  - Définir tous les opcodes (LIST, CREATE, COMBINE, REMOVE, etc.)
  - Définir les codes d'erreur (NOT_FOUND, NOT_RUN)
  - Spécifier le format exact de chaque message
  - Documenter le protocole
- **Livrable** : `protocol.h` avec structures complètes et documentation

---

### T2.2 - Gestion des tubes nommés (FIFO)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 24 novembre 2025, 18h00
- **Deadline** : 26 novembre 2025, 18h00
- **Branche Git** : `feature/named-pipes`
- **Prérequis** : T2.1 (architecture protocole)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `init_pipes` (création des FIFO avec mkfifo)
  - Implémenter `open_pipes` pour le démon (lecture request, écriture reply)
  - Implémenter `open_pipes` pour le client (écriture request, lecture reply)
  - Gestion des erreurs (fichiers existants, permissions)
  - Fermeture propre des tubes
- **Livrable** : Système de communication opérationnel

---

### T2.4 - Module protocole (sérialisation messages)
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 26 novembre 2025, 18h00
- **Deadline** : 28 novembre 2025, 18h00
- **Branche Git** : `feature/protocol-serialization`
- **Prérequis** : T2.1 (architecture protocole), T1.6 (sérialisation)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `send_request` (sérialisation de toutes les requêtes)
  - Implémenter `receive_request` (désérialisation)
  - Implémenter `send_response` et `receive_response`
  - Gestion de tous les types de requêtes (LIST, CREATE, COMBINE, etc.)
  - Gestion mémoire (allocation/désallocation)
- **Livrable** : `protocol.c` avec sérialisation messages complète

---

### T2.5 - Client - Parsing des arguments
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 28 novembre 2025, 18h00
- **Deadline** : 30 novembre 2025, 18h00
- **Branche Git** : `feature/client-parsing`
- **Prérequis** : T2.4 (protocole sérialisation)
- **Responsable** : À assigner
- **Description** :
  - Implémenter le parsing avec `getopt`
  - Parser les options `-l`, `-x`, `-o`, `-e`, `-c`, `-s`, `-r`, `-q`
  - Parser les timings `-m`, `-H`, `-d`, `-n`
  - Parser les arguments de commande après `-c`
  - Parser les listes de taskids après `-s`
  - Validation des arguments
- **Livrable** : Parsing complet dans `tadmor.c`

---

### T2.6 - Client - Requêtes consultatives
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 30 novembre 2025, 18h00
- **Deadline** : 2 décembre 2025, 18h00
- **Branche Git** : `feature/client-queries`
- **Prérequis** : T2.5 (parsing arguments), T2.4 (protocole)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `cmd_list` (liste des tâches)
  - Implémenter `cmd_times_exitcodes` (historique)
  - Implémenter `cmd_stdout` et `cmd_stderr` (sorties)
  - Formatage de l'affichage pour l'utilisateur
  - Gestion des erreurs (affichage messages explicites)
- **Livrable** : Toutes les commandes consultatives fonctionnelles

---

### T2.7 - Démon - Traitement requête LIST
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 28 novembre 2025, 18h00
- **Deadline** : 30 novembre 2025, 18h00
- **Branche Git** : `feature/daemon-list`
- **Prérequis** : T2.4 (protocole), T1.7 (lecture arborescence)
- **Responsable** : À assigner
- **Description** :
  - Implémenter traitement de `OPCODE_LIST` dans `handle_request`
  - Charger toutes les tâches depuis le disque avec `list_all_tasks`
  - Formater les commandes avec `format_command_line`
  - Envoyer la réponse au client (ANSTYPE_OK + NBTASKS + données)
  - Libérer la mémoire après envoi
- **Livrable** : Requête LIST fonctionnelle

---

### T2.8 - Démon - Traitement requête TIMES_EXITCODES
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 1er décembre 2025, 18h00
- **Deadline** : 3 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-times`
- **Prérequis** : T2.4 (protocole), T1.12 (logs)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `read_execution_logs` (lecture times-exitcodes)
  - Traitement de `OPCODE_TIMES_EXITCODES` dans `handle_request`
  - Vérifier que la tâche existe
  - Lire et envoyer les logs d'exécution
  - Gestion erreur si tâche non trouvée
- **Livrable** : Requête TIMES_EXITCODES fonctionnelle

---

### T2.9 - Démon - Traitement requêtes STDOUT/STDERR
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 3h
- **Date de début** : 3 décembre 2025, 18h00
- **Deadline** : 4 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-outputs`
- **Prérequis** : T2.4 (protocole), T1.12 (logs)
- **Responsable** : À assigner
- **Description** :
  - Implémenter `read_stdout` et `read_stderr`
  - Traitement de `OPCODE_STDOUT` et `OPCODE_STDERR`
  - Vérifier que la tâche existe et a été exécutée
  - Envoyer la sortie au format string
  - Gestion erreurs (NOT_FOUND, NOT_RUN)
- **Livrable** : Requêtes STDOUT/STDERR fonctionnelles

---

### T2.10 - Démon - Intégration requêtes dans la boucle
- **Difficulté** : **\*\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 4h
- **Date de début** : 26 novembre 2025, 18h00
- **Deadline** : 28 novembre 2025, 18h00
- **Branche Git** : `feature/request-handling`
- **Prérequis** : T2.2 (tubes nommés), T1.11 (boucle principale)
- **Responsable** : À assigner
- **Description** :
  - Modifier `daemon_loop` pour gérer les requêtes
  - Utiliser `select()` pour gérer requêtes et exécution de manière non-bloquante
  - Implémenter `handle_request` (orchestration de toutes les requêtes)
  - Gérer les timeouts (1 seconde pour vérifier les tâches)
- **Livrable** : Démon répondant aux requêtes client tout en exécutant les tâches

---

### T2.3 - Intégration et tests du jalon 2
- **Difficulté** : **\*\***
- **Jalon** : Jalon 2
- **Durée estimée** : 5h
- **Date de début** : 4 décembre 2025, 18h00
- **Deadline** : 11 décembre 2025, 18h00
- **Branche Git** : `integration/jalon-2`
- **Prérequis** : T2.6 (client consultatif), T2.7, T2.8, T2.9 (toutes requêtes démon)
- **Responsable** : À assigner
- **Description** :
  - Tests de toutes les requêtes consultatives
  - Tests de communication client-serveur
  - Tests avec plusieurs clients (séquentiels)
  - Vérification de la robustesse
  - Corrections de bugs
- **Livrable** : Projet fonctionnel pour jalon 2, tag `jalon-2` créé

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

