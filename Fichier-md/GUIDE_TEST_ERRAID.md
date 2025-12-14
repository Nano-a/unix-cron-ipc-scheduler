# Guide de Test Complet pour `erraid`

## Préparation

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
make
rm -rf /tmp/test-erraid
```

---

## Test 1 : Option `-h` (Help)

### Commande
```bash
./erraid -h
```

### Résultat Attendu
```
Usage: erraid [OPTIONS]
Options:
  -r RUN_DIRECTORY    Répertoire de stockage (défaut: /tmp/$USER/erraid)
  -p PIPES_DIR        Répertoire des pipes (défaut: RUN_DIRECTORY/pipes)
  -d                  Activer les logs de débogage
  -h                  Afficher cette aide
```

### Code de Retour
- **0** : Succès

---

## Test 2 : Option `-r` (Démarrage Simple)

### Commande
```bash
./erraid -r /tmp/test-erraid
```

### Résultat Attendu
- **Aucune sortie visible** (le démon tourne en silence)
- Le démon reste actif et attend les requêtes

### Vérifications
```bash
# Dans un autre terminal, vérifier que le démon tourne
ps aux | grep erraid | grep -v grep
```

**Résultat attendu** : Une ligne avec le processus `erraid`

```bash
# Vérifier que les répertoires sont créés
ls -la /tmp/test-erraid/
```

**Résultat attendu** :
```
drwxrwxr-x 3 ajinou ajinou 4096 ... tasks
drwxrwxr-x 2 ajinou ajinou  80 ... pipes
```

```bash
# Vérifier que les pipes sont créés
ls -la /tmp/test-erraid/pipes/
```

**Résultat attendu** :
```
prw-rw-r-- 1 ajinou ajinou 0 ... erraid-request-pipe
prw-rw-r-- 1 ajinou ajinou 0 ... erraid-reply-pipe
```

### Arrêt
- **Ctrl+C** dans le terminal où le démon tourne
- **OU** dans un autre terminal : `./tadmor -q -p /tmp/test-erraid/pipes`

### Code de Retour
- **0** : Arrêt normal

---

## Test 3 : Option `-r -d` (Démarrage avec Debug)

### Commande
```bash
./erraid -r /tmp/test-erraid -d
```

### Résultat Attendu
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

### Comportement
- Le démon affiche des messages de débogage sur **stderr**
- Les messages apparaissent lors des événements :
  - Démarrage du démon
  - Réception d'une requête client
  - Traitement d'une requête
  - Arrêt du démon

### Test avec Requête Client
**Terminal 1** (démon) :
```bash
./erraid -r /tmp/test-erraid -d
```

**Terminal 2** (client) :
```bash
./tadmor -l -p /tmp/test-erraid/pipes
```

**Résultat Terminal 1** :
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] Processing client request
[DEBUG] Processing client request (reply pipe reopened: fd=4)
```

**Résultat Terminal 2** :
```
(Liste des tâches ou liste vide)
```

### Arrêt
- **Ctrl+C** dans Terminal 1
- **OU** dans Terminal 2 : `./tadmor -q -p /tmp/test-erraid/pipes`

**Résultat Terminal 1 après `-q`** :
```
[DEBUG] Processing client request
[DEBUG] Handling TERMINATE request
[DEBUG] TERMINATE: g_stop set to 1, sending response
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
[DEBUG] daemon_loop exiting (g_stop=1, iterations=X)
```

### Code de Retour
- **0** : Arrêt normal

---

## Test 4 : Option `-p` (Pipes Personnalisés) - ⚠️ NON IMPLÉMENTÉE

### ⚠️ Note Importante
L'option `-p` **n'est PAS implémentée** dans le démon `erraid` pour Jalon 1 et 2.
Le démon utilise toujours `RUN_DIRECTORY/pipes` comme répertoire des pipes.

### Commande (ne fonctionnera PAS)
```bash
./erraid -r /tmp/test-erraid -p /tmp/mes-pipes
```

