# Contexte du Projet erraid-tadmor

## Vue d'Ensemble

Le projet **erraid-tadmor** est un système de planification de tâches similaire à `cron`. Il permet à un utilisateur d'automatiser l'exécution périodique de tâches à des moments spécifiés.

### Composants Principaux

Le projet est composé de deux programmes :

- **`erraid`** : Le démon qui exécute les tâches périodiquement
- **`tadmor`** : Le client qui permet de communiquer avec le démon

### Objectif

L'objectif est de créer un couple démon-client permettant :
- De définir des tâches à exécuter périodiquement
- De consulter la liste des tâches définies
- D'obtenir des informations sur les exécutions passées d'une tâche
- De gérer des tâches correspondant à des commandes complexes (séquences, pipelines)

---

## Les Trois Phases du Projet

Le projet est évalué en **trois phases** distinctes :

### 1. Jalon 1 (25 novembre 2025)

**Objectif** : Implémenter un démon capable d'exécuter des tâches depuis une arborescence **statique et préexistante**.

#### Fonctionnalités

- ✅ Compilation sans erreur
- ✅ Interprétation d'une arborescence de tâches fournie
- ✅ Exécution des tâches simples aux dates prescrites
- ✅ Exécution des séquences de tâches aux dates prescrites
- ✅ Mise à jour des fichiers de log (valeurs de retour, sorties standard/erreur)

#### Limitations

- L'arborescence est **statique** : elle ne peut pas être modifiée par le client
- Pas de communication client-démon
- Le démon lit simplement les tâches depuis le disque et les exécute

#### Tests

12 tests automatiques couvrant :
- Tâches simples
- Tâches séquentielles
- Commandes avec fork
- Commandes de longue durée
- Tâches simultanées
- Détection de fuites mémoire (valgrind)

**Voir** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md#jalon-1)

---

### 2. Jalon 2 (15 décembre 2025)

**Objectif** : Ajouter la communication client-démon pour les requêtes **consultatives** uniquement.

#### Fonctionnalités

- ✅ Le démon peut exploiter une arborescence préexistante ET répondre aux requêtes client
- ✅ Requêtes consultatives :
  - `LIST` : Lister toutes les tâches
  - `TIMES_EXITCODES` : Historique d'exécution d'une tâche
  - `STDOUT` : Sortie standard de la dernière exécution
  - `STDERR` : Sortie d'erreur de la dernière exécution
  - `TERMINATE` : Arrêter le démon

#### Limitations

- ❌ Pas de création de tâches (`CREATE`)
- ❌ Pas de suppression de tâches (`REMOVE`)
- ❌ Pas de combinaison de tâches (`COMBINE`)
- Les tâches doivent toujours être chargées depuis une arborescence préexistante

#### Tests

22 tests automatiques (11 pour le client, 11 pour le démon) couvrant :
- Toutes les requêtes consultatives
- Gestion des erreurs (tâche inexistante, jamais exécutée)
- Format des réponses

**Voir** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md#jalon-2)

---

### 3. Rendu Final (10 janvier 2026)

**Objectif** : Implémenter toutes les fonctionnalités manquantes.

#### Fonctionnalités à Implémenter

- `CREATE` : Créer une nouvelle tâche simple
- `REMOVE` : Supprimer une tâche
- `COMBINE` : Créer une tâche par combinaison de tâches existantes
- Gestion complète du cycle de vie des tâches

#### Note Importante

⚠️ **Cette phase n'est PAS implémentée** dans le code actuel. Le code ne contient aucune fonctionnalité du rendu final pour éviter que le professeur ne voie du code non demandé.

---

## Comparaison des Phases

| Fonctionnalité | Jalon 1 | Jalon 2 | Rendu Final |
|----------------|---------|---------|-------------|
| Exécution de tâches | ✅ | ✅ | ✅ |
| Arborescence statique | ✅ | ✅ | ✅ |
| Communication client-démon | ❌ | ✅ | ✅ |
| LIST | ❌ | ✅ | ✅ |
| TIMES_EXITCODES | ❌ | ✅ | ✅ |
| STDOUT/STDERR | ❌ | ✅ | ✅ |
| TERMINATE | ❌ | ✅ | ✅ |
| CREATE | ❌ | ❌ | ✅ |
| REMOVE | ❌ | ❌ | ✅ |
| COMBINE | ❌ | ❌ | ✅ |

---

## Structure du Projet

```
projet-systeme-l3/
├── src/
│   ├── erraid.c          # Démon
│   ├── tadmor.c          # Client
│   ├── protocol.c        # Communication client-démon
│   ├── task_tree.c       # Gestion de l'arborescence
│   ├── execution.c       # Exécution des commandes
│   └── serialization.c   # Sérialisation binaire
├── include/
│   ├── protocol.h
│   ├── task_tree.h
│   ├── execution.h
│   └── serialization.h
├── tests-prof/           # Tests du professeur
│   ├── test-jalon-1.py
│   ├── run-tadmor-tests-jalon-2.sh
│   └── run-erraid-tests-jalon-2.py
└── Makefile
```

---

## Prochaines Étapes

- **Comprendre l'architecture** : [ARCHITECTURE.md](ARCHITECTURE.md)
- **Apprendre à utiliser le démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Apprendre à utiliser le client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)
- **Tester le projet** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)

