# Gestion des Erreurs

Ce document décrit les erreurs courantes et leurs solutions.

---

## Erreurs du Client

### 1. Démon Non Démarré

**Symptôme** :
```
Timeout: no response from daemon (is it running?)
```

**Cause** : Le démon n'est pas démarré ou les pipes ne sont pas accessibles.

**Solution** :
1. Vérifier que le démon est démarré :
   ```bash
   ps aux | grep erraid
   ```
2. Démarrer le démon :
   ```bash
   ./erraid -r /tmp/test-jalons
   ```
3. Vérifier que les pipes existent :
   ```bash
   ls -la /tmp/test-jalons/pipes/
   ```

---

### 2. Répertoire des Pipes Inexistant

**Symptôme** :
```
open_pipes_client: No such file or directory
```

**Cause** : Le répertoire spécifié avec `-p` n'existe pas.

**Solution** :
1. Vérifier que le démon a créé les pipes :
   ```bash
   ls -la /tmp/test-jalons/pipes/
   ```
2. Utiliser le bon chemin (avec `/pipes`) :
   ```bash
   ./tadmor -l -p /tmp/test-jalons/pipes
   ```
3. Si le démon utilise `-r /tmp/test-erraid`, le client doit utiliser `-p /tmp/test-erraid/pipes`

---

### 3. Tâche Inexistante

**Symptôme** :
```
ERROR: NOT_FOUND
```

**Cause** : Le `TASKID` spécifié n'existe pas dans l'arborescence.

**Solution** :
1. Utiliser `-l` pour lister les tâches disponibles :
   ```bash
   ./tadmor -l -p /tmp/test-jalons/pipes
   ```
2. Utiliser un `TASKID` valide

---

### 4. Tâche Jamais Exécutée

**Symptôme** :
```
ERROR: NOT_RUN
```

**Cause** : La tâche existe mais n'a jamais été exécutée (fichier `stdout` ou `stderr` absent).

**Solution** :
1. Attendre que le démon exécute la tâche (au moins 1 minute)
2. Vérifier le timing de la tâche :
   ```bash
   ./tadmor -l -p /tmp/test-jalons/pipes
   ```
3. Vérifier que la tâche a bien été exécutée :
   ```bash
   ./tadmor -x TASKID -p /tmp/test-jalons/pipes
   ```

---

## Erreurs du Démon

### 1. Répertoire de Stockage Inexistant

**Symptôme** :
```
mkdir: cannot create directory '/tmp/test': Permission denied
```

**Cause** : Permissions insuffisantes pour créer le répertoire.

**Solution** :
1. Utiliser un répertoire accessible (ex: `/tmp/$USER/erraid`)
2. Vérifier les permissions :
   ```bash
   ls -ld /tmp
   ```
3. Utiliser un répertoire dans `/tmp/$USER/` ou demander les permissions nécessaires

---

### 2. Pipes Déjà Existants

**Symptôme** : Le démon démarre normalement (pas d'erreur).

**Comportement** : Le démon réutilise les pipes existants.

**Note** : Si un autre démon utilise déjà les pipes, il peut y avoir des conflits.

**Solution** :
1. Vérifier qu'aucun autre démon n'utilise les pipes :
   ```bash
   ps aux | grep erraid
   ```
2. Utiliser un répertoire différent :
   ```bash
   ./erraid -r /tmp/test-erraid-2
   ```

---

### 3. Arborescence Invalide

**Symptôme** : Le démon démarre mais n'exécute pas les tâches.

**Cause** : Format de l'arborescence incorrect ou fichiers manquants.

**Solution** :
1. Vérifier la structure de l'arborescence :
   ```bash
   ls -la /tmp/test-jalons/tasks/
   ```
2. Vérifier que chaque tâche a les fichiers requis :
   - `timing`
   - `cmd/`
3. Vérifier le format selon `sy5-2025-2026/Projet/arborescence.md`

---

### 4. Erreur de Compilation

**Symptôme** :
```
error: ...
warning: ...
```

**Cause** : Erreur dans le code source.

**Solution** :
1. Vérifier les erreurs de compilation :
   ```bash
   make clean
   make 2>&1 | grep -E "error|warning"
   ```
2. Corriger les erreurs
3. Recompiler

---

## Erreurs des Tests

### 1. Test 12 Échoue (Valgrind)

**Symptôme** :
```
JALON 1, TEST 12 (valgrind):
Erreur : Exit code 33 detected
Résultat : échec
```

**Cause** : Valgrind a détecté une fuite mémoire ou une erreur.

**Solution** :
1. Installer valgrind (voir [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md#installation-de-valgrind-test-12-du-jalon-1))
2. Exécuter valgrind manuellement :
   ```bash
   valgrind --leak-check=full ./erraid -r /tmp/test-jalons
   ```
3. Corriger les fuites mémoire détectées

---

### 2. Tests du Jalon 2 Échouent

**Symptôme** : Tests échouent avec des erreurs de protocole.

**Cause** : Format de sérialisation incorrect ou tube de réponse non fermé.

**Solution** :
1. Vérifier que le démon ferme correctement le tube de réponse après chaque réponse
2. Vérifier le format de sérialisation (big-endian)
3. Vérifier que les réponses correspondent exactement au format attendu

---

### 3. Timeout lors des Tests

**Symptôme** : Tests prennent trop de temps ou timeout.

**Cause** : Le démon est bloqué ou les threads ne fonctionnent pas correctement.

**Solution** :
1. Vérifier que le démon n'est pas bloqué
2. Vérifier que les threads d'exécution fonctionnent correctement
3. Vérifier qu'il n'y a pas de boucles infinies

---

## Codes de Retour

### Client (`tadmor`)

- **0** : Succès
- **1** : Erreur système (allocation mémoire, etc.)
- **2** : Erreur d'utilisation (option invalide, argument manquant, etc.)

### Démon (`erraid`)

- **0** : Arrêt normal
- **1** : Erreur au démarrage (impossible d'initialiser les répertoires/pipes)

---

## Dépannage Général

### Vérifier que le Démon Tourne

```bash
ps aux | grep erraid
```

### Vérifier les Pipes

```bash
ls -la /tmp/test-jalons/pipes/
```

### Vérifier les Logs

```bash
# Si le démon a été lancé avec redirection
cat /tmp/erraid.log

# Si le démon a été lancé avec -d
# Les logs sont sur stderr
```

### Nettoyer et Recommencer

```bash
# Arrêter le démon
pkill erraid

# Nettoyer les répertoires de test
rm -rf /tmp/test-jalons

# Recompiler
make clean
make

# Redémarrer
./erraid -r /tmp/test-jalons
```

---

## Prochaines Étapes

- **Tests** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
- **Tests manuels** : [TESTS_MANUELS.md](TESTS_MANUELS.md)
- **Commandes** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md) et [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)