### Résultat Attendu
```
./erraid: invalid option -- 'p'
Usage: ./erraid [-r <run_dir>] [-d]
  -r <run_dir>  Répertoire d'exécution (défaut: /tmp/$USER/erraid)
  -d            Activer les logs de debug
  -h, --help    Afficher cette aide
```

### Code de Retour
- **1** : Erreur (option invalide)

### ⚠️ Comportement Actuel
Le démon crée toujours les pipes dans `RUN_DIRECTORY/pipes` :
- Si `-r /tmp/test-erraid` → pipes dans `/tmp/test-erraid/pipes`
- Pas de possibilité de personnaliser le répertoire des pipes pour le démon
- Le client `tadmor` peut utiliser `-p` pour spécifier où se trouvent les pipes

---

## Test 5 : Démarrage en Arrière-Plan

### Commande
```bash
./erraid -r /tmp/test-erraid > /tmp/erraid.log 2>&1 &
```

### Résultat Attendu
- Le démon démarre en arrière-plan
- Aucune sortie dans le terminal
- Les logs sont redirigés vers `/tmp/erraid.log`

### Vérifications
```bash
# Vérifier que le démon tourne
ps aux | grep erraid | grep -v grep
```

**Résultat attendu** : Une ligne avec le processus `erraid`

```bash
# Voir les logs (si -d n'est pas utilisé, le fichier sera vide)
cat /tmp/erraid.log
```

**Résultat attendu** : Fichier vide (sans `-d`) ou logs de debug (avec `-d`)

```bash
# Suivre les logs en temps réel
tail -f /tmp/erraid.log
```

### Test avec Client
**Terminal 1** (suivre les logs) :
```bash
tail -f /tmp/erraid.log
```

**Terminal 2** (client) :
```bash
./tadmor -l -p /tmp/test-erraid/pipes
```

**Résultat Terminal 2** :
```
(Liste des tâches ou liste vide)
```

### Arrêt
```bash
# Trouver le PID
ps aux | grep erraid | grep -v grep

# Arrêter via TERMINATE
./tadmor -q -p /tmp/test-erraid/pipes

# OU arrêter via signal
kill <PID>
```

### Code de Retour
- **0** : Arrêt normal

---

## Test 6 : Arrêt via TERMINATE (Client)

### Préparation
**Terminal 1** (démon) :
```bash
./erraid -r /tmp/test-erraid -d
```

### Commande
**Terminal 2** (client) :
```bash
./tadmor -q -p /tmp/test-erraid/pipes
```

### Résultat Attendu

**Terminal 1** (démon) :
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] Processing client request
[DEBUG] Handling TERMINATE request
[DEBUG] TERMINATE: g_stop set to 1, sending response
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
[DEBUG] daemon_loop exiting (g_stop=1, iterations=X)
```

**Terminal 2** (client) :
```
(Aucune sortie, retour immédiat)
```

### Timing
- **Arrêt instantané** (moins de 100ms)
- Le démon se termine proprement

### Code de Retour
- **0** : Arrêt normal (démon)
- **0** : Succès (client)

---

## Test 7 : Arrêt via Signal (SIGTERM)

### Préparation
**Terminal 1** (démon) :
```bash
./erraid -r /tmp/test-erraid -d
```

### Commande
**Terminal 2** :
```bash
# Trouver le PID
ps aux | grep erraid | grep -v grep

# Envoyer SIGTERM
kill <PID>
```

### Résultat Attendu

**Terminal 1** (démon) :
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] daemon_loop exiting (g_stop=1, iterations=X)
```

