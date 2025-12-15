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
- **Date de début** : 27 novembre 2025, 23h00
- **Deadline** : 29 novembre 2025, 18h00
- **Branche Git** : `feature/named-pipes`
- **Prérequis** : T2.1 (architecture protocole)
- **Responsable** : Ahmed
- **Justification** : Ahmed a travaillé sur les appels système de base au Jalon 1 (task_tree avec gestion fichiers/répertoires). Les tubes nommés (FIFO) sont un mécanisme système similaire, donc cohérent avec ses compétences. Cette tâche est la base de la communication, donc logique qu'elle soit faite tôt par quelqu'un qui maîtrise les appels système.
- **Description** :
  - Implémenter `init_pipes` (création des FIFO avec mkfifo)
  - Implémenter `open_pipes` pour le démon (lecture request, écriture reply)
  - Implémenter `open_pipes` pour le client (écriture request, lecture reply)
  - Gestion des erreurs (fichiers existants, permissions)
  - Fermeture propre des tubes
- **Livrable** : Système de communication opérationnel

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
  - Implémenter `send_request` (sérialisation de toutes les requêtes)
  - Implémenter `receive_request` (désérialisation)
  - Implémenter `send_response` et `receive_response`
  - Gestion de tous les types de requêtes (LIST, CREATE, COMBINE, etc.)
  - Gestion mémoire (allocation/désallocation)
- **Livrable** : `protocol.c` avec sérialisation messages complète

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
  - Modifier `daemon_loop` pour gérer les requêtes
  - Utiliser `select()` pour gérer requêtes et exécution de manière non-bloquante
  - Implémenter `handle_request` (orchestration de toutes les requêtes)
  - Gérer les timeouts (1 seconde pour vérifier les tâches)
