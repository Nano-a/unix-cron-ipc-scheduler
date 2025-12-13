# Commandes du Client tadmor

## Syntaxe Générale

```bash
./tadmor [OPTIONS] [COMMAND]
```

---

## Options Communes

### `-p PIPES_DIR`

Spécifie le répertoire contenant les tubes de communication.

**Valeur par défaut** : `/tmp/$USER/erraid/pipes`

**⚠️ IMPORTANT** : Si le démon utilise `-r /tmp/test-erraid`, le client doit utiliser `-p /tmp/test-erraid/pipes` (avec `/pipes`).

**Exemple** :
```bash
./tadmor -p /tmp/test-erraid/pipes -l
```

---

## Commandes Consultatives

### 1. LIST : Lister toutes les tâches

**Syntaxe** :
```bash
./tadmor -l [-p PIPES_DIR]
```

**Exemple** :
```bash
./tadmor -l -p /tmp/test-erraid/pipes
```

**Résultat attendu** :
```
0: * * * echo Feed me lasagna
1: 0 * 1 echo I hate mondays
4: * * * echo -n Nous sommes le  ; date +%d/%m/%Y ; echo -n Il est  ; date +%H:%M:%S
```

**Format de sortie** :
```
TASKID: MINUTES HOURS DAYSOFWEEK COMMANDLINE
```

**Légende du timing** :
- `*` : Toutes les valeurs (toutes les minutes, toutes les heures, tous les jours)
- `-` : Aucune valeur (tâche abstraite, jamais exécutée)
- `0,15,30` : Valeurs spécifiques (minutes 0, 15 et 30)
- `1-5` : Plage de valeurs (jours 1 à 5, soit lundi à vendredi)

**Cas d'erreur** :
- Si le démon n'est pas démarré : `"Timeout: no response from daemon (is it running?)"`
- Si le répertoire des pipes n'existe pas : `"open_pipes_client: No such file or directory"`

---

### 2. TIMES_EXITCODES : Historique d'exécution

**Syntaxe** :
```bash
./tadmor -x TASKID [-p PIPES_DIR]
```

**Exemple** :
```bash
./tadmor -x 0 -p /tmp/test-erraid/pipes
```

**Résultat attendu (si des exécutions ont eu lieu)** :
```
2025-12-13 14:08:00 0
2025-12-13 14:09:00 0
2025-12-13 14:10:00 0
```

**Format de sortie** :
```
YYYY-MM-DD HH:MM:SS EXITCODE
```

**Cas spéciaux** :
- Si aucune exécution : `"No runs recorded for this task."`
- Si tâche inexistante : `"ERROR: NOT_FOUND"` (sur `stderr`)

**Exemple avec tâche jamais exécutée** :
```bash
$ ./tadmor -x 999 -p /tmp/test-erraid/pipes
No runs recorded for this task.
```

**Exemple avec tâche inexistante** :
```bash
$ ./tadmor -x 99999 -p /tmp/test-erraid/pipes
ERROR: NOT_FOUND
```

---

### 3. STDOUT : Sortie standard

**Syntaxe** :
```bash
./tadmor -o TASKID [-p PIPES_DIR]
```

**Exemple** :
```bash
./tadmor -o 0 -p /tmp/test-erraid/pipes
```

**Résultat attendu** :
```
Feed me lasagna
```

**Comportement** :
- Le contenu du fichier `stdout` est affiché directement sur `stdout` du client
- Si la tâche n'a jamais été exécutée : `"ERROR: NOT_RUN"` (sur `stderr`)
- Si la tâche n'existe pas : `"ERROR: NOT_FOUND"` (sur `stderr`)

**Exemple avec tâche jamais exécutée** :
```bash
$ ./tadmor -o 999 -p /tmp/test-erraid/pipes
ERROR: NOT_RUN
```

**Exemple avec sortie vide** :
```bash
$ ./tadmor -o 1 -p /tmp/test-erraid/pipes
(rien, sortie vide)
```

---

### 4. STDERR : Sortie d'erreur

**Syntaxe** :
```bash
./tadmor -e TASKID [-p PIPES_DIR]
```

**Exemple** :
```bash
./tadmor -e 0 -p /tmp/test-erraid/pipes
```

**Fonctionnement identique à STDOUT**, mais lit le fichier `stderr` au lieu de `stdout`.

**Résultat attendu** :
- Si la tâche a produit des erreurs : Affiche le contenu de `stderr`
- Si pas d'erreur : Sortie vide
- Si tâche jamais exécutée : `"ERROR: NOT_RUN"`
- Si tâche inexistante : `"ERROR: NOT_FOUND"`

---

### 5. TERMINATE : Arrêter le démon

**Syntaxe** :
```bash
./tadmor -q [-p PIPES_DIR]
```

**Exemple** :
```bash
./tadmor -q -p /tmp/test-erraid/pipes
```

**Résultat attendu** :
- Aucune sortie (succès silencieux)
- Le démon s'arrête proprement

**Comportement** :
1. Le client envoie une requête `TERMINATE` au démon
2. Le démon reçoit la requête et arrête son exécution
3. Le démon ferme les pipes et termine
4. Le client reçoit la réponse `OK` et termine

---

## Codes de Retour

- **0** : Succès
- **1** : Erreur système (allocation mémoire, etc.)
- **2** : Erreur d'utilisation (option invalide, argument manquant, etc.)

---

## Exemples Complets

### Scénario 1 : Consultation Basique

```bash
# Terminal 1 : Démon
./erraid -r /tmp/test-erraid

# Terminal 2 : Client
./tadmor -l -p /tmp/test-erraid/pipes
./tadmor -x 0 -p /tmp/test-erraid/pipes
./tadmor -o 0 -p /tmp/test-erraid/pipes
./tadmor -e 0 -p /tmp/test-erraid/pipes
```

### Scénario 2 : Gestion des Erreurs

```bash
# Tâche inexistante
./tadmor -x 99999 -p /tmp/test-erraid/pipes
# Résultat : ERROR: NOT_FOUND

# Tâche jamais exécutée
./tadmor -o 999 -p /tmp/test-erraid/pipes
# Résultat : ERROR: NOT_RUN

# Démon non démarré
./tadmor -l -p /tmp/test-erraid/pipes
# Résultat : Timeout: no response from daemon (is it running?)
```

### Scénario 3 : Arrêt du Démon

```bash
# Terminal 2 : Client
./tadmor -q -p /tmp/test-erraid/pipes

# Terminal 1 : Le démon s'arrête
```

---

## Prochaines Étapes

- **Commandes du démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Tests** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
- **Gestion des erreurs** : [ERREURS.md](ERREURS.md)