**OU** (si le signal est intercepté) :
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
(Signal reçu, arrêt propre)
```

### Timing
- **Arrêt rapide** (moins de 1 seconde)
- Le démon se termine proprement

### Code de Retour
- **0** : Arrêt normal

---

## Test 8 : Arrêt via Signal (SIGINT - Ctrl+C)

### Préparation
**Terminal 1** (démon) :
```bash
./erraid -r /tmp/test-erraid -d
```

### Commande
**Terminal 1** : Appuyer sur **Ctrl+C**

### Résultat Attendu

**Terminal 1** (démon) :
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
^C[DEBUG] daemon_loop exiting (g_stop=1, iterations=X)
```

### Timing
- **Arrêt rapide** (moins de 1 seconde)
- Le démon se termine proprement

### Code de Retour
- **0** : Arrêt normal

---

## Test 9 : Codes de Retour (Erreurs)

### Test 9.1 : Répertoire Invalide
```bash
./erraid -r /tmp/inexistant/chemin/impossible
```

**Résultat attendu** :
```
erraid: Impossible de créer le répertoire
```

**Code de Retour** : **1** (erreur)

### Test 9.2 : Pipes Invalides
```bash
./erraid -r /tmp/test-erraid -p /tmp/inexistant/chemin/impossible
```

**Résultat attendu** :
```
open_pipes_daemon: No such file or directory
```

**Code de Retour** : **1** (erreur)

---

## Test 10 : Comportement avec Tâches Existantes

### Préparation
```bash
# Créer une arborescence de test
mkdir -p /tmp/test-erraid/tasks/0
mkdir -p /tmp/test-erraid/tasks/0/cmd
echo "0 * * 0,2-6" > /tmp/test-erraid/tasks/0/timing
echo "echo Feed me lasagna" > /tmp/test-erraid/tasks/0/cmd/0
```

### Commande
```bash
./erraid -r /tmp/test-erraid -d
```

### Résultat Attendu
- Le démon démarre et charge la tâche existante
- Aucun message visible (sauf avec `-d`)

### Vérification
**Terminal 2** (client) :
```bash
./tadmor -l -p /tmp/test-erraid/pipes
```

**Résultat Terminal 2** :
```
0: * * 0,2-6 echo Feed me lasagna
```

### Arrêt
- **Ctrl+C** dans Terminal 1
- **OU** dans Terminal 2 : `./tadmor -q -p /tmp/test-erraid/pipes`

---

## Résumé des Tests

| Test | Commande | Résultat Attendu | Code Retour |
|------|----------|-------------------|-------------|
| 1. Help | `./erraid -h` | Affiche l'aide | 0 |
| 2. Démarrage simple | `./erraid -r /tmp/test-erraid` | Démon actif, pas de sortie | 0 |
| 3. Debug | `./erraid -r /tmp/test-erraid -d` | Logs de debug | 0 |
| 4. Pipes personnalisés | `./erraid -r /tmp/test-erraid -p /tmp/mes-pipes` | ⚠️ Option invalide | 1 |
| 5. Arrière-plan | `./erraid -r /tmp/test-erraid &` | Démon en arrière-plan | 0 |
| 6. Arrêt TERMINATE | `./tadmor -q -p /tmp/test-erraid/pipes` | Arrêt instantané | 0 |
| 7. Arrêt SIGTERM | `kill <PID>` | Arrêt rapide | 0 |
| 8. Arrêt SIGINT | `Ctrl+C` | Arrêt rapide | 0 |
| 9. Erreurs | Répertoire invalide | Message d'erreur | 1 |
| 10. Tâches existantes | Démarrage avec tâches | Tâches chargées | 0 |

---

## Notes Importantes

1. **Ordre des Options** : L'ordre des options n'a pas d'importance (`-r -d` = `-d -r`)
2. **Répertoires** : Les répertoires sont créés automatiquement s'ils n'existent pas
3. **Pipes** : Les pipes sont créés automatiquement dans le répertoire spécifié
4. **Threads** : Le démon utilise des threads pour l'exécution des tâches et la gestion des requêtes
5. **Arrêt Propre** : Le démon se termine proprement en fermant les pipes et en arrêtant les threads

