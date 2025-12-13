# Projet erraid-tadmor

## 📋 Description

**erraid-tadmor** est un système de planification de tâches similaire à `cron`, développé dans le cadre d'un projet de système d'exploitation (L3). Le projet est composé de deux programmes principaux :

- **erraid** : Un démon qui exécute automatiquement les tâches planifiées selon leur configuration de timing
- **tadmor** : Un client en ligne de commande permettant de gérer les tâches (création, consultation, suppression)

## 🏗️ Architecture

Le projet suit une architecture modulaire avec séparation claire des responsabilités :

```
projet-systeme-l3/
├── src/                      # Implémentations des modules
│   ├── erraid.c              # Démon principal
│   ├── tadmor.c              # Client de gestion
│   ├── protocol.c            # Communication client-démon
│   ├── serialization.c       # Sérialisation/désérialisation binaire
│   ├── task_tree.c           # Gestion de l'arborescence des tâches
│   └── execution.c           # Exécution des commandes
├── include/                  # Interfaces publiques (headers)
│   ├── serialization.h
│   ├── task_tree.h
│   ├── execution.h
│   └── protocol.h
├── tests-prof/               # Tests automatiques du professeur
│   ├── test-jalon-1.py       # Tests Jalon 1 (12 tests)
│   ├── run-tadmor-tests-jalon-2.sh  # Tests client Jalon 2
│   └── run-erraid-tests-jalon-2.py  # Tests démon Jalon 2
├── Fichier-md/               # Documentation complète du projet
│   ├── README.md             # Index de la documentation
│   ├── CONTEXTE.md           # Vue d'ensemble
│   ├── ARCHITECTURE.md       # Architecture système
│   ├── JALON_1.md           # Guide Jalon 1
│   ├── JALON_2.md           # Guide Jalon 2
│   ├── COMMANDES_ERRAID.md  # Commandes du démon
│   ├── COMMANDES_TADMOR.md  # Commandes du client
│   ├── TESTS_PROFESSEUR.md  # Tests automatiques
│   ├── TESTS_MANUELS.md     # Tests manuels
│   └── ERREURS.md           # Gestion des erreurs
├── exemples-arborescences/   # Exemples d'arborescences de tâches
├── autotests/                # Tests locaux (copie)
├── scripts/                  # Scripts de test et d'intégration
├── logs-j1/                  # Logs de test du Jalon 1
├── Makefile                  # Règles de compilation
└── AUTHORS.md                # Liste des contributeurs
```

## 🚀 Compilation et Installation

### Prérequis

- Compilateur GCC avec support C11
- Make
- Système d'exploitation POSIX (Linux, macOS, etc.)

### Compilation

```bash
# Compiler les deux programmes
make

# Nettoyer les fichiers objets
make clean

# Nettoyer complètement (objets + binaires + répertoire de stockage)
make distclean
```

Les binaires générés sont :
- `erraid` : Le démon
- `tadmor` : Le client

## 📖 Utilisation

### Démon erraid

Le démon doit être lancé avec le répertoire de travail contenant les tâches :

```bash
./erraid -r /chemin/vers/repertoire
```

Le démon :
1. Charge toutes les tâches depuis `$run_dir/tasks/`
2. Démarre un thread d'exécution des tâches (vérifie toutes les secondes)
3. Ouvre les tubes nommés pour la communication client (Jalon 2)
4. Entre dans une boucle principale qui gère les requêtes client
5. Exécute les tâches éligibles de manière asynchrone et enregistre les résultats
6. Gère les signaux SIGINT/SIGTERM et la requête TERMINATE pour s'arrêter proprement

**Voir** : `Fichier-md/COMMANDES_ERRAID.md` pour la documentation complète.

### Client tadmor

Le client permet de communiquer avec le démon et consulter les tâches :

