# Projet erraid-tadmor

**Système de planification de tâches - L3 Informatique Système**

## 📋 Description

**erraid-tadmor** est un système de planification de tâches similaire à `cron`, développé en C. Le projet est composé de deux programmes :

- **erraid** : Démon qui exécute automatiquement les tâches planifiées
- **tadmor** : Client en ligne de commande pour gérer les tâches

## 👥 Équipe

Voir [`AUTHORS.md`](AUTHORS.md) pour la liste complète des contributeurs.

## 🚀 Compilation

### Prérequis

- Compilateur GCC avec support C11
- Make
- Système POSIX (Linux)

### Commandes

```bash
# Compiler les deux programmes (crée erraid et tadmor)
make

# Nettoyer les fichiers objets
make clean

# Nettoyer complètement (objets + binaires)
make distclean
```

## 📖 Utilisation

### Lancer le démon

```bash
# Lancer en arrière-plan (par défaut)
./erraid -R /tmp/$USER/erraid -P /tmp/$USER/erraid/pipes

# Lancer en avant-plan (pour debug)
./erraid -R /tmp/$USER/erraid -P /tmp/$USER/erraid/pipes -F

# Avec logs de débogage
./erraid -R /tmp/$USER/erraid -P /tmp/$USER/erraid/pipes -F -d
```

**Options** :
- `-R RUN_DIR` : Répertoire de stockage (défaut : `/tmp/$USER/erraid`)
- `-P PIPES_DIR` : Répertoire des tubes de communication (défaut : `RUN_DIR/pipes`)
- `-F` : Exécution en avant-plan (sans démonisation)
- `-d` : Activer les logs de débogage

### Utiliser le client

```bash
# Créer une tâche simple
./tadmor -P /tmp/$USER/erraid/pipes -c echo "Hello World"

# Créer une tâche avec timing (tous les jours à 14h00)
./tadmor -P /tmp/$USER/erraid/pipes -c -m 0 -H 14 echo "Test"

# Lister toutes les tâches
./tadmor -P /tmp/$USER/erraid/pipes -l

# Voir l'historique d'exécution d'une tâche
./tadmor -P /tmp/$USER/erraid/pipes -x TASKID

# Voir la sortie standard de la dernière exécution
./tadmor -P /tmp/$USER/erraid/pipes -o TASKID

# Voir la sortie d'erreur de la dernière exécution
./tadmor -P /tmp/$USER/erraid/pipes -e TASKID

# Supprimer une tâche
./tadmor -P /tmp/$USER/erraid/pipes -r TASKID

# Arrêter le démon
./tadmor -P /tmp/$USER/erraid/pipes -q
```

**Options de création** :
- `-c` : Créer une tâche simple
- `-s` : Créer une séquence de tâches (`;`)
- `-p` : Créer un pipeline de tâches (`|`)
- `-i` : Créer une commande conditionnelle (`if-then-else`)
- `-n` : Créer une tâche abstraite (sans timing)
- `-m MINUTES` : Minutes d'exécution (0-59, ou listes/plages)
- `-H HOURS` : Heures d'exécution (0-23, ou listes/plages)
- `-d DAYSOFWEEK` : Jours de la semaine (0-6, 0=dimanche)

## 🏗️ Architecture

Le projet est organisé en modules :

```
projet-systeme-l3/
├── src/                      # Code source
│   ├── erraid.c              # Démon principal
│   ├── tadmor.c              # Client
│   ├── protocol.c            # Communication client-démon
│   ├── serialization.c       # Sérialisation binaire
│   ├── task_tree.c           # Gestion des tâches sur disque
│   └── execution.c           # Exécution des commandes
├── include/                  # Headers
│   ├── protocol.h
│   ├── serialization.h
│   ├── task_tree.h
│   └── execution.h
├── Makefile                  # Règles de compilation
├── README.md                 # Ce fichier
├── AUTHORS.md                # Liste des contributeurs
├── ARCHITECTURE.md           # Architecture détaillée
├── GUIDE_COMPLET_PROJET.md   # Guide complet d'utilisation
├── BACKLOG.md                # Suivi des tâches
├── .mailmap                  # Mapping des auteurs Git
├── test.sh                   # Script de test officiel (Python 3.10)
└── test-python313.sh         # Script de test avec Python 3.13
```

Voir [`ARCHITECTURE.md`](ARCHITECTURE.md) pour plus de détails.

## 🧪 Tests

### 🚀 Méthode Recommandée (Configuration Automatique)

```bash
# Lancer les tests avec configuration automatique de l'environnement
./run-tests.sh
```

Ce script configure automatiquement Python 3.13 si disponible et lance les tests officiels.

