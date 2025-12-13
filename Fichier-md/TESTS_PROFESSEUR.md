# Tests Automatiques du Professeur

Ce document décrit en détail tous les tests automatiques fournis par le professeur pour les jalons 1 et 2.

## Prérequis

### Compilation

Avant de lancer les tests, assurez-vous que le projet compile sans erreur :

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
make clean
make
```

**Résultat attendu** : Compilation réussie, création des exécutables `erraid` et `tadmor`.

### Installation de Valgrind (Test 12 du Jalon 1)

⚠️ **IMPORTANT** : Le test 12 du Jalon 1 utilise `valgrind` pour détecter les fuites mémoire. Vous devez installer valgrind pour que ce test passe.

#### Installation sur Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y valgrind
```

#### Vérification de l'Installation

```bash
valgrind --version
```

**Résultat attendu** : Affiche la version de valgrind (ex: `valgrind-3.18.1`).

#### Si vous n'avez pas les droits sudo

Si vous n'avez pas les droits `sudo`, vous pouvez essayer :

```bash
# Méthode 1 : Snap (si disponible)
sudo snap install valgrind --classic

# Méthode 2 : Demander à un administrateur
# Contactez votre administrateur système pour installer valgrind
```

**Note** : Sans valgrind, le test 12 échouera avec un message indiquant que valgrind n'est pas disponible.

---

## Jalon 1 : Tests du Démon