**Jalon 2 - Requêtes consultatives** :
- `-l` : Lister toutes les tâches
- `-x TASKID` : Historique d'exécution d'une tâche
- `-o TASKID` : Sortie standard de la dernière exécution
- `-e TASKID` : Sortie d'erreur de la dernière exécution
- `-q` : Arrêter le démon

**Exemple** :
```bash
# Lister les tâches
./tadmor -l -p /tmp/test-erraid/pipes

# Voir l'historique d'une tâche
./tadmor -x 0 -p /tmp/test-erraid/pipes

# Voir la sortie standard
./tadmor -o 0 -p /tmp/test-erraid/pipes
```

**Voir** : `Fichier-md/COMMANDES_TADMOR.md` pour la documentation complète.

## 📁 Structure des Tâches

Chaque tâche est stockée dans une arborescence de fichiers :

```
$run_dir/
└── tasks/
    └── <taskid>/
        ├── timing          # Configuration de planification (binaire)
        ├── cmd/            # Répertoire de la commande
        │   ├── type        # Type de commande (SI ou SQ)
        │   ├── argv        # Arguments (si commande simple)
        │   └── 0/, 1/, ... # Sous-commandes (si séquence)
        ├── times-exitcodes # Logs d'exécution (binaire)
        ├── stdout          # Sortie standard de la dernière exécution
        └── stderr          # Sortie d'erreur de la dernière exécution
```

## 🔧 Modules Principaux

### 1. Sérialisation (`serialization.c/h`)

Gère la conversion entre structures C et format binaire big-endian pour :
- Types entiers (uint8/16/32/64, int64)
- Chaînes de caractères
- Structures de timing
- Commandes (simples et complexes)

### 2. Gestion des Tâches (`task_tree.c/h`)

Fournit les fonctions pour :
- Charger/sauvegarder les tâches depuis/vers le système de fichiers
- Gérer l'arborescence des répertoires
- Enregistrer les logs d'exécution

### 3. Exécution (`execution.c/h`)

Implémente l'exécution des commandes :
- Commandes simples : exécution directe avec capture stdout/stderr
- Séquences : exécution séquentielle avec concaténation des sorties

### 4. Démon (`erraid.c`)

Boucle principale du démon :
- Chargement périodique des tâches
- Vérification des conditions de timing
- Exécution asynchrone des tâches (multithreading)
- **Jalon 2** : Réception et traitement des requêtes client
- Journalisation des résultats

### 5. Communication (`protocol.c`)

Gère la communication client-démon :
- Tubes nommés (FIFO) pour les requêtes et réponses
- Sérialisation/désérialisation des messages
- Gestion des codes d'opération et d'erreur

### 6. Client (`tadmor.c`)

Interface utilisateur en ligne de commande :
- Parsing des arguments
- Envoi de requêtes au démon
- Affichage formaté des réponses

## 📚 Documentation Détaillée

### Documentation Complète

Toute la documentation est organisée dans le dossier `Fichier-md/` :
- **[Fichier-md/README.md](Fichier-md/README.md)** - Index de la documentation
- **[Fichier-md/CONTEXTE.md](Fichier-md/CONTEXTE.md)** - Vue d'ensemble du projet
- **[Fichier-md/ARCHITECTURE.md](Fichier-md/ARCHITECTURE.md)** - Architecture système
- **[Fichier-md/JALON_1.md](Fichier-md/JALON_1.md)** - Guide complet du Jalon 1
- **[Fichier-md/JALON_2.md](Fichier-md/JALON_2.md)** - Guide complet du Jalon 2
- **[Fichier-md/COMMANDES_ERRAID.md](Fichier-md/COMMANDES_ERRAID.md)** - Commandes du démon
- **[Fichier-md/COMMANDES_TADMOR.md](Fichier-md/COMMANDES_TADMOR.md)** - Commandes du client
- **[Fichier-md/TESTS_PROFESSEUR.md](Fichier-md/TESTS_PROFESSEUR.md)** - Tests automatiques
- **[Fichier-md/TESTS_MANUELS.md](Fichier-md/TESTS_MANUELS.md)** - Tests manuels
- **[Fichier-md/ERREURS.md](Fichier-md/ERREURS.md)** - Gestion des erreurs

