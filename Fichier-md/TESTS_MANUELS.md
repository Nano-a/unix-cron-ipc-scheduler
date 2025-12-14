# Guide de Test Manuel

Ce guide décrit comment tester manuellement le projet erraid-tadmor étape par étape.

---

## Préparation

### 1. Compilation

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
make clean
make
```

**Résultat attendu** :
- Compilation sans erreur
- Création des exécutables `erraid` et `tadmor`

### 2. Préparation de l'Environnement de Test

```bash
# Nettoyer les anciens tests
rm -rf /tmp/test-jalons

# Créer un répertoire de test
mkdir -p /tmp/test-jalons

# Extraire une arborescence d'exemple (optionnel, pour Jalon 1)
cd '/home/ajinou/Bureau/Projet System/sy5-2025-2026/Projet/exemples-arborescences'
tar -xzf exemple-arborescence-2.tar.gz
cp -r tmp-username-erraid/* /tmp/test-jalons/
```

---

## Tests Jalon 1

### Test 1 : Démarrage du Démon

**Terminal 1** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./erraid -r /tmp/test-jalons
```

**Résultat attendu** :
- Le démon démarre et reste actif
- Aucun message visible (sauf avec `-d`)
- Le démon ne s'arrête pas

**Vérification** :
```bash
# Dans un autre terminal
ls -la /tmp/test-jalons/pipes/
```

**Résultat attendu** :
```
erraid-request-pipe
erraid-reply-pipe
```

---

### Test 2 : Vérification de l'Exécution des Tâches

**Attendre 1-2 minutes** après le démarrage du démon.

**Vérification** :
```bash
# Vérifier qu'une tâche a été exécutée
hexdump -C /tmp/test-jalons/tasks/0/times-exitcodes | head -5
```

**Résultat attendu** : Des timestamps et codes de retour sont enregistrés.

---

### Test 3 : Vérification des Sorties

**Vérification** :
```bash
# Vérifier la sortie standard
cat /tmp/test-jalons/tasks/0/stdout

# Vérifier la sortie d'erreur
cat /tmp/test-jalons/tasks/0/stderr
```

**Résultat attendu** : Les sorties des commandes exécutées sont enregistrées.

---

### Test 4 : Vérification des Codes de Retour

**Vérification** :
```bash
# Vérifier les codes de retour dans times-exitcodes
# (Les codes de retour sont enregistrés en big-endian après chaque timestamp)
hexdump -C /tmp/test-jalons/tasks/0/times-exitcodes | head -3
```

**Résultat attendu** : Les timestamps et codes de retour sont enregistrés correctement.

---

### Test 5 : Tâches Simultanées (Optionnel)

**Prérequis** : Avoir plusieurs tâches qui s'exécutent à la même minute.

**Vérification** :
```bash
# Attendre qu'une minute passe avec plusieurs tâches éligibles
# Vérifier que toutes les tâches ont été exécutées
ls -la /tmp/test-jalons/tasks/*/times-exitcodes
```

**Résultat attendu** : Toutes les tâches éligibles sont exécutées, même simultanément.

---

## Tests Jalon 2

### Configuration : Deux Terminaux

**⚠️ IMPORTANT** : Ouvrir **deux terminaux** :
- **Terminal 1** : Démon `erraid`
- **Terminal 2** : Client `tadmor`

---

### Test 1 : LIST (Liste Vide)

**Terminal 1** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./erraid -r /tmp/test-jalons
```

**Terminal 2** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./tadmor -l -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Aucune sortie (liste vide)
- Code de retour : 0

---

### Test 2 : LIST (Avec Tâches)

**Prérequis** : Avoir une arborescence avec des tâches (voir préparation).

**Terminal 2** :
```bash
./tadmor -l -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
0: * * * echo Feed me lasagna
1: 0 * 1 echo I hate mondays
4: * * * echo -n Nous sommes le  ; date +%d/%m/%Y ; echo -n Il est  ; date +%H:%M:%S
```

---

### Test 3 : TIMES_EXITCODES (Tâche Exécutée)

**Prérequis** : Attendre que le démon exécute une tâche (au moins 1 minute).

**Terminal 2** :
```bash
./tadmor -x 0 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
2025-12-13 14:08:00 0
2025-12-13 14:09:00 0
2025-12-13 14:10:00 0
```

**Format** : `YYYY-MM-DD HH:MM:SS EXITCODE`

---

### Test 4 : TIMES_EXITCODES (Tâche Jamais Exécutée)

**Prérequis** : Utiliser une tâche qui existe mais n'a jamais été exécutée. 
Dans l'exemple d'arborescence, la tâche 1 existe mais peut ne pas avoir été exécutée 
si elle ne correspond pas au timing actuel.

