# Projet erraid-tadmor

## 📋 Description

**erraid-tadmor** est un système de planification de tâches similaire à `cron`, développé dans le cadre d'un projet de système d'exploitation (L3). Le projet est composé de deux programmes principaux :

- **erraid** : Un démon qui exécute automatiquement les tâches planifiées selon leur configuration de timing
- **tadmor** : Un client en ligne de commande permettant de gérer les tâches (création, consultation, suppression)

## 🏗️ Architecture

Le projet suit une architecture modulaire avec séparation claire des responsabilités :

```
projet-systeme-l3/
├── src/              # Implémentations des modules
│   ├── erraid.c      # Démon principal
│   ├── tadmor.c      # Client de gestion
│   ├── serialization.c  # Sérialisation/désérialisation binaire
│   ├── task_tree.c   # Gestion de l'arborescence des tâches
│   └── execution.c   # Exécution des commandes
├── include/          # Interfaces publiques (headers)
│   ├── serialization.h
│   ├── task_tree.h
│   ├── execution.h
│   └── protocol.h
├── scripts/         # Scripts de test et d'intégration
├── logs-j1/        # Logs de test du Jalon 1
├── Makefile         # Règles de compilation
└── BACKLOG.md       # Gestion des tâches du projet
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
2. Vérifie chaque seconde si une tâche doit être exécutée selon son timing
3. Exécute les tâches éligibles et enregistre les résultats
4. Gère les signaux SIGINT/SIGTERM pour s'arrêter proprement

### Client tadmor

Le client permet de gérer les tâches (fonctionnalités à venir dans les jalons suivants).

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
- Exécution et journalisation

## 📚 Documentation Détaillée

Pour plus de détails sur chaque module, consultez les README.md dans chaque dossier :

- [src/README.md](src/README.md) - Documentation des fichiers sources
- [include/README.md](include/README.md) - Documentation des interfaces
- [scripts/README.md](scripts/README.md) - Documentation des scripts

## 🧪 Tests

Des scripts de test sont disponibles dans le dossier `scripts/` :

```bash
# Test d'intégration du Jalon 1
./scripts/test_integration_j1.sh
```

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

## 📋 Gestion des Tâches

Le fichier `BACKLOG.md` contient la liste complète des tâches du projet avec :
- Prérequis
- Dates de livraison
- Responsables
- Statut

## 👥 Équipe

Voir `AUTHORS.md` pour la liste des contributeurs.

## 📝 Licence

Ce projet est développé dans le cadre d'un projet académique.

## 🔍 Format de Données

Tous les fichiers binaires utilisent le format **big-endian** pour garantir la portabilité entre architectures. Les structures sont sérialisées selon les spécifications définies dans les fichiers d'en-tête.

## ⚠️ Notes Importantes

- Le démon doit avoir les permissions d'écriture dans le répertoire de travail
- Les tâches sont chargées dynamiquement à chaque cycle (1 seconde)
- Le format `times-exitcodes` est binaire pur (pas de caractères de fin de ligne)
- Les séquences sont exécutées dans l'ordre numérique strict (0, 1, 2, 3...)