### Documentation Technique

Pour plus de détails sur chaque module, consultez les README.md dans chaque dossier :

- [src/README.md](src/README.md) - Documentation des fichiers sources
- [include/README.md](include/README.md) - Documentation des interfaces
- [scripts/README.md](scripts/README.md) - Documentation des scripts
- [tests-prof/README.md](tests-prof/README.md) - Tests du professeur
- [exemples-arborescences/README.md](exemples-arborescences/README.md) - Exemples d'arborescences

## 🧪 Tests

### Tests Automatiques du Professeur

Les tests officiels se trouvent dans `tests-prof/` :

```bash
# Tests Jalon 1 (12 tests)
cd tests-prof
python3 test-jalon-1.py

# Tests Jalon 2 - Client (11 tests)
bash run-tadmor-tests-jalon-2.sh

# Tests Jalon 2 - Démon (11 tests)
python3 run-erraid-tests-jalon-2.py
```

**Résultat attendu** :
- Jalon 1 : 12/12 tests réussis ✅
- Jalon 2 : 11/11 tests réussis ✅

**Prérequis** :
- Python 3
- Valgrind installé pour le test 12 du Jalon 1 (voir `Fichier-md/TESTS_PROFESSEUR.md`)

**Voir** : `Fichier-md/TESTS_PROFESSEUR.md` pour la documentation complète des tests.

## 🔀 Workflow Git

Le projet suit un workflow Git standard :

- **main** : Branche principale (merges par le chef de projet uniquement)
- **develop** : Branche de développement (merges des features)
- **jalon1**, **jalon2**, ... : Branches de jalons
- **feature/*** : Branches de fonctionnalités

### Contribution

1. Créer une branche depuis `develop` : `git checkout -b feature/nom-tache`
2. Développer et commiter régulièrement
3. Pousser et créer une merge request vers `develop`

## ✅ État du Projet

### Jalon 1 (25 novembre 2025) ✅
- Démon avec arborescence statique
- Exécution des tâches aux dates prescrites
- Mise à jour des fichiers de log
- **Tests** : 12/12 réussis ✅

### Jalon 2 (15 décembre 2025) ✅
- Communication client-démon via tubes nommés
- Requêtes consultatives (LIST, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE)
- **Tests** : 11/11 réussis ✅

### Rendu Final (10 janvier 2026) ⏳
- Fonctionnalités à implémenter (CREATE, REMOVE, COMBINE)
- **Note** : Non implémenté dans le code actuel

## 👥 Équipe

Voir `AUTHORS.md` pour la liste des contributeurs.

## 📝 Licence

Ce projet est développé dans le cadre d'un projet académique.

## 🔍 Format de Données

Tous les fichiers binaires utilisent le format **big-endian** pour garantir la portabilité entre architectures. Les structures sont sérialisées selon les spécifications définies dans les fichiers d'en-tête.

## ⚠️ Notes Importantes

- Le démon doit avoir les permissions d'écriture dans le répertoire de travail
- Les tâches sont chargées dynamiquement périodiquement (toutes les 10 secondes)
- Le format `times-exitcodes` est binaire pur (pas de caractères de fin de ligne)
- Les séquences sont exécutées dans l'ordre numérique strict (0, 1, 2, 3...)
- **Multithreading** : Le démon utilise des threads pour l'exécution asynchrone des tâches
- **Isolation** : Les processus enfants utilisent `setsid()` pour continuer après SIGTERM
- **Communication** : Les tubes nommés doivent être dans le bon répertoire (avec `/pipes`)

## 🔗 Liens Utiles

- **Documentation complète** : `Fichier-md/README.md`
- **Spécifications officielles** : `sy5-2025-2026/Projet/`
- **Tests** : `tests-prof/README.md`