### Commande de Test

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
python3 test-jalon-1.py
```

### Résultat Attendu

```
Jalon 1 : 12/12 tests réussis, 0 échecs
```

### Description des Tests

#### TEST 1 : Arborescence Simple

**Fichier** : `exemple-arborescence-1.tar.gz`

**Durée** : 182 secondes (environ 3 minutes)

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Sorties standard correctes :
  - Tâche 0 : `"Feed me lasagna\n"`
  - Tâche 1 : `"I hate mondays\n"`

**Résultat attendu** :
```
JALON 1, TEST 1:
Résultat : OK
```

---

#### TEST 2 : Arborescence avec Séquences

**Fichier** : `exemple-arborescence-2.tar.gz`

**Durée** : 182 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Sortie standard de la tâche 4 contient la date et l'heure au format attendu

**Résultat attendu** :
```
JALON 1, TEST 2:
Résultat : OK
```

---

#### TEST 5 : Commandes avec Code de Retour Non-Zéro

**Fichier** : `exemple-arborescence-5.tar.gz`

**Durée** : 182 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 1
- ✅ Sortie standard de la tâche 15 contient le texte attendu (chanson)

**Résultat attendu** :
```
JALON 1, TEST 5:
Résultat : OK
```

---

#### TEST 6 : Commandes avec Fork

**Fichier** : `exemple-arborescence-6-fork.tar.gz`

**Durée** : 90 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Sortie standard de la tâche 0 : `"A\nX\nB\nY\n"`
- ⚠️ Les timings ne sont pas vérifiés (`check_times=False`)

**Résultat attendu** :
```
JALON 1, TEST 6 (fork):
Résultat : OK
```

---

#### TEST 7 : Commandes de Longue Durée

**Fichier** : `exemple-arborescence-7-commandes-longues.tar.gz`

**Durée** : 182 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ⚠️ Les timings ne sont pas vérifiés (`check_times=False`)
- ⚠️ Les sorties ne sont pas vérifiées (liste vide)

**Note** : Ce test vérifie que les commandes longues (ex: `sleep 110`) continuent de s'exécuter même après que le démon reçoive un `SIGTERM`.

**Résultat attendu** :
```
JALON 1, TEST 7 (commandes longue durée):
Résultat : OK
```

---

#### TEST 8 : Commandes de Longue Durée (Chevauchement)

**Fichier** : `exemple-arborescence-8-commandes-longues-chevauchement.tar.gz`

**Durée** : 182 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ⚠️ Les timings ne sont pas vérifiés
- ⚠️ Les sorties ne sont pas vérifiées

**Résultat attendu** :
```
JALON 1, TEST 8 (commande longue durée v2):
Résultat : OK
```

---

#### TEST 9 : Commandes de Longue Durée avec Écriture

**Fichier** : `exemple-arborescence-9-commandes-longues-stdout.tar.gz`

**Durée** : 160 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Sortie standard de la tâche 0 : `"AB\n"`
- ⚠️ Les timings ne sont pas vérifiés

**Résultat attendu** :
```
JALON 1, TEST 9 (commande longue durée avec écriture):
Résultat : OK
```

---

#### TEST 10 : Commandes Externes Imprévisibles

**Fichier** : `exemple-arborescence-10-commandes-externes.tar.gz`

**Durée** : 62 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 56
- ✅ Sortie standard de la tâche 0 : `"1+1=2\n"`

**Note** : Ce test vérifie que le démon gère correctement les commandes externes qui peuvent avoir des comportements imprévisibles.

**Résultat attendu** :
```
JALON 1, TEST 10 (commandes externes imprévisibles):
Résultat : OK
```

---

#### TEST 11 : Arborescence Profonde

**Fichier** : `exemple-arborescence-11-arborescence-profonde.tar.gz`

**Durée** : 62 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Sortie standard de la tâche 30 contient toutes les combinaisons attendues

**Résultat attendu** :
```
JALON 1, TEST 11 (arborescence profonde):
Résultat : OK
```

---

#### TEST 12 : Valgrind (Détection de Fuites Mémoire)

**Fichier** : `exemple-arborescence-11-arborescence-profonde.tar.gz`

**Durée** : 62 secondes

**Prérequis** : ⚠️ **Valgrind doit être installé** (voir section Prérequis)

**Vérifications** :
- ✅ Le démon est exécuté avec `valgrind`
- ✅ Aucune fuite mémoire détectée
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ⚠️ Si valgrind détecte une erreur, le code de retour sera 33

**Commande valgrind utilisée** :
```bash
valgrind --error-exitcode=33 --errors-for-leak-kinds=definite ./erraid ...
```

**Résultat attendu** :
```
JALON 1, TEST 12 (valgrind):
Résultat : OK
```

**Si valgrind détecte une fuite** :
```
JALON 1, TEST 12 (valgrind):
Erreur : Exit code 33 detected for task X
This most likely means that `valgrind` detected a memory error in your program.
Résultat : échec (1 erreurs)
```

---

#### TEST 13 : Tâches Simultanées

**Fichier** : `exemple-arborescence-13-taches-simultanees.tar.gz`

**Durée** : 168 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Les tâches peuvent s'exécuter simultanément sans se bloquer mutuellement

**Note** : Ce test vérifie que le démon peut exécuter plusieurs tâches en parallèle sans que l'une bloque l'autre.

**Résultat attendu** :
```
JALON 1, TEST 13 (tâches simultanées):
Résultat : OK
```

---

#### TEST 14 : Tâche Jamais Lancée

**Fichier** : `exemple-arborescence-14-tache-jamais-lancee.tar.gz`

**Durée** : 122 secondes

**Vérifications** :
- ✅ Toutes les tâches s'exécutent avec le code de retour 0
- ✅ Les tâches qui ne doivent pas être exécutées ne le sont pas

**Résultat attendu** :
```
JALON 1, TEST 14 (tâche jamais lancée):
Résultat : OK
```

---

## Jalon 2 : Tests du Client (tadmor)

### Commande de Test

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
bash run-tadmor-tests-jalon-2.sh
```

### Résultat Attendu

```
Client: all tests passed
Jalon 2 : 11/11 tests réussis, 0 échecs
```

### Description des Tests

Les tests du client vérifient que `tadmor` peut :
- ✅ Envoyer correctement les requêtes au démon
- ✅ Interpréter correctement les réponses du démon
- ✅ Afficher les résultats dans le format attendu

#### Tests 1-11

Chaque test vérifie une requête spécifique :
- **Test 1** : LIST (liste vide)
- **Test 2** : LIST (avec tâches)
- **Test 3** : TIMES_EXITCODES (tâche existante)
- **Test 4** : TIMES_EXITCODES (tâche inexistante)
- **Test 5** : STDOUT (tâche existante)
- **Test 6** : STDOUT (tâche inexistante)
- **Test 7** : STDERR (tâche existante)
- **Test 8** : STDERR (tâche inexistante)
- **Test 9** : TERMINATE
- **Test 10** : Cas d'erreur divers
- **Test 11** : Format de sortie complexe