**Terminal 2** :
```bash
# Tester avec une tâche qui existe mais n'a jamais été exécutée
# (par exemple, la tâche 1 si elle n'a pas encore été exécutée)
./tadmor -x 1 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Si la tâche a été exécutée : Liste des timestamps et exitcodes
- Si la tâche n'a jamais été exécutée : 
```
No runs recorded for this task.
```

**Note** : Si vous obtenez `ERROR: NOT_FOUND`, cela signifie que la tâche n'existe pas du tout 
(le répertoire de la tâche n'existe pas). Dans ce cas, utilisez une autre tâche qui existe.

---

### Test 5 : TIMES_EXITCODES (Tâche Inexistante)

**Terminal 2** :
```bash
./tadmor -x 99999 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
ERROR: NOT_FOUND
```

**Note** : Le message d'erreur est affiché sur `stderr`.

---

### Test 6 : STDOUT (Tâche Exécutée)

**Terminal 2** :
```bash
./tadmor -o 0 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
Feed me lasagna
```

---

### Test 7 : STDOUT (Tâche Jamais Exécutée)

**Prérequis** : Utiliser une tâche qui existe mais n'a jamais été exécutée.

**Terminal 2** :
```bash
# Tester avec une tâche qui existe mais n'a jamais été exécutée
# (par exemple, la tâche 1 si elle n'a pas encore été exécutée)
./tadmor -o 1 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
ERROR: NOT_RUN
```

**Note** : Si vous obtenez `ERROR: NOT_FOUND`, cela signifie que la tâche n'existe pas du tout.

---

### Test 8 : STDOUT (Tâche Inexistante)

**Terminal 2** :
```bash
./tadmor -o 99999 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
ERROR: NOT_FOUND
```

---

### Test 9 : STDERR (Tâche Exécutée)

**Terminal 2** :
```bash
./tadmor -e 0 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Si la tâche a produit des erreurs : Affiche le contenu
- Si pas d'erreur : Sortie vide (aucune sortie affichée)

**Note** : Pour tester avec une sortie stderr réelle, vous pouvez créer une tâche qui écrit sur stderr :
```bash
# Exemple : créer une tâche qui écrit sur stderr
# (nécessite la phase "rendu final" pour créer des tâches)
```

---

### Test 10 : STDERR (Tâche Jamais Exécutée)

**Prérequis** : Utiliser une tâche qui existe mais n'a jamais été exécutée.

**Terminal 2** :
```bash
# Tester avec une tâche qui existe mais n'a jamais été exécutée
# (par exemple, la tâche 1 si elle n'a pas encore été exécutée)
./tadmor -e 1 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
ERROR: NOT_RUN
```

**Note** : Si vous obtenez `ERROR: NOT_FOUND`, cela signifie que la tâche n'existe pas du tout.

---

### Test 11 : TERMINATE

**Terminal 2** :
```bash
./tadmor -q -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Aucune sortie (succès silencieux)
- Le démon dans le Terminal 1 s'arrête

**Vérification Terminal 1** :
- Le démon se termine proprement
- Les pipes sont fermés

---

### Test 12 : Client Sans Démon

**Prérequis** : Le démon n'est **pas** démarré.

**Terminal 2** :
```bash
./tadmor -l -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
Timeout: no response from daemon (is it running?)
```

**Comportement** : Le client attend 5 secondes, puis affiche le message d'erreur.

---

### Test 13 : Client avec Mauvais Répertoire

**Terminal 2** :
```bash
./tadmor -l -p /tmp/mauvais-repertoire/pipes
```

**Résultat attendu** :
```
open_pipes_client: No such file or directory
```

---

### Test 14 : TIMES_EXITCODES avec Plusieurs Exécutions

**Prérequis** : Attendre que le démon exécute une tâche plusieurs fois (au moins 2-3 minutes).

**Terminal 2** :
```bash
./tadmor -x 0 -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
```
2025-12-14 11:14:00 0
2025-12-14 11:15:00 0
2025-12-14 11:16:00 0
...
```

**Format** : Une ligne par exécution, format `YYYY-MM-DD HH:MM:SS EXITCODE`

---

### Test 15 : STDOUT avec Sortie Vide

**Prérequis** : Utiliser une tâche qui produit une sortie vide (ex: `echo -n` sans argument).

**Terminal 2** :
```bash
# Si une tâche produit une sortie vide
./tadmor -o TASKID -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Aucune sortie affichée (sortie vide)
- Code de retour : 0

---

### Test 16 : STDERR avec Contenu

**Prérequis** : Utiliser une tâche qui écrit sur stderr (ex: `echo "error" >&2`).

**Terminal 2** :
```bash
# Si une tâche a écrit sur stderr
./tadmor -e TASKID -p /tmp/test-jalons/pipes
```

**Résultat attendu** :
- Affiche le contenu de stderr
- Code de retour : 0

---

## Checklist de Vérification

### Jalon 1

- [x] Le démon démarre sans erreur
- [x] Les pipes sont créés correctement
- [x] Les tâches s'exécutent à la seconde 0 de chaque minute
- [x] Les fichiers `times-exitcodes` sont mis à jour
- [x] Les fichiers `stdout` et `stderr` sont créés
- [x] Les codes de retour sont enregistrés correctement
- [x] Les commandes longues continuent après `SIGTERM`
- [x] Les tâches simultanées s'exécutent correctement

### Jalon 2

- [x] `LIST` affiche toutes les tâches (liste vide et avec tâches)
- [x] `TIMES_EXITCODES` affiche l'historique correct (une ou plusieurs exécutions)
- [x] `TIMES_EXITCODES` gère correctement les cas : tâche exécutée, jamais exécutée, inexistante
- [x] `STDOUT` affiche la sortie standard (avec contenu et vide)
- [x] `STDERR` affiche la sortie d'erreur (avec contenu et vide)
- [x] `STDOUT`/`STDERR` gèrent correctement les cas : tâche exécutée, jamais exécutée, inexistante
- [x] `TERMINATE` arrête le démon proprement
- [x] Les erreurs sont gérées correctement (NOT_FOUND, NOT_RUN)
- [x] Le client gère le timeout si le démon n'est pas démarré
- [x] Le client gère les erreurs de répertoire invalide

---

## Prochaines Étapes

- **Tests automatiques** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
- **Gestion des erreurs** : [ERREURS.md](ERREURS.md)
- **Commandes** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md) et [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)