- **Livrable** : Démon répondant aux requêtes client tout en exécutant les tâches

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
- **Date de début** : 3 décembre 2025, 18h00
- **Deadline** : 4 décembre 2025, 18h00
- **Branche Git** : `feature/client-queries`
- **Prérequis** : T2.5 (parsing arguments), T2.3 (protocole)
- **Responsable** : Pitel
- **Justification** : Pitel a fait T2.5 (parsing), donc il connaît déjà comment les arguments sont parsés. Cette tâche utilise ces arguments parsés pour envoyer les requêtes et formater les réponses. C'est une suite logique de son travail sur le client. Cela regroupe toutes les fonctionnalités client dans un même flux de travail.
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
- **Date de début** : 4 décembre 2025, 18h00
- **Deadline** : 5 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-list`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.7 (lecture arborescence)
- **Responsable** : Ahmed
- **Justification** : Ahmed a fait T2.4 (intégration requêtes dans la boucle), donc il connaît déjà `handle_request` et le mécanisme de traitement. Cette tâche implémente une requête spécifique dans ce framework qu'il a mis en place. De plus, Ahmed a travaillé sur la lecture d'arborescence au Jalon 1 (T1.7), donc il maîtrise `load_task_from_dir` et `list_all_tasks`.
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
- **Date de début** : 5 décembre 2025, 18h00
- **Deadline** : 6 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-times`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.12 (logs)
- **Responsable** : AJINOU Abderrahman
- **Justification** : AJINOU a fait T1.11 (boucle principale du démon) au Jalon 1, donc il connaît bien l'architecture du démon et l'orchestration des tâches. Cette tâche traite les logs d'exécution (times-exitcodes) qui sont directement liés à l'exécution des tâches qu'il a orchestrée. Cela lui permet d'avoir une vue d'ensemble du démon (architecture, logs, intégration) avant l'intégration finale (T2.10). C'est une extension naturelle de son travail sur le démon.
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
- **Date de début** : 6 décembre 2025, 18h00
- **Deadline** : 7 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-outputs`
- **Prérequis** : T2.3 (protocole), T2.4 (intégration requêtes), T1.12 (logs)
- **Responsable** : AJINOU Abderrahman
- **Justification** : AJINOU a fait T1.9 (exécution commandes simples) et T1.11 (boucle principale) au Jalon 1, donc il connaît bien comment les sorties stdout/stderr sont capturées et sauvegardées lors de l'exécution. Cette tâche lit ces fichiers sauvegardés pour les renvoyer au client, ce qui est cohérent avec son travail précédent sur l'exécution. Cela complète sa série de tâches sur le démon (T2.1 architecture, T2.8 logs, T2.9 sorties, T2.10 intégration) et lui donne une vue complète avant l'intégration finale.
- **Description** :
  - Implémenter `read_stdout` et `read_stderr`
  - Traitement de `OPCODE_STDOUT` et `OPCODE_STDERR`
  - Vérifier que la tâche existe et a été exécutée
  - Envoyer la sortie au format string
  - Gestion erreurs (NOT_FOUND, NOT_RUN)
- **Livrable** : Requêtes STDOUT/STDERR fonctionnelles

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
  - Tests de toutes les requêtes consultatives
  - Tests de communication client-serveur
  - Tests avec plusieurs clients (séquentiels)
  - Vérification de la robustesse
  - Corrections de bugs
- **Livrable** : Projet fonctionnel pour jalon 2, tag `jalon-2` créé

---

## 🎯 RENDU FINAL - 10 janvier 2026

### T3.1 - Module protocole - Support CREATE, REMOVE, COMBINE
- **Difficulté** : **\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 4h
- **Date de début** : 19 décembre 2025, 14h00
- **Deadline** : 20 décembre 2025, 18h00
- **Branche Git** : `feature/protocol-rendu-final`
- **Prérequis** : T2.3 (protocole sérialisation), T2.10 (jalon 2 terminé)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  - Modifier `send_request()` pour supporter `OPCODE_CREATE` et `OPCODE_COMBINE`
  - Modifier `receive_request()` pour désérialiser CREATE et COMBINE
  - Modifier `send_response()` pour envoyer les réponses CREATE/REMOVE/COMBINE
  - Modifier `receive_response()` pour lire les réponses CREATE/REMOVE/COMBINE
  - Modifier `free_request()` pour libérer la mémoire de CREATE (argv) et COMBINE (taskids)
  - Gestion des erreurs et validation des données
- **Livrable** : `protocol.c` avec support complet de CREATE, REMOVE, COMBINE

---

### T3.2 - Module task_tree - Génération ID, suppression et combinaison
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 5h
- **Date de début** : 20 décembre 2025, 18h00
- **Deadline** : 22 décembre 2025, 18h00
- **Branche Git** : `feature/task-tree-rendu-final`
- **Prérequis** : T3.1 (protocole CREATE/REMOVE/COMBINE), T1.7 (lecture arborescence), T1.8 (écriture arborescence)
- **Responsable** : Ahmed Mouncef Chabira
- **Description** :
  - Implémenter `generate_task_id()` : parcourt le répertoire tasks/ pour trouver le max_id et retourne max_id + 1
  - Implémenter `remove_task()` : vérifie l'existence avec `stat()`, supprime récursivement avec `system("rm -rf")`, vérifie la suppression
  - Implémenter `combine_tasks()` : vérifie que toutes les tâches existent, génère un nouvel ID, crée la structure de la tâche combinée, copie les arborescences cmd/ de chaque tâche, supprime les tâches combinées (consommation)
  - Gestion des erreurs (ENOENT si tâche inexistante, EIO si erreur système)
- **Livrable** : `task_tree.c` avec `generate_task_id()`, `remove_task()`, `combine_tasks()` fonctionnelles

---

### T3.3 - Démon - Traitement requêtes CREATE, REMOVE, COMBINE
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 4h
- **Date de début** : 22 décembre 2025, 18h00
- **Deadline** : 23 décembre 2025, 18h00
- **Branche Git** : `feature/daemon-rendu-final`
- **Prérequis** : T3.2 (task_tree fonctions), T3.1 (protocole), T2.4 (intégration requêtes)
- **Responsable** : Jeremy Pitel
- **Description** :
  - Ajouter traitement de `OPCODE_CREATE` dans `handle_request()` : créer commande simple, générer ID, sauvegarder tâche
  - Ajouter traitement de `OPCODE_REMOVE` dans `handle_request()` : appeler `remove_task()`, gérer erreur NOT_FOUND
  - Ajouter traitement de `OPCODE_COMBINE` dans `handle_request()` : appeler `combine_tasks()`, gérer erreur NOT_FOUND
  - Ajouter option `-p` dans `main()` pour spécifier le répertoire des pipes (défaut: `<run_dir>/pipes`)
  - Modifier `init_pipes()` et `open_pipes_daemon()` pour utiliser le répertoire personnalisé
  - Gestion des erreurs et logs de debug
- **Livrable** : `erraid.c` avec traitement complet de CREATE, REMOVE, COMBINE et option `-p`

---

### T3.4 - Client - Parsing CREATE, REMOVE, COMBINE
- **Difficulté** : **\*\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 3h
- **Date de début** : 23 décembre 2025, 18h00
- **Deadline** : 24 décembre 2025, 18h00
- **Branche Git** : `feature/client-parsing-rendu-final`
- **Prérequis** : T3.3 (démon CREATE/REMOVE/COMBINE), T2.5 (parsing arguments)
- **Responsable** : Ahmed Mouncef Chabira
- **Description** :
  - Corriger `parse_list()` pour gérer `"*"` correctement : `(1ULL << (max_value + 1)) - 1` au lieu de `(1ULL << max_value) - 1`
  - Ajouter parsing de `-c` (CREATE) avec options `-m`, `-H`, `-d`, `-n` (abstraite)
  - Ajouter parsing de `-r` (REMOVE) avec taskid
  - Ajouter parsing de `-s` (COMBINE) : modifier `optstr` de `"s:"` à `"s"`, parser les taskids après `getopt()`, gérer `-p` s'il apparaît
  - Construire requête CREATE avec timing et arguments
  - Construire requête COMBINE avec timing, type "SQ", et taskids
  - Gestion des erreurs de parsing (format invalide, commande manquante)
- **Livrable** : `tadmor.c` avec parsing complet de CREATE, REMOVE, COMBINE

---

### T3.5 - Client - Gestion des réponses CREATE, REMOVE, COMBINE
- **Difficulté** : **\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 3h
- **Date de début** : 24 décembre 2025, 18h00
- **Deadline** : 25 décembre 2025, 18h00
- **Branche Git** : `feature/client-responses-rendu-final`
- **Prérequis** : T3.4 (parsing CREATE/REMOVE/COMBINE), T2.6 (requêtes consultatives)
- **Responsable** : Jeremy Pitel
- **Description** :
  - Ajouter gestion des réponses CREATE/REMOVE/COMBINE dans le switch de traitement des réponses
  - Afficher le taskid pour CREATE/COMBINE (succès)
  - Succès silencieux pour REMOVE (pas de sortie)
  - Gestion des erreurs : afficher "ERROR: NOT_FOUND" si nécessaire
  - Formatage correct de l'affichage
  - Tests avec le démon
- **Livrable** : `tadmor.c` avec gestion complète des réponses CREATE, REMOVE, COMBINE

---

### T3.6 - Intégration et tests du rendu final

---

### T3.6 - Intégration et tests du rendu final
- **Difficulté** : **\*\***
- **Jalon** : Rendu Final
- **Durée estimée** : 6h
- **Date de début** : 25 décembre 2025, 18h00
- **Deadline** : 10 janvier 2026, 12h00
- **Branche Git** : `integration/rendu-final`
- **Prérequis** : T3.5 (gestion réponses client), T3.3 (démon CREATE/REMOVE/COMBINE)
- **Responsable** : AJINOU Abderrahman
- **Description** :
  - Tests d'intégration de toutes les fonctionnalités du rendu final
  - Tests CREATE (simple, avec timings, abstraite, formats "*" et "-")
  - Tests REMOVE (tâche existante, tâche inexistante avec erreur NOT_FOUND)
  - Tests COMBINE (2 tâches, plusieurs tâches, avec timing, abstraite, tâches inexistantes)
  - Tests option `-p` pour le démon
  - Tests de persistance (reprise après arrêt du démon)
  - Tests de consultation (TIMES_EXITCODES, STDOUT, STDERR) pour vérifier qu'ils fonctionnent toujours
  - Vérification compilation sans erreurs ni warnings
  - Corrections de bugs
  - Documentation des tests dans `RESULTATS_TESTS.md`
- **Livrable** : Projet fonctionnel pour rendu final, tag `rendu-final` créé, tous les tests passent
