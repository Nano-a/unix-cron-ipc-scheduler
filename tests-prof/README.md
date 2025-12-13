# Dossier `tests-prof/`

Ce dossier contient les **tests automatiques fournis par le professeur** pour évaluer les jalons 1 et 2 du projet.

## 📁 Structure

```
tests-prof/
├── test-jalon-1.py              # Tests automatiques Jalon 1 (12 tests)
├── run-tadmor-tests-jalon-2.sh  # Tests automatiques client Jalon 2 (11 tests)
├── run-erraid-tests-jalon-2.py  # Tests automatiques démon Jalon 2 (11 tests)
├── common.py                    # Fonctions communes aux tests Python
├── erraid-tests/                # Tests du démon (Jalon 2)
│   └── erraid-jalon-2-test-X/   # Chaque test contient :
│       ├── arborescence.tar.gz  # Arborescence de tâches à charger
│       ├── arguments            # Arguments de la ligne de commande
│       ├── request              # Requête binaire envoyée par le client
│       ├── reply                # Réponse binaire attendue du démon
│       ├── stdout               # Sortie standard attendue
│       └── exitcode             # Code de retour attendu
├── tadmor-tests/                # Tests du client (Jalon 2)
│   └── tadmor-jalon-2-test-X/   # Chaque test contient :
│       ├── arguments            # Arguments de la ligne de commande
│       ├── request              # Requête binaire attendue
│       ├── reply                # Réponse binaire attendue
│       ├── stdout               # Sortie standard attendue
│       └── exitcode             # Code de retour attendu
└── exemples-arborescences/      # Arborescences d'exemple pour les tests
    └── exemple-arborescence-X.tar.gz
```

## 🧪 Tests Jalon 1

### Fichier : `test-jalon-1.py`

**Description** : Teste le démon avec des arborescences préexistantes.

**Nombre de tests** : 12

**Tests inclus** :
1. Arborescence simple
2. Arborescence avec séquences
3. Commandes avec code de retour non-zéro
4. Commandes avec fork
5. Commandes de longue durée
6. Commandes de longue durée (chevauchement)
7. Commandes de longue durée avec écriture
8. Commandes externes imprévisibles
9. Arborescence profonde
10. **Valgrind** (détection fuites mémoire) ⚠️ Nécessite valgrind installé
11. Tâches simultanées
12. Tâche jamais lancée

**Utilisation** :
```bash
cd tests-prof
python3 test-jalon-1.py
```

**Résultat attendu** :
```
Jalon 1 : 12/12 tests réussis, 0 échecs
```

**Prérequis** :
- Python 3
- Valgrind installé pour le test 12 (voir `Fichier-md/TESTS_PROFESSEUR.md`)

---

## 🧪 Tests Jalon 2 - Client

### Fichier : `run-tadmor-tests-jalon-2.sh`

**Description** : Teste le client `tadmor` avec des requêtes préenregistrées.

**Nombre de tests** : 11

**Tests inclus** :
- LIST (liste vide, avec tâches)
- TIMES_EXITCODES (tâche existante, inexistante, jamais exécutée)
- STDOUT (tâche existante, inexistante, jamais exécutée)
- STDERR (tâche existante, inexistante, jamais exécutée)
- TERMINATE
- Format de sortie complexe

**Utilisation** :
```bash
cd tests-prof
bash run-tadmor-tests-jalon-2.sh
```

**Résultat attendu** :
```
Client: all tests passed
Jalon 2 : 11/11 tests réussis, 0 échecs
```

**Prérequis** :
- Bash
- Le projet doit être compilé (`make` dans le répertoire parent)

---

## 🧪 Tests Jalon 2 - Démon

### Fichier : `run-erraid-tests-jalon-2.py`

**Description** : Teste le démon `erraid` avec des requêtes préenregistrées.

**Nombre de tests** : 11

**Tests inclus** :
- LIST
- TIMES_EXITCODES (tâche existante, inexistante, jamais exécutée)
- STDOUT (tâche existante, inexistante, jamais exécutée)
- STDERR (tâche existante, inexistante, jamais exécutée)
- TERMINATE

**Utilisation** :
```bash
cd tests-prof
python3 run-erraid-tests-jalon-2.py
```

**Résultat attendu** :
```
Jalon 2 : 11/11 tests réussis, 0 échecs
```

**Prérequis** :
- Python 3
- Le projet doit être compilé

---

## 📚 Fichiers Communs

### `common.py`

**Description** : Module Python contenant les fonctions communes utilisées par les tests.

**Fonctions principales** :
- `run_daemon_against_archive()` : Lance le démon avec une arborescence
- `daemon_replay()` : Rejoue une séquence de requêtes/réponses
- `check_task_exitcodes()` : Vérifie les codes de retour
- `check_task_output()` : Vérifie les sorties standard/erreur
- Fonctions de désérialisation (timing, logs, etc.)

**Utilisation** : Importé automatiquement par les scripts de test.

---

## 📦 Exemples d'Arborescences

### Dossier : `exemples-arborescences/`

**Description** : Contient des arborescences d'exemple utilisées par les tests.

**Fichiers** :
- `exemple-arborescence-1.tar.gz` : Arborescence simple
- `exemple-arborescence-2.tar.gz` : Arborescence avec séquences
- `exemple-arborescence-5.tar.gz` : Commandes avec erreurs
- `exemple-arborescence-6-fork.tar.gz` : Commandes avec fork
- `exemple-arborescence-7-commandes-longues.tar.gz` : Commandes longues
- `exemple-arborescence-11-arborescence-profonde.tar.gz` : Arborescence profonde
- Et autres...

**Utilisation** : Les tests extraient automatiquement ces archives dans des répertoires temporaires.

---

## 🔍 Format des Tests

### Test du Démon (erraid-tests)

Chaque test contient :
- **`arborescence.tar.gz`** : Arborescence à charger
- **`arguments`** : Arguments de la ligne de commande pour le démon
- **`request`** : Requête binaire envoyée par le client
- **`reply`** : Réponse binaire attendue du démon
- **`stdout`** : Sortie standard attendue
- **`exitcode`** : Code de retour attendu

### Test du Client (tadmor-tests)

Chaque test contient :
- **`arguments`** : Arguments de la ligne de commande pour le client
- **`request`** : Requête binaire attendue
- **`reply`** : Réponse binaire attendue du démon
- **`stdout`** : Sortie standard attendue
- **`exitcode`** : Code de retour attendu

---

## 🚀 Exécution Complète

Pour exécuter tous les tests :

```bash
cd tests-prof

# Jalon 1
python3 test-jalon-1.py

# Jalon 2 - Client
bash run-tadmor-tests-jalon-2.sh

# Jalon 2 - Démon
python3 run-erraid-tests-jalon-2.py
```

---

## 📝 Notes Importantes

1. **Les tests modifient `/tmp/`** : Les tests créent des répertoires temporaires dans `/tmp/`
2. **Compilation automatique** : Les tests compilent automatiquement le projet avant de tester
3. **Valgrind** : Le test 12 du Jalon 1 nécessite valgrind (voir `Fichier-md/TESTS_PROFESSEUR.md`)
4. **Timeout** : Certains tests ont des timeouts pour éviter les blocages
5. **Nettoyage** : Les tests nettoient automatiquement les fichiers temporaires

---

## 🔗 Documentation Complémentaire

Pour plus de détails sur les tests, voir :
- `../Fichier-md/TESTS_PROFESSEUR.md` : Guide complet des tests
- `../Fichier-md/TESTS_MANUELS.md` : Guide de test manuel