### 📋 Autres Méthodes

```bash
# Option 1 : Tests officiels du professeur (nécessite Python 3.11+)
./test.sh

# Option 2 : Tests avec Python 3.13 explicite
./test-python313.sh

# Option 3 : Tests par jalon
./test.sh 1      # Jalon 1 uniquement
./test.sh 2d     # Jalon 2 (démon) uniquement
./test.sh 2c     # Jalon 2 (client) uniquement
```

### 📊 Résultats Attendus

#### Avec Python 3.13 (Recommandé)

- **Jalon 1** : 11-12/12 tests réussis (91-100%) ✅
- **Jalon 2 (démon)** : 16/16 tests réussis (100%) ✅
- **Jalon 2 (client)** : 14/14 tests réussis (100%) ✅

**Total : 41-42/42 tests réussis (97-100%)**

#### Avec Python 3.10 (Par défaut du système)

- **Jalon 1** : 12/12 tests réussis ✅
- **Jalon 2 (démon)** : 16/16 tests réussis ✅
- **Jalon 2 (client)** : 0/14 (incompatibilité Python 3.10) ❌

**Note Importante** : Les tests du client Jalon 2 utilisent `subprocess.Popen(process_group=...)` qui n'existe qu'à partir de Python 3.11. Le système utilise Python 3.10 par défaut, mais Python 3.13 est disponible. Utilisez `./run-tests.sh` pour une configuration automatique.

### 📖 Guide Complet

Voir [TESTS_ENVIRONNEMENT.md](TESTS_ENVIRONNEMENT.md) pour :
- Instructions détaillées d'exécution
- Configuration de l'environnement
- Résolution des problèmes
- Explications pour le professeur

## 📁 Structure des Données

Chaque tâche est stockée dans une arborescence :

```
/tmp/$USER/erraid/
└── tasks/
    └── <taskid>/
        ├── timing          # Configuration de planification (binaire)
        ├── argv            # Arguments de la commande (binaire)
        ├── times-exitcodes # Historique d'exécution (binaire)
        ├── stdout          # Sortie standard de la dernière exécution
        └── stderr          # Sortie d'erreur de la dernière exécution
```

## 📚 Documentation

- **[GUIDE_COMPLET_PROJET.md](GUIDE_COMPLET_PROJET.md)** : Guide complet avec tous les cas d'utilisation
- **[ARCHITECTURE.md](ARCHITECTURE.md)** : Architecture système détaillée
- **[BACKLOG.md](BACKLOG.md)** : Suivi du développement

## ✅ État du Projet

### Jalon 1 (25 novembre 2025) ✅
- Démon avec arborescence statique
- Exécution des tâches aux dates prescrites
- Tests : 12/12 réussis

### Jalon 2 (15 décembre 2025) ✅
- Communication client-démon via tubes nommés
- Requêtes consultatives
- Tests : 16/16 (démon) + 14/14 (client) réussis

### Rendu Final (11 janvier 2026) ✅
- Création/suppression de tâches
- Combinaisons de tâches (séquence, pipeline, conditionnel)
- Tests : 28/28 fonctionnels réussis (Jalon 1 + Jalon 2 démon)

## 🔧 Détails Techniques

### Communication

- **Tubes nommés (FIFO)** pour la communication client-démon
- **Format big-endian** pour la portabilité
- **Protocole binaire** défini dans `include/protocol.h`

### Exécution

- **Multithreading** : Thread dédié pour l'exécution des tâches
- **Vérification périodique** : Toutes les 10 secondes
- **Isolation des processus** : Utilisation de `setsid()` pour les tâches

### Persistance

- **Stockage sur disque** : Toutes les tâches sont sauvegardées
- **Reprise après interruption** : Le démon peut redémarrer sans perte de données
- **Logs d'exécution** : Historique complet avec timestamps et codes de retour

## ⚠️ Notes Importantes

- Le démon doit avoir les permissions d'écriture dans `RUN_DIR`
- Les identifiants de tâches sont uniques et incrémentaux (jamais réutilisés)
- Les tâches combinées consomment les tâches sources (elles sont supprimées)
- Le format de timing suit la syntaxe cron (minutes, heures, jours de la semaine)

## 🔗 Liens Utiles

- **Énoncé du projet** : `Prof/sy5-2025-2026/Projet/enonce.md`
- **Protocole de communication** : `Prof/sy5-2025-2026/Projet/protocole.md`
- **Format d'arborescence** : `Prof/sy5-2025-2026/Projet/arborescence.md`

## 📝 Licence

Projet académique - L3 Informatique Système - Université Paris Cité