**Structure d'un test** :
- `arguments` : Arguments de la ligne de commande
- `request` : Requête binaire envoyée au démon
- `reply` : Réponse binaire attendue du démon
- `stdout` : Sortie standard attendue du client
- `exitcode` : Code de retour attendu

---

## Jalon 2 : Tests du Démon (erraid)

### Commande de Test

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
python3 run-erraid-tests-jalon-2.py
```

### Résultat Attendu

```
Jalon 2 : 11/11 tests réussis, 0 échecs
```

### Description des Tests

Les tests du démon vérifient que `erraid` peut :
- ✅ Recevoir correctement les requêtes du client
- ✅ Traiter les requêtes et générer les bonnes réponses
- ✅ Gérer les cas d'erreur (tâche inexistante, jamais exécutée)

#### Tests 1-11

Chaque test vérifie une requête spécifique :
- **Test 1** : LIST
- **Test 2** : TIMES_EXITCODES (tâche existante)
- **Test 3** : TIMES_EXITCODES (tâche inexistante)
- **Test 4** : TIMES_EXITCODES (tâche jamais exécutée)
- **Test 5** : STDOUT (tâche existante)
- **Test 6** : STDOUT (tâche inexistante)
- **Test 7** : STDOUT (tâche jamais exécutée)
- **Test 8** : STDERR (tâche existante)
- **Test 9** : STDERR (tâche inexistante)
- **Test 10** : STDERR (tâche jamais exécutée)
- **Test 11** : TERMINATE

**Structure d'un test** :
- `arborescence.tar.gz` : Arborescence de tâches à charger
- `arguments` : Arguments de la ligne de commande pour le démon
- `request` : Requête binaire envoyée par le client
- `reply` : Réponse binaire attendue du démon
- `stdout` : Sortie standard attendue du démon
- `exitcode` : Code de retour attendu

**Note** : Certains tests peuvent utiliser `valgrind` si un fichier `valgrind` est présent dans le répertoire du test.

---

## Exécution Complète de Tous les Tests

### Script Complet

Pour exécuter tous les tests en une seule fois :

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'

# Jalon 1
echo "=== JALON 1 ==="
python3 test-jalon-1.py

# Jalon 2 - Client
echo "=== JALON 2 - CLIENT ==="
bash run-tadmor-tests-jalon-2.sh

# Jalon 2 - Démon
echo "=== JALON 2 - DÉMON ==="
python3 run-erraid-tests-jalon-2.py
```

### Résultat Attendu

```
=== JALON 1 ===
Jalon 1 : 12/12 tests réussis, 0 échecs

=== JALON 2 - CLIENT ===
Client: all tests passed
Jalon 2 : 11/11 tests réussis, 0 échecs

=== JALON 2 - DÉMON ===
Jalon 2 : 11/11 tests réussis, 0 échecs
```

---

## Dépannage

### Test 12 Échoue (Valgrind)

**Symptôme** :
```
JALON 1, TEST 12 (valgrind):
Erreur : Exit code 33 detected
Résultat : échec
```

**Solutions** :
1. Installer valgrind (voir section Prérequis)
2. Vérifier que valgrind est dans le PATH : `which valgrind`
3. Vérifier les permissions : `valgrind --version`

### Tests du Jalon 2 Échouent

**Symptôme** : Tests échouent avec des erreurs de protocole

**Solutions** :
1. Vérifier que le démon ferme correctement le tube de réponse après chaque réponse
2. Vérifier le format de sérialisation (big-endian)
3. Vérifier que les réponses correspondent exactement au format attendu

### Timeout lors des Tests

**Symptôme** : Tests prennent trop de temps ou timeout

**Solutions** :
1. Vérifier que le démon n'est pas bloqué
2. Vérifier que les threads d'exécution fonctionnent correctement
3. Vérifier qu'il n'y a pas de boucles infinies

---

## Prochaines Étapes

- **Tests manuels** : [TESTS_MANUELS.md](TESTS_MANUELS.md)
- **Gestion des erreurs** : [ERREURS.md](ERREURS.md)
- **Commandes du client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)

