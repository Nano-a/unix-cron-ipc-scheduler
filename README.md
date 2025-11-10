# Projet erraid-tadmor

Projet de système d'exploitation (L3) - Démon de planification de tâches similaire à `cron`.

## 📋 Description

Ce projet implémente un système de planification de tâches composé de :
- **erraid** : Démon qui exécute les tâches planifiées
- **tadmor** : Client en ligne de commande pour gérer les tâches

## 🏗️ Structure du projet

```
projet-systeme-l3/
├── src/              # Fichiers sources (.c)
│   ├── erraid.c      # Démon
│   ├── tadmor.c      # Client
│   ├── serialization.c
│   ├── task_tree.c
│   └── protocol.c
├── include/          # Fichiers d'en-tête (.h)
│   ├── serialization.h
│   ├── task_tree.h
│   └── protocol.h
├── Makefile          # Règles de compilation
├── BACKLOG.md        # Liste des tâches à réaliser
├── ARCHITECTURE_T1.1.md  # Architecture détaillée
└── CONVENTIONS.md    # Conventions de code
```

## 🚀 Compilation

```bash
make              # Compile erraid et tadmor
make clean        # Supprime les fichiers objets
make distclean    # Supprime les binaires et le répertoire de stockage
```

## 📚 Documentation

- **BACKLOG.md** : Liste complète des tâches avec prérequis, dates et responsables
- **ARCHITECTURE_T1.1.md** : Architecture détaillée des modules
- **CONVENTIONS.md** : Conventions de code à respecter

## 🔀 Workflow Git

- **main** : Branche principale (merges par le chef de projet uniquement)
- **develop** : Branche de développement (merges des features)
- **feature/*** : Branches de fonctionnalités créées depuis `develop`

Pour contribuer :
1. Créer une branche depuis `develop` : `git checkout -b feature/nom-tache`
2. Développer et commiter régulièrement
3. Pousser et créer une merge request vers `develop`

## 👥 Équipe

Voir `AUTHORS.md` pour la liste des contributeurs.
