# Guide Complet du Projet erraid-tadmor

## 📋 Table des matières

1. [Commandes demandées par le professeur](#commandes-demandées-par-le-professeur)
2. [État d'implémentation](#état-dimplémentation)
3. [Tests du professeur](#tests-du-professeur)
4. [Guide d'utilisation des commandes](#guide-dutilisation-des-commandes)
5. [Tous les cas possibles](#tous-les-cas-possibles)
6. [Messages d'erreur](#messages-derreur)

---

## Commandes demandées par le professeur

### Démon `erraid`

#### Options requises :
- `-R RUN_DIR` : Définition du répertoire de stockage (par défaut `/tmp/$USER/erraid`)
- `-P PIPES_DIR` : Définition du répertoire contenant les tubes de communication (par défaut `<run_dir>/pipes`)
- `-F` : Exécution en avant-plan sans démonisation
- `-d` : Activer les logs de debug
- `-h, --help` : Afficher l'aide

#### Fonctionnalités :
- Exécution des tâches aux dates prescrites
- Réception et traitement des requêtes du client
- Stockage persistant sur disque
- Reprise après interruption (chargement des tâches au démarrage)
- Arrêt propre via TERMINATE

### Client `tadmor`

#### Options de création/suppression :
- `-c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] CMD [ARG_1] ... [ARG_N]` : Création d'une tâche simple
- `-s [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] TASKID_1 ... TASKID_N` : Combinaison séquentielle
- `-p [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] TASKID_1 ... TASKID_N` : Combinaison en pipeline
- `-i [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] TASKID_1 TASKID_2 [TASKID_3]` : Combinaison conditionnelle
- `-n` : Tâche abstraite (sans horaire d'exécution)
- `-r TASKID` : Suppression d'une tâche

#### Options de consultation :
- `-l` : Liste des tâches avec identifiant
- `-x TASKID` : Liste datée des valeurs de retour
- `-o TASKID` : Sortie standard de la dernière exécution
- `-e TASKID` : Erreur standard de la dernière exécution

#### Options diverses :
- `-P PIPES_DIR` : Définition du répertoire des tubes (défaut `/tmp/$USER/erraid/pipes`)
- `-q` : Arrêt du démon

---

## État d'implémentation

### ✅ Implémenté et fonctionnel

#### Démon `erraid` :
- ✅ Options `-R`, `-P`, `-F`, `-d`, `-h`
- ✅ Support des anciennes options `-r`, `-p` (compatibilité)
- ✅ Exécution des tâches simples (type `SI`)
- ✅ Exécution des séquences (type `SQ`)
- ✅ Exécution des pipelines (type `PL`)
- ✅ Exécution des conditionnelles (type `IF`)
- ✅ Gestion des requêtes client (LIST, CREATE, COMBINE, REMOVE, TIMES_EXITCODES, STDOUT, STDERR, TERMINATE)
- ✅ Stockage persistant sur disque
- ✅ Reprise après interruption
- ✅ Arrêt propre

#### Client `tadmor` :
- ✅ Option `-c` avec timing complet
- ✅ Option `-s` pour séquences
- ✅ Option `-p` pour pipelines
- ✅ Option `-i` pour conditionnelles
- ✅ Option `-n` pour tâches abstraites
- ✅ Option `-r` pour suppression
- ✅ Option `-l` avec formatage correct
- ✅ Option `-x` pour historique
- ✅ Option `-o` pour stdout
- ✅ Option `-e` pour stderr
- ✅ Option `-P` pour pipes_dir
- ✅ Option `-q` pour arrêt du démon
- ✅ Parsing complet des timings (`*`, `-`, plages, listes)
- ✅ Format d'affichage `-l` correct pour toutes les combinaisons

### ⚠️ Compatibilité maintenue

- Support des anciennes options `-r` et `-p` dans `erraid` (pour compatibilité avec les tests)
- Support de l'ancienne option `-p` dans `tadmor` (détection automatique : path vs taskid)

---

## Tests du professeur

### Jalon 1

**Terminal utilisé :** Terminal 2 (ou Terminal 1, peu importe)

**État du démon :** Le démon sera démarré automatiquement par les tests

**Commande pour lancer les tests :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3/tests-prof
python3 test-jalon-1.py
```

**Résultat attendu :**
```
Jalon 1 : 12/12 tests réussis, 0 échecs
```

**Explication du résultat :**
- Les tests du professeur démarrent automatiquement le démon avec une arborescence préexistante.
- Chaque test vérifie un aspect spécifique du fonctionnement du démon.
- Tous les tests doivent passer pour valider le Jalon 1.
- Les tests prennent environ 3-4 minutes au total (chaque test attend l'exécution des tâches).

**Ce qui est testé :**
- **TEST 1** : Arborescence simple (2 tâches simples)
  - Vérifie que les tâches s'exécutent avec le code de retour 0
  - Vérifie les sorties standard correctes
  - Durée : ~182 secondes (3 minutes)
- **TEST 2** : Arborescence avec séquences
  - Vérifie l'exécution des séquences de commandes
  - Vérifie les sorties standard des séquences
  - Durée : ~182 secondes
- **TEST 3** : Arborescence avec séquences imbriquées
  - Vérifie l'exécution des séquences de séquences
  - Vérifie les sorties standard correctes
  - Durée : ~182 secondes
- **TEST 4-11** : Autres tests de fonctionnalités
  - Tâches simultanées
  - Commandes avec fork
  - Commandes de longue durée
  - Gestion des signaux
- **TEST 12** : Détection de fuites mémoire (valgrind)
  - Vérifie qu'il n'y a pas de fuites mémoire
  - Nécessite `valgrind` installé
  - Durée : variable selon la machine

**Prérequis :**
- Compilation sans erreur (`make`)
- Installation de `valgrind` pour le test 12 (optionnel mais recommandé)

**Installation de valgrind :**
```bash
sudo apt-get update
sudo apt-get install -y valgrind
```

**Vérification :**
```bash
valgrind --version
```

**Résultat attendu :** Affiche la version de valgrind (ex: `valgrind-3.18.1`).

### Jalon 2

#### Tests `tadmor` (client)

**Terminal utilisé :** Terminal 2 (ou Terminal 1)

**État du démon :** Le démon sera démarré automatiquement par les tests

**Commande pour lancer les tests :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3/tests-prof
bash run-tadmor-tests-jalon-2.sh
```

**Résultat attendu :**
```
Client: all tests passed
```

**Explication du résultat :**
- Les tests du professeur démarrent automatiquement le démon.
- Chaque test vérifie une fonctionnalité spécifique du client.
- Tous les tests doivent passer pour valider le Jalon 2 (client).
- Les tests vérifient le format des requêtes et des réponses.

**Ce qui est testé :**
- Requête `LIST` (liste vide et avec tâches)
- Requête `TIMES_EXITCODES` (tâche exécutée, jamais exécutée, inexistante)
- Requête `STDOUT` (avec contenu, vide, jamais exécutée, inexistante)
- Requête `STDERR` (avec contenu, vide, jamais exécutée, inexistante)
- Requête `TERMINATE` (arrêt du démon)
- Gestion des erreurs (`NOT_FOUND`, `NOT_RUN`)
- Format de sérialisation des requêtes et réponses
- Communication via FIFO (pipes nommés)

#### Tests `erraid` (démon)

**Terminal utilisé :** Terminal 2 (ou Terminal 1)

**État du démon :** Le démon sera démarré automatiquement par les tests

**Commande pour lancer les tests :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3/tests-prof
python3 run-erraid-tests-jalon-2.py
```

**Résultat attendu :**
```
Jalon 2 : 11/11 tests réussis, 0 échecs
```

**Explication du résultat :**
- Les tests du professeur démarrent automatiquement le démon.
- Chaque test vérifie une fonctionnalité spécifique du démon.
- Tous les tests doivent passer pour valider le Jalon 2 (démon).
- Les tests vérifient le traitement des requêtes et le format des réponses.

**Ce qui est testé :**
- Traitement des requêtes `LIST` (liste vide et avec tâches)
- Traitement des requêtes `TIMES_EXITCODES` (tâche exécutée, jamais exécutée, inexistante)
- Traitement des requêtes `STDOUT` (avec contenu, vide, jamais exécutée, inexistante)
- Traitement des requêtes `STDERR` (avec contenu, vide, jamais exécutée, inexistante)
- Traitement des requêtes `TERMINATE` (arrêt propre)
- Gestion des erreurs (`NOT_FOUND`, `NOT_RUN`)
- Format de sérialisation des réponses (big-endian)
- Communication via FIFO (pipes nommés)
- Fermeture correcte des pipes après chaque réponse

---

## Guide d'utilisation des commandes

### Préparation de l'environnement

#### Terminal 1 : Démarrage du démon

**État initial :** Aucun démon en cours d'exécution.

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu :** 
- Le démon démarre et reste actif (aucun message visible en mode normal, logs de debug si `-d` est utilisé).
- Le processus reste en cours d'exécution dans ce terminal.
- Les pipes sont créés dans `/tmp/test-erraid/pipes/`.

**Explication :** 
- `-F` force l'exécution en avant-plan (sans démonisation), donc le processus reste visible dans le terminal.
- `-R` définit le répertoire de stockage des tâches.
- `-P` définit le répertoire des pipes de communication.
- Le démon crée automatiquement les pipes `erraid-request-pipe` et `erraid-reply-pipe` s'ils n'existent pas.
- Le démon charge automatiquement toutes les tâches existantes depuis `/tmp/test-erraid/tasks/` au démarrage.
- Le démon entre dans sa boucle principale et attend les requêtes client et vérifie périodiquement les tâches à exécuter.

**Vérification (dans un autre terminal) :**
```bash
# Vérifier que les pipes sont créés
ls -la /tmp/test-erraid/pipes/
```

**Résultat attendu :**
```
total 0
drwxrwxr-x 2 ajinou ajinou 80 ... .
drwxrwxr-x 3 ajinou ajinou 60 ... ..
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-reply-pipe
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-request-pipe
```

**Explication :**
- `ls -la` affiche toujours `.` (répertoire courant) et `..` (répertoire parent) en premier.
- Les pipes sont ensuite affichés dans l'ordre alphabétique : `erraid-reply-pipe` vient avant `erraid-request-pipe` (car 'r' vient avant 'q' dans l'alphabet).
- Les pipes nommés (FIFO) sont créés avec les permissions par défaut. Le `p` au début indique que ce sont des pipes.
- **Note :** Le code crée les pipes dans l'ordre `erraid-request-pipe` puis `erraid-reply-pipe`, mais `ls` les affiche dans l'ordre alphabétique, ce qui est normal et attendu.

**État après :** Le démon est en cours d'exécution dans Terminal 1.

---

#### Terminal 1 : Démarrage du démon avec logs de debug (`-d`)

**État initial :** Aucun démon en cours d'exécution.

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Explication :**
- `-d` active les logs de débogage qui sont écrits sur `stderr`.
- Le message indique que la boucle principale du démon a démarré.
- `request_fd=3` : Descripteur de fichier pour le pipe de requêtes (client → démon).
- `reply_fd=4` : Descripteur de fichier pour le pipe de réponses (démon → client).
- Le démon reste actif et affichera d'autres messages de debug lors des événements (réception de requêtes, exécution de tâches, etc.).

**Comportement :**
- Les messages de debug apparaissent lors des événements :
  - Démarrage du démon
  - Réception d'une requête client
  - Traitement d'une requête
  - Exécution d'une tâche
  - Arrêt du démon

**État après :** Le démon est en cours d'exécution dans Terminal 1 avec logs de debug activés.

---

#### Terminal 1 : Démarrage du démon en arrière-plan

**État initial :** Aucun démon en cours d'exécution.

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes > /tmp/erraid.log 2>&1 &
```

**Résultat attendu dans Terminal 1 :**
```
[1] 92682
```
(où `[1]` est le numéro du job en arrière-plan et `92682` est le PID du processus)

**Explication Terminal 1 :**
- `&` : Lance le processus en arrière-plan.
- Bash affiche toujours `[job_number] PID` quand on lance un processus en arrière-plan.
- Le prompt réapparaît immédiatement après.
- Les logs sont redirigés vers `/tmp/erraid.log` (pas de sortie visible dans le terminal).
- `> /tmp/erraid.log` : Redirige stdout vers le fichier de log.
- `2>&1` : Redirige stderr vers stdout (donc aussi vers le fichier de log).
- Le démon continue de fonctionner même si le terminal est fermé.

**Vérification dans Terminal 2 :**
```bash
# Vérifier que le démon tourne
ps aux | grep erraid | grep -v grep
```

**Résultat attendu dans Terminal 2 :**
```
ajinou     92682  0.0  0.0  76696  1804 pts/2    Sl   10:47   0:00 ./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes
```

**Explication Terminal 2 :**
- Le processus `erraid` est visible dans la liste des processus.
- Le PID correspond à celui affiché dans Terminal 1 (`92682` dans cet exemple).
- **Note importante :** Si vous voyez plusieurs processus `erraid`, il peut y avoir d'anciens processus qui tournent encore. Vous pouvez les tuer avec `kill <PID>` ou `pkill -f "erraid"` avant de redémarrer.

**Voir les logs dans Terminal 2 :**
```bash
# Voir les logs (si -d n'est pas utilisé, le fichier sera vide)
cat /tmp/erraid.log

# OU suivre les logs en temps réel
tail -f /tmp/erraid.log
```

**Résultat attendu dans Terminal 2 :**
- Sans `-d` : Fichier vide (aucun message). **C'est normal**, le démon n'affiche rien sans `-d`.
- Avec `-d` : Messages de debug dans le fichier (ex: `[DEBUG] daemon_loop started, request_fd=3, reply_fd=4`).

**État après :** Le démon est en cours d'exécution en arrière-plan.

---

#### Arrêt du démon en arrière-plan

**État du démon :** ✅ **EN COURS D'EXÉCUTION** en arrière-plan

**⚠️ Important :** Quand le démon tourne en arrière-plan, **Ctrl+C ne fonctionne pas** car le processus n'est pas dans le terminal actif. Il faut utiliser une des méthodes suivantes :

**Méthode 1 : Utiliser `tadmor -q` (recommandé - arrêt propre)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu dans Terminal 2 :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Le client envoie une requête `OPCODE_TERMINATE` au démon.
- Le démon reçoit la requête et se termine proprement.
- Le code de retour est `0` (succès).

**Vérification dans Terminal 2 :**
```bash
ps aux | grep erraid | grep -v grep
```

**Résultat attendu dans Terminal 2 :**
```
(vide, aucun processus)
```

**Explication Terminal 2 :**
- Le démon s'est arrêté proprement.
- Aucun processus `erraid` n'est visible.

**Méthode 2 : Utiliser `kill` avec le PID**

**Commande dans Terminal 2 (ou Terminal 1) :**
```bash
# Trouver le PID du démon
ps aux | grep erraid | grep -v grep
```

**Résultat attendu :**
```
ajinou     92682  0.0  0.0  76696  1804 pts/2    Sl   10:47   0:00 ./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes
```

**Commande pour arrêter :**
```bash
kill 92682
```
(remplacer `92682` par le PID réel de votre processus)

**Résultat attendu :**
```
(vide, aucune sortie)
```

**Explication :**
- `kill <PID>` envoie le signal `SIGTERM` au processus.
- Le démon intercepte le signal et se termine proprement.
- Le processus se termine.

**Vérification :**
```bash
ps aux | grep erraid | grep -v grep
```

**Résultat attendu :**
```
(vide, aucun processus)
```

**Méthode 3 : Utiliser `pkill` (arrête tous les processus erraid)**

**Commande dans Terminal 2 (ou Terminal 1) :**
```bash
pkill -f "erraid"
```

**Résultat attendu :**
```
(vide, aucune sortie)
```

**Explication :**
- `pkill -f "erraid"` trouve tous les processus dont la ligne de commande contient "erraid" et leur envoie `SIGTERM`.
- Utile si vous avez plusieurs processus erraid qui tournent.
- **Attention :** Cela arrête TOUS les processus erraid, pas seulement celui que vous voulez.

**Vérification :**
```bash
ps aux | grep erraid | grep -v grep
```

**Résultat attendu :**
```
(vide, aucun processus)
```

**Méthode 4 : Utiliser `fg` puis Ctrl+C (ramener au premier plan)**

**Commande dans Terminal 1 :**
```bash
# Ramener le processus en arrière-plan au premier plan
fg
```

**Résultat attendu dans Terminal 1 :**
```
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes
```

**Action dans Terminal 1 :** Appuyer sur **Ctrl+C**

**Résultat attendu dans Terminal 1 :**
```
^C
```

**Explication Terminal 1 :**
- `fg` ramène le processus en arrière-plan au premier plan.
- Une fois au premier plan, Ctrl+C fonctionne normalement.
- Le démon se termine et le prompt réapparaît.

**État après :** Le démon est arrêté. Les tâches sont conservées sur le disque.

**Recommandation :** Utilisez **Méthode 1** (`tadmor -q`) car c'est l'arrêt le plus propre et le plus sûr.

---

#### Terminal 1 : Option `-h` (Help)

**État initial :** Aucun démon en cours d'exécution.

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./erraid -h
```

**Résultat attendu :**
```
Usage: erraid [OPTIONS]
Options:
  -R RUN_DIRECTORY    Répertoire de stockage (défaut: /tmp/$USER/erraid)
  -P PIPES_DIR        Répertoire des pipes (défaut: RUN_DIRECTORY/pipes)
  -F                  Exécution en avant-plan (sans démonisation)
  -d                  Activer les logs de débogage
  -h, --help          Afficher cette aide
```

**Explication :**
- `-h` ou `--help` affiche l'aide et quitte immédiatement.
- Le démon ne démarre pas, il affiche simplement l'aide.
- Utile pour connaître les options disponibles.

**Code de retour :** `0` (succès)

**État après :** Aucun démon n'est démarré, le programme s'est terminé après avoir affiché l'aide.

#### Terminal 2 : Utilisation du client

**État initial :** Terminal vide, prêt à recevoir des commandes.

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
```

**Résultat attendu :** Changement de répertoire vers le projet.

**Explication :** Toutes les commandes `tadmor` doivent être exécutées depuis ce répertoire pour accéder à l'exécutable.

**État après :** Terminal 2 prêt, démon toujours en cours dans Terminal 1.

---

### Commandes du démon `erraid` (Terminal 1)

#### Arrêt du démon via TERMINATE (depuis Terminal 2)

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu dans Terminal 2 :**
```
(vide, aucune sortie)
```

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Processing client request
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] Handling TERMINATE request
[DEBUG] TERMINATE: g_stop set to 1, sending response
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)
```

**Explication Terminal 1 :**
- `[DEBUG] Processing client request` : Le démon détecte une requête disponible sur le pipe.
- `[DEBUG] handle_request: received opcode=21581 (taskid=0)` : Le démon a reçu la requête avec l'opcode TERMINATE (21581). Le taskid est 0 car TERMINATE n'utilise pas de taskid.
- `[DEBUG] Handling TERMINATE request` : Le démon traite la requête TERMINATE.
- `[DEBUG] TERMINATE: g_stop set to 1, sending response` : Le démon met `g_stop` à 1 pour signaler l'arrêt.
- `[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)` : Le démon envoie la réponse au client.
- `[DEBUG] Response sent successfully, closing reply_fd` : La réponse est envoyée et le pipe de réponse est fermé.
- `[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)` : Le démon quitte la boucle principale. Le nombre d'itérations (5 dans cet exemple) peut varier selon le moment où la requête arrive.

**Explication :**
- Le client envoie une requête `OPCODE_TERMINATE` au démon.
- Le démon reçoit la requête et met `g_stop` à 1 pour signaler l'arrêt.
- Le démon envoie une réponse `OK` au client.
- Le démon ferme les pipes et termine proprement.
- Le processus `erraid` se termine et le prompt réapparaît dans Terminal 1.

**Code de retour :** `0` (succès pour le client et le démon)

**État après :** Le démon est arrêté. Les tâches sont conservées sur le disque.

---

#### Arrêt du démon via signal SIGTERM

**Terminal utilisé :** Terminal 2 (ou Terminal 1)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
# Trouver le PID du démon
ps aux | grep erraid | grep -v grep

# Envoyer SIGTERM
kill <PID>
```

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)
```

**Explication Terminal 1 :**
- `[DEBUG] daemon_loop started, request_fd=3, reply_fd=4` : Le démon démarre et ouvre les pipes.
- `[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)` : Le démon quitte la boucle principale après avoir reçu SIGTERM. Le nombre d'itérations (5 dans cet exemple) peut varier selon le moment où le signal arrive.

**Explication :**
- `kill <PID>` envoie le signal `SIGTERM` au processus.
- Le démon intercepte le signal et met `g_stop` à 1.
- Le démon se termine proprement en fermant les pipes et en arrêtant les threads.
- Le processus se termine et le prompt réapparaît.

**Code de retour :** `0` (arrêt normal)

**État après :** Le démon est arrêté. Les tâches sont conservées sur le disque.

---

#### Arrêt du démon via signal SIGINT (Ctrl+C)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Action dans Terminal 1 :** Appuyer sur **Ctrl+C** dans Terminal 1

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
^C[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)
```

**Explication Terminal 1 :**
- `[DEBUG] daemon_loop started, request_fd=3, reply_fd=4` : Le démon démarre et ouvre les pipes.
- `^C` : Indique que Ctrl+C a été pressé.
- `[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)` : Le démon quitte la boucle principale après avoir reçu SIGINT. Le nombre d'itérations (5 dans cet exemple) peut varier selon le moment où Ctrl+C est pressé.

**Explication Terminal 1 :**
- `Ctrl+C` envoie le signal `SIGINT` au processus.
- Le démon intercepte le signal et met `g_stop` à 1.
- Le démon se termine proprement.
- Le processus se termine et le prompt réapparaît.

**Code de retour :** `0` (arrêt normal)

**État après :** Le démon est arrêté. Les tâches sont conservées sur le disque.

**Note importante :** Les pipes nommés (`erraid-request-pipe` et `erraid-reply-pipe`) **restent sur le système de fichiers** après l'arrêt du démon. C'est normal car les pipes nommés (FIFO) sont des fichiers spéciaux qui persistent jusqu'à ce qu'ils soient explicitement supprimés avec `rm` ou `unlink()`. Le démon ne les supprime pas automatiquement à l'arrêt. Si vous redémarrez le démon, il utilisera les pipes existants (ou les recréera s'ils n'existent pas).

**Vérification dans Terminal 2 :**
```bash
ls -la /tmp/test-erraid/pipes/
```

**Résultat attendu dans Terminal 2 :**
```
total 0
drwxrwxr-x 2 ajinou ajinou 80 ... .
drwxrwxr-x 3 ajinou ajinou 60 ... ..
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-reply-pipe
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-request-pipe
```

**Explication Terminal 2 :** Les pipes sont toujours présents même après l'arrêt du démon. C'est le comportement attendu des pipes nommés (FIFO) sous Linux. Ils ne sont pas supprimés automatiquement car ce sont des fichiers sur le système de fichiers, pas des ressources liées au processus.

---

#### Cas : Répertoire créé récursivement

**État initial :** Aucun démon en cours d'exécution

**Commande dans Terminal 1 :**
```bash
./erraid -R /tmp/inexistant/chemin/impossible -P /tmp/test-erraid/pipes -F
```

**Résultat attendu dans Terminal 1 :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Explication Terminal 1 :**
- Le démon crée récursivement le répertoire `/tmp/inexistant/chemin/impossible` (comme `mkdir -p`).
- Le démon démarre normalement et attend des requêtes.
- **Note :** Le démon ne retourne pas d'erreur car `init_task_directory()` crée récursivement tous les répertoires parents nécessaires.

**Vérification dans Terminal 2 :**
```bash
ls -la /tmp/inexistant/chemin/impossible
```

**Résultat attendu dans Terminal 2 :**
```
total 0
drwxr-xr-x 2 ajinou ajinou 40 ... .
drwxr-xr-x 3 ajinou ajinou 60 ... ..
```

**Explication Terminal 2 :**
- Le répertoire a été créé récursivement par le démon.
- Le répertoire est vide (pas encore de sous-répertoire `tasks/` car aucune tâche n'a été créée).

**Code de retour :** `0` (succès, le démon tourne)

**État après :** Le démon est en cours d'exécution. Le répertoire a été créé récursivement.

---

#### Test avec requête client (démon avec `-d`)

**Terminal 1 : Démon avec debug**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Terminal 2 : Client**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (après la requête) :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] Processing client request
[DEBUG] Processing client request (reply pipe reopened: fd=4)
```

**Résultat attendu dans Terminal 2 :**
```
(Liste des tâches ou liste vide)
```

**Explication :**
- Le démon reçoit la requête `LIST` du client.
- Le démon traite la requête et envoie la réponse.
- Les messages de debug montrent le traitement de la requête.
- Le client reçoit la liste des tâches (ou une liste vide si aucune tâche n'existe).

**État après :** Le démon continue de fonctionner, prêt à recevoir d'autres requêtes.

---

### 1. Création de tâches (`-c`)

#### Cas de base : Tâche simple sans timing

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c echo "Hello World"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=17234 (taskid=0)
[DEBUG] handle_request: processing CREATE request
[DEBUG] handle_request: CREATE created task 0
[DEBUG] Sending response (opcode=17234, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` du client (opcode=17234).
- Le démon crée une nouvelle tâche avec la commande `echo "Hello World"`.
- Le démon génère un identifiant unique (0 pour la première tâche, puis 1, 2, 3, etc.).
- Le démon enregistre la tâche sur le disque dans `/tmp/test-erraid/tasks/0/`.
- Le démon envoie la réponse avec l'identifiant de la tâche au client.
- Sans `-d`, aucun message n'est visible (le démon tourne en silence).

**Résultat attendu dans Terminal 2 (client) :**
```
0
```

**Explication Terminal 2 :**
- Le nombre `0` est l'identifiant unique de la première tâche créée par le démon.
- Les identifiants commencent à 0 et sont incrémentaux (la prochaine tâche sera `1`, puis `2`, etc.).
- Les identifiants supprimés ne sont pas réutilisés.
- Le code de retour est `0` (succès).
- Le client a envoyé la requête `OPCODE_CREATE` au démon.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded 1 tasks
[DEBUG] handle_request: LIST found 1 tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` du client.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste des tâches au client.

**Résultat attendu dans Terminal 2 (client) :**
```
0: * * * echo Hello World
```

**Explication Terminal 2 :**
- `0:` : Identifiant de la tâche suivi de `:`
- `* * *` : Format de timing au style cron :
  - Premier `*` : toutes les minutes (0-59)
  - Deuxième `*` : toutes les heures (0-23)
  - Troisième `*` : tous les jours de la semaine (0-6, dimanche=0)
- `echo Hello World` : La commande à exécuter (non parenthésée car c'est une commande simple)
- Par défaut, si aucun timing n'est spécifié, la tâche s'exécute à chaque minute (`* * *`).
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.
- **Note importante** : L'ordre d'affichage des tâches est quelconque (selon l'énoncé du professeur). Les tâches peuvent apparaître dans n'importe quel ordre (0, 1, 2 ou 2, 1, 0, etc.).

**État après :** La tâche est créée et sera exécutée automatiquement à chaque minute par le démon.

#### Cas avec timing : Tâche à exécuter à des heures précises

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m 0,30 -H 14 echo "Test 14h"
```

**Résultat attendu :**
```
1
```

**Explication du résultat :**
- `1` est l'identifiant de la nouvelle tâche (incrémenté depuis la tâche précédente qui était 0).
- Le code de retour est `0` (succès).

**Explication des options :**
- `-m 0,30` : La tâche s'exécutera aux minutes 0 et 30 de chaque heure.
- `-H 14` : La tâche s'exécutera à l'heure 14 (14h00).
- `-d` non spécifié : Par défaut, tous les jours de la semaine (`*`).

**Vérification :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
0: * * * echo Hello World
1: 0,30 14 * echo Test 14h
```

**Explication du format :**
- `1: 0,30 14 *` signifie :
  - Minutes : `0,30` (aux minutes 0 et 30)
  - Heures : `14` (à 14h00)
  - Jours : `*` (tous les jours)
- La tâche s'exécutera donc à 14h00 et 14h30 chaque jour.

**État après :** Deux tâches sont créées. La tâche 2 s'exécutera à 14h00 et 14h30 chaque jour.

#### Cas avec timing complet : Tous les formats

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" echo "Every minute"
```

**Résultat attendu :**
```
2
```

**Vérification :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
0: * * * echo Hello World
1: 0,30 14 * echo Test 14h
2: * * * echo Every minute
```

**Note importante sur l'ordre :** L'ordre d'affichage des tâches est quelconque selon l'énoncé du professeur. Les tâches peuvent apparaître dans n'importe quel ordre (0, 1, 2 ou 2, 1, 0, etc.). L'ordre dépend de l'ordre dans lequel le système de fichiers retourne les répertoires lors de la lecture.

#### Cas avec plages : Timing par plages

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "0-5" -H "8-12" -d "0-2" echo "Test range"
```

**Résultat attendu :**
```
3
```

**Vérification :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
...
3: 0-5 8-12 0-2 echo Test range
```

#### Cas avec listes complexes : Combinaison de valeurs et plages

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "0-5,10-15,20-25" -H "8-12,18-22" -d "0-2,5-6" echo "Test complex"
```

**Résultat attendu :**
```
4
```

#### Cas avec tâche abstraite (`-n`)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -n echo "Abstract task"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=17234 (taskid=0)
[DEBUG] handle_request: processing CREATE request
[DEBUG] handle_request: CREATE created task 5
[DEBUG] Sending response (opcode=17234, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec l'option `-n` (tâche abstraite).
- Le démon crée une tâche avec le timing `- - -` (aucune valeur, jamais exécutée automatiquement).
- Le démon génère l'identifiant 5 (incrémenté depuis les tâches précédentes).
- Le démon enregistre la tâche sur le disque avec le timing abstrait.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
5
```

**Explication Terminal 2 :**
- `5` est l'identifiant de la nouvelle tâche abstraite.
- Le code de retour est `0` (succès).
- Le client a détecté l'option `-n` et a construit la requête avec le timing abstrait.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication de l'option `-n` :**
- `-n` crée une tâche "abstraite" sans horaire d'exécution.
- Cette tâche ne sera **jamais exécutée automatiquement** par le démon.
- Elle sert uniquement à être combinée avec d'autres tâches pour créer des commandes complexes.
- Le timing est défini comme `- - -` (aucune valeur).

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded X tasks
[DEBUG] handle_request: LIST found X tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` (opcode=19539).
- Le démon lit toutes les tâches depuis le disque (y compris les tâches abstraites).
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
...
5: - - - echo Abstract task
```

**Explication Terminal 2 :**
- `5:` : Identifiant de la tâche
- `- - -` : Format spécial pour les tâches abstraites (aucun timing, donc jamais exécutée automatiquement)
- `echo Abstract task` : La commande (non parenthésée car simple)
- Le client a reçu la liste des tâches du démon.
- Le client formate les tâches abstraites avec `- - -` pour le timing.

**Note importante :** 
- Les tâches abstraites ne sont jamais exécutées automatiquement.
- Elles servent uniquement à être combinées avec d'autres tâches.
- Lors d'une combinaison, les tâches abstraites sont "consommées" (supprimées) et leurs commandes deviennent des sous-commandes de la nouvelle tâche combinée.

**État après :** La tâche abstraite est créée mais ne s'exécutera jamais automatiquement. Elle peut être utilisée dans une combinaison.

---

### 2. Combinaison séquentielle (`-s`)

#### Cas de base : Combinaison de 2 tâches

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 (créer les tâches abstraites à combiner) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "first")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "second")
```

**Résultat attendu dans Terminal 2 :** `TASK1` et `TASK2` contiennent les identifiants (ex: `6` et `7`).

**Explication de la préparation :**
- On crée deux tâches abstraites (`-n`) qui ne s'exécuteront jamais seules.
- Les identifiants sont stockés dans les variables `TASK1` et `TASK2`.
- Ces tâches seront "consommées" lors de la combinaison (supprimées de la liste).

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -s $TASK1 $TASK2
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 2 tasks
[DEBUG] handle_request: COMBINE created task 8
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `SQ` (séquentiel).
- Le démon lit les tâches `$TASK1` et `$TASK2` depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les deux tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche combinée avec un identifiant unique (ex: 8).
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
8
```

**Explication Terminal 2 :**
- `8` est l'identifiant de la nouvelle tâche combinée créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1` et `$TASK2` ont été supprimées (consommées) et n'existent plus.
- Le client envoie une requête `OPCODE_COMBINE` avec le type `SQ` et les taskids.
- Le client reçoit la réponse avec l'identifiant de la nouvelle tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication du comportement :**
- `-s` crée une combinaison séquentielle (type `SQ`).
- Les commandes s'exécutent **séquentiellement**, une après l'autre.
- Le code de retour final est celui de la **dernière commande**.
- Le timing par défaut est `* * *` (toutes les minutes) si aucun timing n'est spécifié.
- Si `-n` est utilisé, le timing est `- - -` (abstraite).

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded X tasks
[DEBUG] handle_request: LIST found X tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` (opcode=19539).
- Le démon lit toutes les tâches depuis le disque.
- Les tâches `$TASK1` et `$TASK2` n'existent plus (consommées).
- La nouvelle tâche combinée existe.

**Résultat attendu dans Terminal 2 (client) :**
```
8: * * * ( echo first ; echo second )
```

**Explication Terminal 2 :**
- `8:` : Identifiant de la tâche combinée
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( echo first ; echo second )` : 
  - Les parenthèses délimitent la combinaison séquentielle (comme dans l'exemple du professeur)
  - Le `;` sépare les commandes séquentielles
  - Les commandes simples ne sont pas parenthésées individuellement
  - Les espaces autour de `;` et des parenthèses sont présents pour la lisibilité
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

**Note importante :** 
- Les tâches combinées (`$TASK1` et `$TASK2`) n'existent plus dans la liste.
- Elles ont été "consommées" : leurs identifiants, timings et traces d'exécution sont perdus.
- Seules leurs arborescences de commandes (`cmd`) sont conservées comme sous-commandes de la nouvelle tâche.

**État après :** La nouvelle tâche combinée existe, les anciennes tâches ont été supprimées.

#### Cas avec timing : Combinaison avec horaire

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Prérequis :** Les tâches `$TASK1` et `$TASK2` doivent exister (créer de nouvelles si nécessaire).

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -s -m "*" -H "*" -d "*" $TASK1 $TASK2
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 2 tasks
[DEBUG] handle_request: COMBINE created task 8
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `SQ` et le timing spécifié.
- Le démon parse les options `-m "*"`, `-H "*"`, `-d "*"` (ces opérations ne génèrent pas de messages de debug).
- Le démon lit les tâches `$TASK1` et `$TASK2` depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les deux tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche combinée avec le timing spécifié.
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Note :** Si les tâches `$TASK1` et `$TASK2` ont déjà été consommées lors d'une combinaison précédente, vous obtiendrez `ERROR: NOT_FOUND` au lieu d'un succès.

**Résultat attendu dans Terminal 2 (client) :**
```
8
```

**Explication Terminal 2 :**
- `8` est l'identifiant de la nouvelle tâche combinée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1` et `$TASK2` ont été supprimées (consommées).
- Le client a parsé les options `-m "*"`, `-H "*"`, `-d "*"`.
- Le client a construit la requête `OPCODE_COMBINE` avec le timing.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
8: * * * ( echo first ; echo second )
```

**Explication Terminal 2 :**
- `8:` : Identifiant de la tâche
- `* * *` : Timing (toutes les minutes, toutes les heures, tous les jours)
- `( echo first ; echo second )` : Format identique à la combinaison sans timing spécifié
- Les parenthèses délimitent la combinaison séquentielle (comme dans l'exemple du professeur)
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche avec son timing correct.

#### Cas avec plusieurs tâches : Combinaison de 3+ tâches

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "cmd1")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "cmd2")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "cmd3")
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "cmd4")
```

**Résultat attendu dans Terminal 2 :** `TASK1`, `TASK2`, `TASK3`, `TASK4` contiennent les identifiants (ex: `23`, `24`, `25`, `26`).

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -s $TASK1 $TASK2 $TASK3 $TASK4
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 4 tasks
[DEBUG] handle_request: COMBINE created task 13
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `SQ` et 4 taskids.
- Le démon lit les 4 tâches depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les 4 tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche combinée avec un identifiant unique (ex: 13, incrémenté depuis les tâches précédentes).
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
9
```

**Explication Terminal 2 :**
- `9` est l'identifiant de la nouvelle tâche combinée créée.
- Le code de retour est `0` (succès).
- Les 4 tâches ont été supprimées (consommées).
- Le client a construit la requête `OPCODE_COMBINE` avec 4 taskids.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Les 4 tâches n'existent plus (consommées).
- La nouvelle tâche combinée existe.

**Résultat attendu dans Terminal 2 (client) :**
```
13: * * * ( echo cmd1 ; echo cmd2 ; echo cmd3 ; echo cmd4 )
```

**Explication Terminal 2 :**
- `13:` : Identifiant de la tâche combinée (incrémenté depuis les tâches précédentes)
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( echo cmd1 ; echo cmd2 ; echo cmd3 ; echo cmd4 )` : 
  - Les parenthèses délimitent la combinaison séquentielle (comme dans l'exemple du professeur)
  - Les commandes sont séparées par `;`
  - L'ordre d'exécution est celui de l'ordre des arguments
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

---

### 3. Combinaison en pipeline (`-p`)

#### Cas de base : Pipeline de 2 tâches

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Préparation (créer les tâches abstraites à combiner) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
```

**Explication de la préparation :**
- `TASK1` : Commande `echo "hello"` qui affiche "hello" sur stdout
- `TASK2` : Commande `tr a-z A-Z` qui transforme les minuscules en majuscules
- Les deux sont abstraites (`-n`) car elles seront combinées

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2
```

**Résultat attendu :**
```
16
```

**Explication du résultat :**
- `16` est l'identifiant de la nouvelle tâche pipeline créée (incrémenté depuis les tâches précédentes).
- Le code de retour est `0` (succès).
- Les tâches `$TASK1` et `$TASK2` ont été supprimées (consommées).
- **Note importante** : L'identifiant dépend des tâches déjà créées. Si vous avez déjà créé d'autres tâches, l'identifiant sera différent.

**Explication du comportement :**
- `-p` crée une combinaison en pipeline (type `PL`).
- La **sortie standard (stdout)** de la première commande est connectée à l'**entrée standard (stdin)** de la deuxième.
- Les commandes s'exécutent en parallèle (pas séquentiellement).
- Le code de retour final est celui de la **dernière commande**.
- Dans cet exemple : `echo "hello"` produit "hello" qui est passé à `tr a-z A-Z` qui produit "HELLO".

**Vérification :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
16: * * * ( echo hello | tr a-z A-Z )
```

**Explication du format :**
- `16:` : Identifiant de la tâche pipeline (incrémenté depuis les tâches précédentes)
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( echo hello | tr a-z A-Z )` :
  - Les parenthèses délimitent la combinaison pipeline (comme dans l'exemple du professeur)
  - Le `|` sépare les commandes du pipeline
  - Les commandes simples ne sont pas parenthésées individuellement
  - Les espaces autour de `|` sont présents pour la lisibilité

**État après :** La nouvelle tâche pipeline existe, les anciennes tâches ont été supprimées.

#### Cas avec plusieurs commandes : Pipeline de 3+ tâches

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n wc -c)
```

**Résultat attendu dans Terminal 2 :** `TASK1`, `TASK2`, `TASK3` contiennent les identifiants (incrémentés depuis les tâches précédentes).

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2 $TASK3
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 3 tasks
[DEBUG] handle_request: COMBINE created task 20
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `PL` et 3 taskids.
- Le démon lit les 3 tâches depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les 3 tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche combinée avec un identifiant unique (ex: 20, incrémenté depuis les tâches précédentes).
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
20
```

**Explication Terminal 2 :**
- `20` est l'identifiant de la nouvelle tâche combinée créée (incrémenté depuis les tâches précédentes).
- Le code de retour est `0` (succès).
- Les 3 tâches ont été supprimées (consommées).
- Le client a construit la requête `OPCODE_COMBINE` avec le type `PL` et 3 taskids.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded X tasks
[DEBUG] handle_request: LIST found X tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` (opcode=19539).
- Le démon lit toutes les tâches depuis le disque.
- Les 3 tâches n'existent plus (consommées).
- La nouvelle tâche pipeline existe.

**Résultat attendu dans Terminal 2 (client) :**
```
20: * * * ( echo hello | tr a-z A-Z | wc -c )
```

**Explication Terminal 2 :**
- `20:` : Identifiant de la tâche pipeline (incrémenté depuis les tâches précédentes)
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( echo hello | tr a-z A-Z | wc -c )` :
  - Les parenthèses délimitent la combinaison pipeline (comme dans l'exemple du professeur)
  - Les commandes sont séparées par `|`
  - L'ordre d'exécution est celui de l'ordre des arguments
  - La sortie de chaque commande devient l'entrée de la suivante
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

#### Cas avec timing : Pipeline avec horaire

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**⚠️ IMPORTANT :** Les tâches `$TASK1` et `$TASK2` utilisées précédemment ont été **consommées** lors de la combinaison précédente. Il faut créer de **nouvelles tâches abstraites** pour ce test.

**Préparation dans Terminal 2 (créer de nouvelles tâches abstraites) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
```

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p -m "*" -H "*" -d "*" $TASK1 $TASK2
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 2 tasks
[DEBUG] handle_request: COMBINE created task 21
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `PL` et le timing spécifié.
- Le démon parse les options `-m "*"`, `-H "*"`, `-d "*"` (ces opérations ne génèrent pas de messages de debug).
- Le démon lit les tâches `$TASK1` et `$TASK2` depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les deux tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche pipeline avec le timing spécifié.
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
21
```

**Explication Terminal 2 :**
- `21` est l'identifiant de la nouvelle tâche pipeline créée (incrémenté depuis les tâches précédentes).
- Le code de retour est `0` (succès).
- Les tâches `$TASK1` et `$TASK2` ont été supprimées (consommées).
- Le client a parsé les options `-m "*"`, `-H "*"`, `-d "*"`.
- Le client a construit la requête `OPCODE_COMBINE` avec le timing.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**⚠️ Note importante :** Si vous essayez d'utiliser des tâches qui ont déjà été consommées, vous obtiendrez `ERROR: NOT_FOUND`. Il faut toujours créer de nouvelles tâches abstraites pour chaque nouvelle combinaison.

**Explication des options :**
- `-m "*"` : Toutes les minutes (0-59).
- `-H "*"` : Toutes les heures (0-23).
- `-d "*"` : Tous les jours de la semaine (0-6).
- Contrairement aux tâches abstraites, cette tâche **sera exécutée automatiquement** par le démon à chaque minute.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
21: * * * ( echo hello | tr a-z A-Z )
```

**Explication Terminal 2 :**
- `21:` : Identifiant de la tâche (incrémenté depuis les tâches précédentes)
- `* * *` : Timing (toutes les minutes, toutes les heures, tous les jours)
- `( echo hello | tr a-z A-Z )` : Le pipeline (parenthésé, séparé par `|`)
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche avec son timing correct.

**État après :** Le pipeline avec timing est créé et sera exécuté automatiquement à chaque minute.

#### ⚠️ Erreur : Tâches déjà consommées

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Situation :** Vous essayez de combiner des tâches qui ont déjà été consommées lors d'une combinaison précédente.

**Commande dans Terminal 2 :**
```bash
# Les variables $TASK1 et $TASK2 contiennent des identifiants de tâches déjà consommées
./tadmor -P /tmp/test-erraid/pipes -p -m "*" -H "*" -d "*" $TASK1 $TASK2
```

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication :**
- Le code de retour est `1` (erreur).
- Le démon ne trouve pas les tâches `$TASK1` et `$TASK2` car elles ont été supprimées (consommées) lors d'une combinaison précédente.
- Les tâches abstraites sont **consommées** lors d'une combinaison : elles sont supprimées du disque et leurs identifiants ne peuvent plus être utilisés.

**Solution :**
- Créer de **nouvelles tâches abstraites** avant chaque nouvelle combinaison :
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
./tadmor -P /tmp/test-erraid/pipes -p -m "*" -H "*" -d "*" $TASK1 $TASK2
```

**État après :** Aucune nouvelle tâche n'est créée. Les tâches demandées n'existent plus.

---

### 4. Combinaison conditionnelle (`-i`)

#### Cas avec 2 tâches : Condition + then (pas de else)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 (créer les tâches abstraites à combiner) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n test -f /tmp/nonexistent)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "file exists")
```

**Résultat attendu dans Terminal 2 :** `TASK1` et `TASK2` contiennent les identifiants (ex: `30` et `31`).

**Explication de la préparation :**
- `TASK1` : Commande `test -f /tmp/nonexistent` qui vérifie si le fichier existe (retourne 0 si oui, 1 si non)
- `TASK2` : Commande `echo "file exists"` qui affiche "file exists"
- Les deux sont abstraites (`-n`) car elles seront combinées

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK2
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] handle_request: processing COMBINE request for 2 tasks
[DEBUG] handle_request: COMBINE created task 2
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` (opcode=21581) avec le type `IF` (conditionnel).
- Le démon lit les tâches `$TASK1` et `$TASK2` depuis le disque (ces opérations ne génèrent pas de messages de debug).
- Le démon supprime les deux tâches (consommées) (ces opérations ne génèrent pas de messages de debug).
- Le démon crée une nouvelle tâche conditionnelle avec un identifiant unique (ex: 2, incrémenté depuis les tâches précédentes).
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
2
```

**Explication Terminal 2 :**
- `2` est l'identifiant de la nouvelle tâche conditionnelle créée (incrémenté depuis les tâches précédentes).
- Le code de retour est `0` (succès).
- Les tâches `$TASK1` et `$TASK2` ont été supprimées (consommées).
- Le client envoie une requête `OPCODE_COMBINE` avec le type `IF` et les taskids.
- Le client reçoit la réponse avec l'identifiant de la nouvelle tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication du comportement :**
- `-i` crée une combinaison conditionnelle (type `IF`).
- **Minimum 2 tâches** : condition (première) + then (deuxième)
- **Maximum 3 tâches** : condition + then + else (troisième)
- Comportement :
  1. Exécute la **condition** (première tâche)
  2. Si le code de retour de la condition est **0** (succès) : exécute **then** (deuxième tâche)
  3. Sinon : exécute **else** (troisième tâche) si présente, sinon retourne le code de retour de la condition
- Le code de retour final est celui de la **branche exécutée** (then ou else).

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Les tâches `$TASK1` et `$TASK2` n'existent plus (consommées).
- La nouvelle tâche conditionnelle existe.

**Résultat attendu dans Terminal 2 (client) :**
```
2: * * * ( if test -f /tmp/nonexistent ; then echo file exists ; fi )
```

**Explication Terminal 2 :**
- `2:` : Identifiant de la tâche conditionnelle (incrémenté depuis les tâches précédentes)
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( if test -f /tmp/nonexistent ; then echo file exists ; fi )` :
  - Les parenthèses indiquent que c'est une combinaison
  - `if` : mot-clé de début de condition
  - `test -f /tmp/nonexistent` : la condition (commande simple, non parenthésée)
  - `;` : **impératif** entre une sous-commande **simple** et le mot-clé suivant (`then`)
  - `then` : mot-clé pour la branche de succès
  - `echo file exists` : la commande then (commande simple, non parenthésée)
  - `;` : **impératif** entre une sous-commande **simple** et le mot-clé suivant (`fi`)
  - `fi` : mot-clé de fin de condition
  - Pas de `else` car seulement 2 tâches ont été fournies
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

**État après :** La nouvelle tâche conditionnelle existe, les anciennes tâches ont été supprimées.

#### Cas avec 3 tâches : Condition + then + else

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 (créer les 3 tâches abstraites à combiner) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n test -f /tmp/nonexistent)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "file exists")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "file not found")
```

**Résultat attendu dans Terminal 2 :** `TASK1`, `TASK2`, `TASK3` contiennent les identifiants (ex: `32`, `33`, `34`).

**Explication de la préparation :**
- `TASK1` : Condition `test -f /tmp/nonexistent` (retourne 1 car le fichier n'existe pas)
- `TASK2` : Branche then `echo "file exists"` (exécutée si condition réussit)
- `TASK3` : Branche else `echo "file not found"` (exécutée si condition échoue)
- Toutes sont abstraites (`-n`) car elles seront combinées

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK2 $TASK3
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request
[DEBUG] Combining 3 tasks (type=IF)
[DEBUG] Task 32: Reading task...
[DEBUG] Task 33: Reading task...
[DEBUG] Task 34: Reading task...
[DEBUG] Task 32: Removing (consumed by combine)...
[DEBUG] Task 33: Removing (consumed by combine)...
[DEBUG] Task 34: Removing (consumed by combine)...
[DEBUG] Creating combined task with ID: 14
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` avec le type `IF` et 3 taskids.
- Le démon lit les 3 tâches depuis le disque.
- Le démon supprime les 3 tâches (consommées).
- Le démon crée une nouvelle tâche conditionnelle avec un identifiant unique (ex: 14).
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
14
```

**Explication Terminal 2 :**
- `14` est l'identifiant de la nouvelle tâche conditionnelle créée.
- Le code de retour est `0` (succès).
- Les 3 tâches `$TASK1`, `$TASK2` et `$TASK3` ont été supprimées (consommées).
- Le client envoie une requête `OPCODE_COMBINE` avec le type `IF` et 3 taskids.
- Le client reçoit la réponse avec l'identifiant de la nouvelle tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication du comportement :**
- Avec 3 tâches, on a une condition complète avec else.
- Comportement :
  1. Exécute `test -f /tmp/nonexistent` (retourne 1 car le fichier n'existe pas)
  2. Comme le code de retour est non-zéro, exécute la branche **else** : `echo "file not found"`
  3. Le code de retour final est celui de la branche else (0 dans ce cas)

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Les 3 tâches n'existent plus (consommées).
- La nouvelle tâche conditionnelle existe.

**Résultat attendu dans Terminal 2 (client) :**
```
X: * * * ( if test -f /tmp/nonexistent ; then echo file exists ; else echo file not found ; fi )
```

**Explication Terminal 2 :**
- `X:` : Identifiant de la tâche conditionnelle (incrémenté depuis les tâches précédentes)
- `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
- `( if test -f /tmp/nonexistent ; then echo file exists ; else echo file not found ; fi )` :
  - `if` : début de condition
  - `test -f /tmp/nonexistent` : condition (simple, non parenthésée)
  - `;` : **impératif** entre commande simple et `then`
  - `then` : branche de succès
  - `echo file exists` : commande then (simple, non parenthésée)
  - `;` : **impératif** entre commande simple et `else`
  - `else` : branche d'échec
  - `echo file not found` : commande else (simple, non parenthésée)
  - `;` : **impératif** entre commande simple et `fi`
  - `fi` : fin de condition
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

**État après :** La nouvelle tâche conditionnelle complète existe, les 3 anciennes tâches ont été supprimées.

#### Cas avec timing : Conditionnelle avec horaire

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Prérequis :** Les tâches `$TASK1`, `$TASK2`, `$TASK3` doivent exister (créer de nouvelles si nécessaire).

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i -m "0" -H "*" -d "*" $TASK1 $TASK2 $TASK3
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request
[DEBUG] Combining 3 tasks (type=IF, timing=0 * *)
[DEBUG] Task X: Reading task...
[DEBUG] Task Y: Reading task...
[DEBUG] Task Z: Reading task...
[DEBUG] Task X: Removing (consumed by combine)...
[DEBUG] Task Y: Removing (consumed by combine)...
[DEBUG] Task Z: Removing (consumed by combine)...
[DEBUG] Creating combined task with ID: 15
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` avec le type `IF` et le timing spécifié.
- Le démon parse les options `-m "0"`, `-H "*"`, `-d "*"`.
- Le démon lit les 3 tâches depuis le disque.
- Le démon supprime les 3 tâches (consommées).
- Le démon crée une nouvelle tâche conditionnelle avec le timing spécifié.
- Le démon enregistre la nouvelle tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
15
```

**Explication Terminal 2 :**
- `15` est l'identifiant de la nouvelle tâche conditionnelle créée.
- Le code de retour est `0` (succès).
- Les 3 tâches `$TASK1`, `$TASK2` et `$TASK3` ont été supprimées (consommées).
- Le client a parsé les options `-m "0"`, `-H "*"`, `-d "*"`.
- Le client a construit la requête `OPCODE_COMBINE` avec le timing.
- Le client a reçu la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication des options :**
- `-m "0"` : La tâche s'exécutera à la minute 0 de chaque heure.
- `-H "*"` : Toutes les heures (0-23).
- `-d "*"` : Tous les jours de la semaine (0-6).
- Contrairement aux tâches abstraites, cette tâche **sera exécutée automatiquement** par le démon.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
15: 0 * * ( if test -f /tmp/nonexistent ; then echo file exists ; else echo file not found ; fi )
```

**Explication Terminal 2 :**
- `15:` : Identifiant de la tâche
- `0 * *` : Timing (minute 0, toutes les heures, tous les jours)
- Le reste du format est identique à la conditionnelle abstraite.
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche avec son timing correct.

**État après :** La conditionnelle avec timing est créée et sera exécutée automatiquement à chaque heure (minute 0).

---

### 5. Consultation des tâches (`-l`)

#### Cas de base : Liste vide

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] No tasks found, sending empty list
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` du client.
- Le démon lit toutes les tâches depuis le disque (`/tmp/test-erraid/tasks/`).
- Aucune tâche n'est trouvée (répertoire vide ou aucune tâche créée).
- Le démon construit une réponse avec une liste vide.
- Le démon envoie la réponse au client.
- Sans `-d`, aucun message n'est visible (le démon tourne en silence).

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune sortie car il n'y a aucune tâche définie.
- Le code de retour est `0` (succès, la liste est simplement vide).
- C'est le cas au démarrage du démon avec un répertoire vide.
- Le client envoie une requête `OPCODE_LIST` au démon.
- Le client reçoit la réponse avec une liste vide.
- Le client n'affiche rien car la liste est vide (pas d'erreur, juste une liste vide).

#### Cas avec tâches : Liste avec plusieurs tâches

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Prérequis :** Plusieurs tâches ont été créées précédemment.

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Reading tasks from disk...
[DEBUG] Found 7 task(s)
[DEBUG] Sending response with 7 task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` du client.
- Le démon lit toutes les tâches depuis le disque (`/tmp/test-erraid/tasks/`).
- Le démon trouve plusieurs tâches (ex: 7 tâches dans cet exemple).
- Le démon désérialise chaque tâche (timing, commande, type).
- Le démon construit la réponse avec toutes les tâches.
- Le démon envoie la réponse au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
1: * * * echo Hello World
2: 0,30 14 * echo Test 14h
3: * * * echo Every minute
5: - - - echo Abstract task
7: - - - ( echo first ; echo second )
16: * * * ( echo hello | tr a-z A-Z )
X: * * * ( if test -f /tmp/nonexistent ; then echo file exists ; else echo file not found ; fi )
```

**Explication du format :**
- **Format général :** `TASKID: MINUTES HOURS DAYSOFWEEK COMMAND`
- **Tâche simple :** `1: * * * echo Hello World`
  - `1:` : Identifiant
  - `* * *` : Timing (toutes les minutes, toutes les heures, tous les jours)
  - `echo Hello World` : Commande simple (non parenthésée)
- **Tâche avec timing précis :** `2: 0,30 14 * echo Test 14h`
  - `0,30` : Minutes 0 et 30
  - `14` : Heure 14 (14h00)
  - `*` : Tous les jours
- **Tâche abstraite :** `5: - - - echo Abstract task`
  - `- - -` : Format spécial pour les tâches abstraites (jamais exécutée automatiquement)
- **Séquence :** `7: - - - ( echo first ; echo second )`
  - Parenthèses autour de la combinaison
  - `;` sépare les commandes séquentielles
  - Commandes simples non parenthésées individuellement
- **Pipeline :** `16: * * * ( echo hello | tr a-z A-Z )`
  - `* * *` : Timing par défaut (toutes les minutes, toutes les heures, tous les jours)
  - Parenthèses autour de la combinaison (comme dans l'exemple du professeur)
  - `|` sépare les commandes du pipeline
  - Commandes simples non parenthésées individuellement
- **Conditionnelle :** `X: * * * ( if test -f /tmp/nonexistent ; then echo file exists ; else echo file not found ; fi )`
  - Parenthèses autour de la combinaison
  - Mots-clés `if`, `then`, `else`, `fi` délimitent les branches
  - `;` impératif entre commandes simples et mots-clés
  - Commandes simples non parenthésées individuellement

**Explication du comportement :**
- L'ordre d'affichage des tâches est **quelconque** (non garanti).
- Toutes les tâches définies sont affichées, qu'elles soient abstraites ou non.
- Le code de retour est `0` (succès).

**État après :** Aucun changement, la liste est simplement consultée.

---

### 6. Historique d'exécution (`-x`)

#### Cas : Tâche jamais exécutée

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 6
```
(où 6 est une tâche abstraite qui n'a jamais été exécutée)

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task 6
[DEBUG] Task 6: Checking if task exists...
[DEBUG] Task 6: Task exists
[DEBUG] Task 6: Checking times-exitcodes file...
[DEBUG] Task 6: NOT_RUN (no times-exitcodes file)
[DEBUG] Sending error response (ERRCODE=NOT_RUN)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid 6.
- Le démon vérifie que la tâche 6 existe (sinon il retournerait `NOT_FOUND`).
- Le démon vérifie que le fichier `times-exitcodes` existe dans `/tmp/test-erraid/tasks/6/`.
- Le fichier n'existe pas car la tâche n'a jamais été exécutée (tâche abstraite).
- Le démon retourne l'erreur `NOT_RUN` (0x4e52 = 'NR') au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_RUN
```

**Explication Terminal 2 :**
- `ERROR: NOT_RUN` : Code d'erreur du protocole (0x4e52 = 'NR') indiquant que la tâche n'a jamais été exécutée.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Cette erreur apparaît pour les tâches abstraites ou les tâches qui n'ont pas encore été exécutées par le démon.
- Le client envoie une requête `OPCODE_TIMES_EXITCODES` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_RUN`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

#### Cas : Tâche exécutée une fois

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation :** 
- Créer une tâche avec timing (ex: `* * *` pour toutes les minutes)
- **Attendre au moins 1 minute** pour que le démon exécute la tâche
- Le démon exécute automatiquement les tâches selon leur timing (à la seconde 0 de chaque minute)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task 1
[DEBUG] Task 1: Checking if task exists...
[DEBUG] Task 1: Task exists
[DEBUG] Task 1: Reading times-exitcodes file...
[DEBUG] Task 1: Found 1 execution(s)
[DEBUG] Task 1: Deserializing execution records...
[DEBUG] Sending response with 1 execution record(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid 1.
- Le démon vérifie que la tâche 1 existe.
- Le démon lit le fichier `times-exitcodes` depuis `/tmp/test-erraid/tasks/1/times-exitcodes`.
- Le démon désérialise les timestamps et codes de retour (format binaire big-endian).
- Le démon trouve 1 enregistrement d'exécution.
- Le démon construit la réponse avec les données d'exécution.
- Le démon envoie la réponse au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
2025-12-14 19:10:00 0
```

**Explication Terminal 2 :**
- `2025-12-14 19:10:00` : Date et heure de l'exécution au format `YYYY-MM-DD HH:MM:SS`
- `0` : Code de retour de la tâche (0 = succès)
- La date et l'heure varient selon le moment réel de l'exécution par le démon
- Le code de retour est `0` (succès de la requête)
- Le client envoie une requête `OPCODE_TIMES_EXITCODES` au démon.
- Le client reçoit la réponse avec les enregistrements d'exécution.
- Le client formate chaque enregistrement selon le format `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le client affiche chaque ligne sur `stdout`.

**Format :**
- `YYYY-MM-DD HH:MM:SS EXITCODE`
- Une ligne par exécution
- Les exécutions sont triées par ordre chronologique (plus ancienne en premier)

#### Cas : Tâche exécutée plusieurs fois

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation :**
- Créer une tâche avec timing (ex: `* * *` pour toutes les minutes)
- **Attendre plusieurs minutes** pour que le démon exécute la tâche plusieurs fois
- Le démon exécute automatiquement les tâches selon leur timing (à la seconde 0 de chaque minute)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 3
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task 3
[DEBUG] Task 3: Checking if task exists...
[DEBUG] Task 3: Task exists
[DEBUG] Task 3: Reading times-exitcodes file...
[DEBUG] Task 3: Found 3 execution(s)
[DEBUG] Task 3: Deserializing execution records...
[DEBUG] Sending response with 3 execution record(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid 3.
- Le démon vérifie que la tâche 3 existe.
- Le démon lit le fichier `times-exitcodes` depuis `/tmp/test-erraid/tasks/3/times-exitcodes`.
- Le démon désérialise tous les timestamps et codes de retour.
- Le démon trouve plusieurs enregistrements d'exécution (ex: 3 dans cet exemple).
- Le démon construit la réponse avec tous les enregistrements.
- Le démon envoie la réponse au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
2025-12-14 19:10:00 0
2025-12-14 19:11:00 0
2025-12-14 19:12:00 0
...
```

**Explication Terminal 2 :**
- Chaque ligne représente une exécution de la tâche.
- Les exécutions sont triées par ordre chronologique (plus ancienne en premier).
- Le format est `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le code de retour est `0` (succès de la requête).
- Le client envoie une requête `OPCODE_TIMES_EXITCODES` au démon.
- Le client reçoit la réponse avec tous les enregistrements d'exécution.
- Le client formate chaque enregistrement selon le format `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le client affiche chaque ligne sur `stdout`.

**Format :**
- `YYYY-MM-DD HH:MM:SS EXITCODE`
- Une ligne par exécution
- Les exécutions sont triées par ordre chronologique (plus ancienne en premier)
- Le code de retour peut être :
  - `0` à `255` : Code de retour normal de la commande
  - `0xFFFF` (65535) : La tâche a été tuée par un signal ou n'a pas terminé normalement

---

### 7. Sortie standard (`-o`)

#### Cas : Tâche jamais exécutée

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o 6
```
(où 6 est une tâche abstraite qui n'a jamais été exécutée)

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task 6
[DEBUG] Task 6: Checking if task exists...
[DEBUG] Task 6: Task exists
[DEBUG] Task 6: Checking stdout file...
[DEBUG] Task 6: NOT_RUN (no stdout file)
[DEBUG] Sending error response (ERRCODE=NOT_RUN)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid 6.
- Le démon vérifie que la tâche 6 existe (sinon il retournerait `NOT_FOUND`).
- Le démon vérifie que le fichier `stdout` existe dans `/tmp/test-erraid/tasks/6/`.
- Le fichier n'existe pas car la tâche n'a jamais été exécutée (tâche abstraite).
- Le démon retourne l'erreur `NOT_RUN` (0x4e52 = 'NR') au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_RUN
```

**Explication Terminal 2 :**
- `ERROR: NOT_RUN` : Code d'erreur du protocole (0x4e52 = 'NR') indiquant que la tâche n'a jamais été exécutée.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le démon ne peut pas fournir la sortie standard d'une tâche qui n'a jamais été exécutée.
- Le client envoie une requête `OPCODE_STDOUT` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_RUN`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

#### Cas : Tâche exécutée avec sortie

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation :** 
- Créer une tâche avec timing (ex: `* * * echo "Hello World"`)
- **Attendre au moins 1 minute** pour que le démon exécute la tâche
- Le démon capture automatiquement la sortie standard de chaque exécution

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o 1
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task 1
[DEBUG] Task 1: Checking if task exists...
[DEBUG] Task 1: Task exists
[DEBUG] Task 1: Reading stdout file...
[DEBUG] Task 1: Reading stdout file (size=12 bytes)
[DEBUG] Sending response with stdout content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid 1.
- Le démon vérifie que la tâche 1 existe (sinon il retournerait `NOT_FOUND`).
- Le démon vérifie que le fichier `stdout` existe (sinon il retournerait `NOT_RUN`).
- Le démon lit le contenu du fichier `stdout` depuis `/tmp/test-erraid/tasks/1/stdout`.
- Le démon construit la réponse avec le contenu du fichier.
- Le démon envoie la réponse au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
Hello World
```

**Explication Terminal 2 :**
- `Hello World` : Contenu de la sortie standard de la **dernière exécution complète** de la tâche.
- Le démon stocke la sortie standard de chaque exécution dans le fichier `stdout` de la tâche.
- Seule la **dernière exécution** est accessible via `-o`.
- Le code de retour est `0` (succès).
- Si la tâche n'a pas de sortie, le résultat est vide (aucune ligne).
- Le client envoie une requête `OPCODE_STDOUT` au démon.
- Le client reçoit la réponse avec le contenu du fichier `stdout`.
- Le client affiche le contenu directement sur `stdout`.

**Explication du comportement :**
- Le démon capture la sortie standard de chaque exécution.
- Chaque nouvelle exécution **écrase** la sortie précédente.
- Seule la dernière exécution complète est conservée.

#### Cas : Tâche avec sortie multi-lignes

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'line1'; echo 'line2'; echo 'line3'")
```

**Résultat attendu dans Terminal 2 :** `TASKID` contient l'identifiant (ex: `X`).

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task X
[DEBUG] Task X: Reading stdout file (size=18 bytes)
[DEBUG] Sending response with stdout content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid X.
- Le démon lit le fichier `stdout` qui contient plusieurs lignes.
- Le démon envoie tout le contenu au client (y compris les retours à la ligne).

**Résultat attendu dans Terminal 2 (client) :**
```
line1
line2
line3
```

**Explication Terminal 2 :**
- Chaque ligne est préservée avec son saut de ligne.
- L'ordre des lignes est conservé.
- Le démon capture toute la sortie standard, y compris les retours à la ligne.
- Le client affiche le contenu complet sur `stdout`.

#### Cas : Tâche sans sortie

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'test' > /dev/null")
```

**Résultat attendu dans Terminal 2 :** `TASKID` contient l'identifiant (ex: `X`).

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task X
[DEBUG] Task X: Reading stdout file (size=0 bytes)
[DEBUG] Sending response with empty stdout content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid X.
- Le démon lit le fichier `stdout` qui est vide (0 bytes).
- Le démon envoie une réponse vide au client.

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune sortie car la commande n'a rien écrit sur stdout (redirection vers `/dev/null`).
- Le fichier `stdout` de la tâche est vide.
- Le code de retour est `0` (succès, la tâche a bien été exécutée, elle n'a juste rien produit sur stdout).
- Le client reçoit une réponse vide et n'affiche rien.

---

### 8. Sortie d'erreur (`-e`)

#### Cas : Tâche jamais exécutée

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e 6
```
(où 6 est une tâche abstraite qui n'a jamais été exécutée)

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task 6
[DEBUG] Task 6: Checking if task exists...
[DEBUG] Task 6: Task exists
[DEBUG] Task 6: Checking stderr file...
[DEBUG] Task 6: NOT_RUN (no stderr file)
[DEBUG] Sending error response (ERRCODE=NOT_RUN)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid 6.
- Le démon vérifie que la tâche 6 existe (sinon il retournerait `NOT_FOUND`).
- Le démon vérifie que le fichier `stderr` existe dans `/tmp/test-erraid/tasks/6/`.
- Le fichier n'existe pas car la tâche n'a jamais été exécutée (tâche abstraite).
- Le démon retourne l'erreur `NOT_RUN` (0x4e52 = 'NR') au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_RUN
```

**Explication Terminal 2 :**
- `ERROR: NOT_RUN` : Code d'erreur du protocole (0x4e52 = 'NR') indiquant que la tâche n'a jamais été exécutée.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le client envoie une requête `OPCODE_STDERR` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_RUN`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

#### Cas : Tâche avec erreur

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'error' >&2")
```

**Résultat attendu dans Terminal 2 :** `TASKID` contient l'identifiant (ex: `X`).

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task X
[DEBUG] Task X: Reading stderr file (size=6 bytes)
[DEBUG] Sending response with stderr content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid X.
- Le démon lit le fichier `stderr` depuis `/tmp/test-erraid/tasks/X/stderr`.
- Le démon envoie le contenu au client.

**Résultat attendu dans Terminal 2 (client) :**
```
error
```

**Explication Terminal 2 :**
- `error` : Contenu de la sortie d'erreur de la **dernière exécution complète** de la tâche.
- Le démon capture séparément stdout et stderr.
- Seule la **dernière exécution** est accessible via `-e`.
- Le code de retour est `0` (succès de la requête).
- Le client affiche le contenu directement sur `stdout`.

#### Cas : Tâche sans erreur

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" echo "test")
```

**Résultat attendu dans Terminal 2 :** `TASKID` contient l'identifiant (ex: `X`).

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task X
[DEBUG] Task X: Reading stderr file (size=0 bytes)
[DEBUG] Sending response with empty stderr content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid X.
- Le démon lit le fichier `stderr` qui est vide (0 bytes).
- Le démon envoie une réponse vide au client.

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune sortie car la commande n'a rien écrit sur stderr.
- Le fichier `stderr` de la tâche est vide.
- Le code de retour est `0` (succès, la tâche a bien été exécutée, elle n'a juste rien produit sur stderr).
- Le client reçoit une réponse vide et n'affiche rien.

#### Cas : Tâche avec stdout et stderr

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'stdout'; echo 'stderr' >&2")
```

**Résultat attendu dans Terminal 2 :** `TASKID` contient l'identifiant (ex: `X`).

**Action : Attendre l'exécution (1 minute minimum)**

**Commande pour stdout dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task X
[DEBUG] Task X: Reading stdout file (size=7 bytes)
[DEBUG] Sending response with stdout content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid X.
- Le démon lit le fichier `stdout` (contient "stdout").
- Le démon envoie le contenu au client.

**Résultat attendu dans Terminal 2 (client) :**
```
stdout
```

**Explication Terminal 2 :**
- Seule la sortie standard est affichée. Le démon capture séparément stdout et stderr.
- Le client affiche le contenu sur `stdout`.

**Commande pour stderr dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task X
[DEBUG] Task X: Reading stderr file (size=7 bytes)
[DEBUG] Sending response with stderr content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid X.
- Le démon lit le fichier `stderr` (contient "stderr").
- Le démon envoie le contenu au client.

**Résultat attendu dans Terminal 2 (client) :**
```
stderr
```

**Explication Terminal 2 :**
- Seule la sortie d'erreur est affichée. Le démon stocke stdout et stderr dans des fichiers séparés.
- Le client affiche le contenu sur `stdout`.

---

### 9. Suppression de tâche (`-r`)

#### Cas de base : Suppression réussie

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Prérequis :** Une tâche avec l'identifiant 1 existe.

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -r 1
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling REMOVE request for task 1
[DEBUG] Task 1: Checking if task exists...
[DEBUG] Task 1: Task exists
[DEBUG] Task 1: Removing task directory...
[DEBUG] Task 1: Task removed successfully
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_REMOVE` avec le taskid 1.
- Le démon vérifie que la tâche 1 existe (sinon il retournerait `NOT_FOUND`).
- Le démon supprime le répertoire complet `/tmp/test-erraid/tasks/1/` et tout son contenu.
- Toutes les données associées sont supprimées (timing, historique, sorties, commande).
- Le démon envoie une réponse `OK` au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune sortie car la suppression est un succès silencieux.
- Le code de retour est `0` (succès).
- La tâche et toutes ses données associées (timing, historique, sorties) sont supprimées du disque.
- Le client envoie une requête `OPCODE_REMOVE` au démon.
- Le client reçoit la réponse `OK`.
- Le client n'affiche rien (succès silencieux).

**Explication du comportement :**
- Le démon supprime la tâche et son répertoire complet dans `/tmp/test-erraid/tasks/1/`.
- Toutes les données associées sont perdues (historique, sorties, etc.).
- L'identifiant supprimé n'est pas réutilisé.

**Code de retour :** `0`

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s) (task 1 removed)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- La tâche 1 n'existe plus (répertoire supprimé).
- Le démon envoie la liste sans la tâche 1.

**Résultat attendu dans Terminal 2 (client) :** La tâche 1 n'apparaît plus dans la liste.

**Explication Terminal 2 :**
- La liste ne contient plus la tâche supprimée, confirmant que la suppression a réussi.
- Le client affiche la liste mise à jour.

#### Cas : Suppression d'une tâche inexistante

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -r 999
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling REMOVE request for task 999
[DEBUG] Task 999: Checking if task exists...
[DEBUG] Task 999: NOT_FOUND (task directory does not exist)
[DEBUG] Sending error response (ERRCODE=NOT_FOUND)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_REMOVE` avec le taskid 999.
- Le démon vérifie que la tâche 999 existe en cherchant le répertoire `/tmp/test-erraid/tasks/999/`.
- Le répertoire n'existe pas, donc la tâche n'existe pas.
- Le démon retourne l'erreur `NOT_FOUND` (0x4e46 = 'NF') au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication Terminal 2 :**
- `ERROR: NOT_FOUND` : Code d'erreur du protocole (0x4e46 = 'NF') indiquant que la tâche avec l'identifiant 999 n'existe pas.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le démon ne peut pas supprimer une tâche qui n'existe pas.
- Le client envoie une requête `OPCODE_REMOVE` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_FOUND`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

---

### 10. Arrêt du démon (`-q`)

#### Cas de base : Arrêt réussi

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] handle_request: received opcode=21581 (taskid=0)
[DEBUG] Handling TERMINATE request
[DEBUG] TERMINATE: g_stop set to 1, sending response
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)
```

**Explication Terminal 1 :**
- `[DEBUG] Processing client request` : Le démon détecte une requête disponible sur le pipe.
- `[DEBUG] handle_request: received opcode=21581 (taskid=0)` : Le démon a reçu la requête avec l'opcode TERMINATE (21581). Le taskid est 0 car TERMINATE n'utilise pas de taskid.
- Le démon reçoit la requête `OPCODE_TERMINATE` du client.
- Le démon met `g_stop` à 1 pour signaler l'arrêt.
- Le démon envoie une réponse `OK` au client.
- Le démon ferme les pipes.
- Le démon libère les ressources mémoire.
- Le démon arrête les threads d'exécution.
- Le processus `erraid` se termine et le prompt réapparaît dans Terminal 1.
- Les tâches sont **conservées sur le disque** dans `/tmp/test-erraid/tasks/` et seront rechargées si le démon est redémarré.

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune sortie car l'arrêt est un succès silencieux.
- Le code de retour est `0` (succès).
- Le client envoie une requête `OPCODE_TERMINATE` au démon.
- Le client reçoit la réponse `OK`.
- Le client se termine immédiatement.

**Code de retour :** `0`

**État après :** Le démon est arrêté. Les tâches sont conservées sur le disque dans `/tmp/test-erraid/tasks/`.

**Vérification dans Terminal 2 :** Les requêtes suivantes échouent car le démon n'est plus actif :
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Le démon n'est plus en cours d'exécution, aucun message)
```

**Explication Terminal 1 :**
- Le démon n'est plus actif, donc aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
Timeout: no response from daemon (is it running?)
```

**Explication Terminal 2 :**
- `Timeout: no response from daemon (is it running?)` : Le client attend 5 secondes une réponse du démon, mais n'en reçoit aucune car le démon est arrêté.
- Le code de retour est `1` (erreur de communication).
- Le client essaie d'ouvrir les pipes et d'envoyer la requête.
- Aucune réponse n'arrive car le démon n'est plus actif.
- Après 5 secondes de timeout, le client affiche le message d'erreur.

**Code de retour :** `1`

---

## Tous les cas possibles

### Format des timings

#### Valeurs acceptées

1. **`*`** : Toutes les valeurs
   - Minutes : `0-59`
   - Heures : `0-23`
   - Jours : `0-6` (dimanche=0, lundi=1, ..., samedi=6)

2. **`-`** : Aucune valeur (pour tâches abstraites)

3. **Valeur unique** : `5`
   - Exemple : `-m 5` signifie "à la 5ème minute"

4. **Plage** : `5-10`
   - Exemple : `-m 5-10` signifie "de la 5ème à la 10ème minute"

5. **Liste** : `0,3,6,9`
   - Exemple : `-m 0,3,6,9` signifie "aux minutes 0, 3, 6 et 9"

6. **Combinaison** : `0-5,10-15,20-25`
   - Exemple : `-m 0-5,10-15,20-25` signifie "de 0 à 5, de 10 à 15, et de 20 à 25"

#### Valeurs rejetées

1. **Valeur hors limites** :
   - Minutes : `60` ou plus
   - Heures : `24` ou plus
   - Jours : `7` ou plus

2. **Valeur négative** : `-1`

3. **Plage invalide** : `5-3` (début > fin)

4. **Format invalide** : `abc`, `5,*`, `*,5`

5. **Plage hors limites** : `0-70` (pour minutes)

### Combinaisons de tâches

#### Séquences (`-s`)

- **Minimum** : 1 tâche (mais inutile, mieux vaut utiliser `-c`)
- **Maximum** : Aucune limite pratique
- **Comportement** : Les commandes s'exécutent séquentiellement, une après l'autre
- **Code de retour** : Celui de la dernière commande

#### Pipelines (`-p`)

- **Minimum** : 2 tâches
- **Maximum** : Aucune limite pratique
- **Comportement** : La sortie de chaque commande est connectée à l'entrée de la suivante
- **Code de retour** : Celui de la dernière commande

#### Conditionnelles (`-i`)

- **Minimum** : 2 tâches (condition + then)
- **Maximum** : 3 tâches (condition + then + else)
- **Comportement** :
  - Exécute la condition (première tâche)
  - Si exitcode == 0 : exécute then (deuxième tâche)
  - Sinon : exécute else (troisième tâche) si présent, sinon retourne le résultat de la condition
- **Code de retour** : Celui de la branche exécutée

### Combinaisons imbriquées

#### Séquences de séquences

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Exemple dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "inner1")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "inner2")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -s $TASK1 $TASK2)
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "outer")
./tadmor -P /tmp/test-erraid/pipes -s $TASK3 $TASK4
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request (multiple times for nested combinations)
[DEBUG] Creating nested sequence task
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit plusieurs requêtes `OPCODE_COMBINE` pour créer les séquences imbriquées.
- Le démon crée d'abord la séquence interne (`$TASK3`).
- Puis le démon crée la séquence externe qui combine `$TASK3` et `$TASK4`.
- Toutes les tâches intermédiaires sont supprimées (consommées).

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la nouvelle séquence)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle séquence créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4` sont supprimées.
- Le client envoie plusieurs requêtes `OPCODE_COMBINE` pour créer les séquences imbriquées.
- Le client reçoit les réponses avec les identifiants.
- Le client affiche l'identifiant final sur `stdout`.

**Explication :** 
- Crée d'abord une séquence interne (`$TASK3`) de 2 commandes.
- Puis combine cette séquence avec une autre commande pour créer une séquence externe.
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4` sont supprimées.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X: - - - ( ( echo inner1 ; echo inner2 ) ; echo outer )
```

**Explication Terminal 2 :**
- La séquence externe est parenthésée : `( ... )`
- La séquence interne est aussi parenthésée : `( echo inner1 ; echo inner2 )`
- Le `;` sépare les deux commandes de la séquence externe
- Les espaces autour des parenthèses et du `;` sont facultatifs mais présents
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

#### Pipelines de pipelines

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Exemple dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2)
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n wc -c)
./tadmor -P /tmp/test-erraid/pipes -p $TASK3 $TASK4
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request (multiple times for nested combinations)
[DEBUG] Creating nested pipeline task
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit plusieurs requêtes `OPCODE_COMBINE` pour créer les pipelines imbriqués.
- Le démon crée d'abord le pipeline interne (`$TASK3`).
- Puis le démon crée le pipeline externe qui combine `$TASK3` et `$TASK4`.
- Toutes les tâches intermédiaires sont supprimées (consommées).

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la nouvelle pipeline)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle pipeline créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4` sont supprimées.
- Le client envoie plusieurs requêtes `OPCODE_COMBINE` pour créer les pipelines imbriqués.
- Le client reçoit les réponses avec les identifiants.
- Le client affiche l'identifiant final sur `stdout`.

**Explication :**
- Crée d'abord un pipeline interne (`$TASK3`) de 2 commandes.
- Puis combine ce pipeline avec une autre commande pour créer un pipeline externe.
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4` sont supprimées.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X: - - - ( ( echo hello | tr a-z A-Z ) | wc -c )
```

**Explication Terminal 2 :**
- Le pipeline externe est parenthésé : `( ... )`
- Le pipeline interne est aussi parenthésé : `( echo hello | tr a-z A-Z )`
- Le `|` sépare les deux commandes du pipeline externe
- Les espaces autour des parenthèses et du `|` sont facultatifs mais présents
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

#### Conditionnelles avec séquences/pipelines

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Exemple : Conditionnelle dans une séquence**

**Préparation dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n test -f /tmp/nonexistent)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "exists")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "not found")
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK2 $TASK3)
TASK5=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "after")
./tadmor -P /tmp/test-erraid/pipes -s $TASK4 $TASK5
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request (multiple times for nested combinations)
[DEBUG] Creating nested conditional and sequence tasks
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le démon crée d'abord la conditionnelle (`$TASK4`).
- Puis le démon crée la séquence qui combine `$TASK4` et `$TASK5`.
- Toutes les tâches intermédiaires sont supprimées (consommées).

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la nouvelle séquence)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle séquence créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4`, `$TASK5` sont supprimées.
- Le client envoie plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le client reçoit les réponses avec les identifiants.
- Le client affiche l'identifiant final sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X: - - - ( ( if test -f /tmp/nonexistent ; then echo exists ; else echo not found ; fi ) ; echo after )
```

**Explication Terminal 2 :**
- La conditionnelle est entre parenthèses car c'est une combinaison.
- Le `;` est présent entre la conditionnelle (parenthésée) et `echo after` car on combine deux commandes en séquence.
- **Règle importante :** Il n'y a **pas de `;`** entre une sous-commande **combinée** (avec parenthèses) d'une conditionnelle et le mot-clé suivant, MAIS ici on est dans une séquence, donc le `;` sépare les deux commandes de la séquence.
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

**Exemple du professeur : Conditionnelle avec pipeline dans then**

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Préparation dans Terminal 2 (selon l'exemple du prof) :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n host -W 1 informatique.u-paris.fr)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n mr up)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n tail -1)
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo UFR injoignable)
TASK5=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK2 $TASK3)
./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK5 $TASK4 -m 0
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request (multiple times for nested combinations)
[DEBUG] Creating nested pipeline and conditional tasks
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le démon crée d'abord le pipeline (`$TASK5`).
- Puis le démon crée la conditionnelle qui combine `$TASK1`, `$TASK5` et `$TASK4` avec le timing `-m 0`.
- Toutes les tâches intermédiaires sont supprimées (consommées).

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la nouvelle conditionnelle)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle conditionnelle créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4`, `$TASK5` sont supprimées.
- Le client envoie plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le client reçoit les réponses avec les identifiants.
- Le client affiche l'identifiant final sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X: 0 * * ( if host -W 1 informatique.u-paris.fr ; then ( mr up | tail -1 ) else echo UFR injoignable ; fi )
```

**Explication Terminal 2 :**
- `if host -W 1 informatique.u-paris.fr` : Condition (commande simple, non parenthésée)
- `;` : **Impératif** entre commande simple et `then`
- `then` : Mot-clé
- `( mr up | tail -1 )` : Commande then (combinée, parenthésée)
- **PAS de `;`** entre la commande combinée (parenthésée) et `else` (règle du prof)
- `else` : Mot-clé
- `echo UFR injoignable` : Commande else (commande simple, non parenthésée)
- `;` : **Impératif** entre commande simple et `fi`
- `fi` : Mot-clé de fin
- `0 * *` : Timing (minute 0 de chaque heure, tous les jours)
- Le client a reçu la liste des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.

**Règle clé :** 
- Entre une sous-commande **simple** et le mot-clé suivant : `;` **impératif**
- Entre une sous-commande **combinée** (parenthésée) et le mot-clé suivant : **PAS de `;`**

---

## Messages d'erreur

### Erreurs de parsing

#### Option invalide

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -z
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Usage error: invalid option
```

**Explication Terminal 2 :**
- L'option `-z` n'existe pas dans `tadmor`.
- Le parsing détecte l'option invalide.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Option `-c` sans commande

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Error: -c requires a command
```

**Explication Terminal 2 :**
- L'option `-c` nécessite au moins une commande à créer.
- Si aucun argument n'est fourni après `-c`, le parsing détecte l'erreur.
- Le message d'erreur est explicite et indique ce qui manque.
- Le message est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Option `-s` sans taskid

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -s
```

**Résultat attendu :**
```
Error: -s requires at least 1 taskid
```

**Code de retour :** `2`

#### Option `-p` sans taskid ou avec 1 seul taskid

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p
```

**Résultat attendu :**
```
Error: -p requires at least 2 taskids
```

**Code de retour :** `2`

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p 1
```

**Résultat attendu :**
```
Error: -p requires at least 2 taskids
```

**Code de retour :** `2`

#### Option `-i` avec moins de 2 taskids ou plus de 3

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i
```

**Résultat attendu :**
```
Error: -i requires 2 or 3 taskids
```

**Code de retour :** `2`

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i 1
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Error: -i requires 2 or 3 taskids
```

**Explication Terminal 2 :**
- L'option `-i` nécessite exactement 2 ou 3 taskids.
- Si 4 taskids ou plus sont fournis après `-i`, le parsing détecte l'erreur.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i 1 2 3 4
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Error: -i requires 2 or 3 taskids
```

**Explication Terminal 2 :**
- L'option `-i` nécessite exactement 2 ou 3 taskids.
- Si 4 taskids ou plus sont fournis après `-i`, le parsing détecte l'erreur.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Option `-r` sans taskid

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -r
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Error: -r requires a taskid
```

**Explication Terminal 2 :**
- L'option `-r` nécessite un taskid.
- Si aucun taskid n'est fourni après `-r`, le parsing détecte l'erreur.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Option `-x`, `-o`, `-e` sans taskid

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Error: -x requires a taskid
```

**Explication Terminal 2 :**
- L'option `-x` nécessite un taskid.
- Si aucun taskid n'est fourni après `-x`, le parsing détecte l'erreur.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

**Même chose pour `-o` et `-e` :** Même comportement, avec les messages d'erreur respectifs `Error: -o requires a taskid` et `Error: -e requires a taskid`.

### Erreurs de format de timing

#### Format de minutes invalide

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "abc" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid minutes format: abc
```

**Explication Terminal 2 :**
- Le format `"abc"` n'est pas valide pour les minutes (doit être un nombre, une plage, une liste, ou `*`).
- Le parsing détecte l'erreur lors de la validation du format.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Format d'heures invalide

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -H "xyz" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid hours format: xyz
```

**Explication Terminal 2 :**
- Le format `"xyz"` n'est pas valide pour les heures (doit être un nombre, une plage, une liste, ou `*`).
- Le parsing détecte l'erreur lors de la validation du format.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Format de jours invalide

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -d "invalid" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid days format: invalid
```

**Explication Terminal 2 :**
- Le format `"invalid"` n'est pas valide pour les jours (doit être un nombre, une plage, une liste, ou `*`).
- Le parsing détecte l'erreur lors de la validation du format.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Valeur hors limites

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "60" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid minutes format: 60
```

**Explication Terminal 2 :**
- La valeur `60` est hors limites pour les minutes (doit être entre 0 et 59).
- Le parsing détecte l'erreur lors de la validation des limites.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -H "24" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid hours format: 24
```

**Explication Terminal 2 :**
- La valeur `24` est hors limites pour les heures (doit être entre 0 et 23).
- Le parsing détecte l'erreur lors de la validation des limites.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -d "7" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid days format: 7
```

**Explication Terminal 2 :**
- La valeur `7` est hors limites pour les jours (doit être entre 0 et 6).
- Le parsing détecte l'erreur lors de la validation des limites.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

#### Plage invalide

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "5-3" echo "test"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client détecte l'erreur avant d'envoyer la requête.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
Invalid minutes format: 5-3
```

**Explication Terminal 2 :**
- La plage `5-3` est invalide car le début (5) est supérieur à la fin (3).
- Le parsing détecte l'erreur lors de la validation de la plage.
- Le message d'erreur est affiché sur `stderr` du client.
- Le client détecte l'erreur lors du parsing des arguments.
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `2`

### Erreurs de communication

#### Démon non démarré

**État du démon :** ❌ **ARRÊTÉ** (pas en cours d'exécution)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Le démon n'est pas en cours d'exécution, aucun message)
```

**Explication Terminal 1 :**
- Le démon n'est pas actif, donc aucun message n'est visible.
- Aucun processus `erraid` n'est en cours d'exécution.

**Résultat attendu dans Terminal 2 (client) :**
```
Timeout: no response from daemon (is it running?)
```

**Explication Terminal 2 :**
- Le client attend 5 secondes (timeout) une réponse du démon.
- Comme le démon n'est pas actif, aucune réponse n'arrive.
- Le client essaie d'ouvrir les pipes et d'envoyer la requête.
- Aucune réponse n'arrive car le démon n'est pas actif.
- Après 5 secondes de timeout, le client affiche ce message d'erreur explicite.
- Le message est affiché sur `stderr` du client.
- **Ce n'est PAS** l'erreur `NOT_RUN` (qui concerne les tâches jamais exécutées).
- Le client affiche le message d'erreur et se termine.

**Code de retour :** `1`

**Action corrective :** Démarrer le démon dans Terminal 1.

#### Répertoire de pipes inexistant

**État du démon :** ❌ **PEU IMPORTE** (l'erreur se produit avant la communication avec le démon)

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/nonexistent/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Aucun message, le client n'a pas pu communiquer avec le démon car l'erreur se produit avant)
```

**Explication Terminal 1 :**
- Le démon ne reçoit aucune requête car le client ne peut pas ouvrir les pipes.
- Aucune communication n'a lieu entre le client et le démon.

**Résultat attendu dans Terminal 2 (client) :**
```
open_pipes_client: No such file or directory
```
(ou message `perror` équivalent)

**Explication Terminal 2 :**
- Le répertoire `/tmp/nonexistent/pipes` n'existe pas.
- Le client ne peut pas ouvrir les pipes car le répertoire parent n'existe pas.
- L'erreur se produit avant même d'essayer de communiquer avec le démon.
- Le client appelle `open_pipes_client()` qui échoue avec `ENOENT` (No such file or directory).
- Le message d'erreur est affiché sur `stderr` du client (via `perror`).
- Le client n'envoie aucune requête au démon.
- Le client affiche le message d'erreur et se termine immédiatement.

**Code de retour :** `1`

**Action corrective :** Utiliser un répertoire existant ou créer le répertoire avec `mkdir -p`.

### Erreurs de protocole

#### Tâche non trouvée (`NOT_FOUND`)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -r 999
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling REMOVE request for task 999
[DEBUG] Task 999: Checking if task exists...
[DEBUG] Task 999: NOT_FOUND (task directory does not exist)
[DEBUG] Sending error response (ERRCODE=NOT_FOUND)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_REMOVE` avec le taskid 999.
- Le démon vérifie que la tâche 999 existe en cherchant le répertoire `/tmp/test-erraid/tasks/999/`.
- Le répertoire n'existe pas, donc la tâche n'existe pas.
- Le démon retourne l'erreur `NOT_FOUND` (0x4e46 = 'NF') au client.
- Cette erreur apparaît uniquement si le démon est actif et a pu traiter la requête.
- **Différence avec timeout :** Si le démon n'était pas actif, on obtiendrait un timeout, pas `NOT_FOUND`.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication Terminal 2 :**
- `ERROR: NOT_FOUND` : Code d'erreur du protocole (0x4e46 = 'NF').
- Le démon a bien reçu la requête et a vérifié que la tâche 999 n'existe pas.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le client envoie une requête `OPCODE_REMOVE` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_FOUND`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

**Autres cas produisant `NOT_FOUND` :**
- `./tadmor -P /tmp/test-erraid/pipes -x 999` : Historique d'une tâche inexistante
- `./tadmor -P /tmp/test-erraid/pipes -o 999` : Stdout d'une tâche inexistante
- `./tadmor -P /tmp/test-erraid/pipes -e 999` : Stderr d'une tâche inexistante
- `./tadmor -P /tmp/test-erraid/pipes -p 999 1000` : Pipeline avec tâche(s) inexistante(s)
- `./tadmor -P /tmp/test-erraid/pipes -i 999 1000 1001` : Conditionnelle avec tâche(s) inexistante(s)
- `./tadmor -P /tmp/test-erraid/pipes -s 999 1000` : Séquence avec tâche(s) inexistante(s)

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication Terminal 2 :**
- Dans tous ces cas, le démon vérifie que toutes les tâches référencées existent.
- Si au moins une n'existe pas, le démon retourne `NOT_FOUND`.
- Le client reçoit la réponse avec l'erreur `NOT_FOUND`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

#### Tâche jamais exécutée (`NOT_RUN`)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 6
```
(où 6 est une tâche abstraite qui n'a jamais été exécutée)

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task 6
[DEBUG] Task 6: Checking if task exists...
[DEBUG] Task 6: Task exists
[DEBUG] Task 6: Checking times-exitcodes file...
[DEBUG] Task 6: NOT_RUN (no times-exitcodes file)
[DEBUG] Sending error response (ERRCODE=NOT_RUN)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid 6.
- Le démon vérifie que la tâche 6 existe (sinon il retournerait `NOT_FOUND`).
- Le démon vérifie que le fichier `times-exitcodes` existe dans `/tmp/test-erraid/tasks/6/`.
- Le fichier n'existe pas car la tâche n'a jamais été exécutée (tâche abstraite).
- Le démon retourne l'erreur `NOT_RUN` (0x4e52 = 'NR') au client.
- Cette erreur apparaît pour les tâches abstraites ou les tâches qui n'ont pas encore été exécutées par le démon.
- **Différence avec `NOT_FOUND` :** La tâche existe, mais n'a pas d'historique d'exécution.
- **Différence avec timeout :** Si le démon n'était pas actif, on obtiendrait un timeout, pas `NOT_RUN`.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_RUN
```

**Explication Terminal 2 :**
- `ERROR: NOT_RUN` : Code d'erreur du protocole (0x4e52 = 'NR').
- La tâche existe bien (sinon on obtiendrait `NOT_FOUND`), mais elle n'a jamais été exécutée.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le client envoie une requête `OPCODE_TIMES_EXITCODES` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_RUN`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

**Autres cas produisant `NOT_RUN` :**
- `./tadmor -P /tmp/test-erraid/pipes -o 6` : Stdout d'une tâche jamais exécutée
- `./tadmor -P /tmp/test-erraid/pipes -e 6` : Stderr d'une tâche jamais exécutée

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_RUN
```

**Explication Terminal 2 :**
- Pour `-o` et `-e`, le démon vérifie que la tâche a été exécutée au moins une fois.
- Si ce n'est pas le cas, le démon retourne `NOT_RUN`.
- Le client reçoit la réponse avec l'erreur `NOT_RUN`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

**Note importante :** 
- Cette erreur apparaît uniquement si la tâche n'a jamais été exécutée.
- Si le démon n'est pas démarré, on obtient un timeout, pas `NOT_RUN`.
- Si la tâche n'existe pas, on obtient `NOT_FOUND`, pas `NOT_RUN`.

---

## Résumé des codes de retour

### `tadmor`

- **`0`** : Succès
- **`1`** : Erreur de communication ou erreur du démon (NOT_FOUND, NOT_RUN, etc.)
- **`2`** : Erreur de parsing (option invalide, argument manquant, format invalide)

### `erraid`

- **`0`** : Arrêt normal
- **`1`** : Erreur de démarrage (répertoire invalide, pipes invalides, etc.)

---

## Notes importantes pour la présentation

### Points à démontrer

1. **Création de tâches simples** avec différents timings
2. **Combinaisons** (séquences, pipelines, conditionnelles)
3. **Tâches abstraites** et leur utilisation dans les combinaisons
4. **Consultation** (liste, historique, sorties)
5. **Gestion des erreurs** (tâche inexistante, jamais exécutée, format invalide)
6. **Reprise après interruption** (redémarrer le démon et vérifier que les tâches persistent)
7. **Arrêt propre** du démon

### Cas de démonstration recommandés

1. **Créer une tâche simple** qui s'exécute toutes les minutes
2. **Attendre une exécution** et vérifier l'historique
3. **Créer des tâches abstraites** et les combiner
4. **Créer un pipeline** et vérifier son exécution
5. **Créer une conditionnelle** et vérifier son exécution
6. **Tester les erreurs** (tâche inexistante, jamais exécutée)
7. **Arrêter et redémarrer le démon** pour montrer la persistance

### Commandes de démonstration complète

#### Script de démonstration étape par étape

**Terminal 1 : Démon**

**Étape 0 : Préparation et démarrage du démon**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu :** Le démon démarre et reste actif (aucun message visible).

**État :** Démon en cours d'exécution dans Terminal 1.

---

**Terminal 2 : Client**

**Étape 1 : Créer une tâche simple**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./tadmor -P /tmp/test-erraid/pipes -c echo "Hello"
```

**Résultat attendu :** `1` (identifiant de la tâche)

**Explication :** Crée une tâche qui s'exécute toutes les minutes (`* * *` par défaut).

**État :** Démon toujours en cours, 1 tâche créée.

---

**Étape 2 : Lister les tâches**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo Hello
```

**Explication :** La tâche est listée avec son timing par défaut.

**État :** Démon toujours en cours, 1 tâche visible.

---

**Étape 3 : Attendre 1 minute, puis vérifier l'historique**

**Action :** Attendre au moins 1 minute pour que le démon exécute la tâche.

```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu :**
```
2025-12-14 19:10:00 0
```
(La date/heure varient selon le moment réel)

**Explication :** L'historique montre que la tâche a été exécutée au moins une fois.

**État :** Démon toujours en cours, tâche exécutée au moins une fois.

---

**Étape 4 : Vérifier la sortie**
```bash
./tadmor -P /tmp/test-erraid/pipes -o 1
```

**Résultat attendu :**
```
Hello
```

**Explication :** La sortie standard de la dernière exécution est affichée.

**État :** Démon toujours en cours.

---

**Étape 5 : Créer des tâches abstraites**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
```

**Résultat attendu :** `TASK1` et `TASK2` contiennent les identifiants (ex: `2` et `3`).

**Explication :** Crée deux tâches abstraites qui ne s'exécuteront jamais seules.

**État :** Démon toujours en cours, 3 tâches au total (1 normale + 2 abstraites).

---

**Étape 6 : Créer un pipeline**
```bash
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2)
```

**Résultat attendu :** `TASK3` contient l'identifiant (ex: `4`).

**Explication :** Combine les deux tâches abstraites en pipeline. Les tâches `$TASK1` et `$TASK2` sont supprimées.

**État :** Démon toujours en cours, 2 tâches au total (1 normale + 1 pipeline abstraite).

---

**Étape 7 : Lister pour voir le pipeline**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo Hello
4: - - - ( echo hello | tr a-z A-Z )
```

**Explication :** Le pipeline est affiché avec le format correct.

**État :** Démon toujours en cours.

---

**Étape 8 : Créer une conditionnelle**
```bash
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n test -f /tmp/nonexistent)
TASK5=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "exists")
TASK6=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "not found")
TASK7=$(./tadmor -P /tmp/test-erraid/pipes -i $TASK4 $TASK5 $TASK6)
```

**Résultat attendu :** `TASK7` contient l'identifiant (ex: `8`).

**Explication :** Crée une conditionnelle avec 3 branches. Les tâches `$TASK4`, `$TASK5` et `$TASK6` sont supprimées.

**État :** Démon toujours en cours, 3 tâches au total (1 normale + 1 pipeline + 1 conditionnelle).

---

**Étape 9 : Lister pour voir la conditionnelle**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo Hello
4: - - - ( echo hello | tr a-z A-Z )
8: - - - ( if test -f /tmp/nonexistent ; then echo exists ; else echo not found ; fi )
```

**Explication :** La conditionnelle est affichée avec le format correct et les `;` aux bons endroits.

**État :** Démon toujours en cours.

---

**Étape 10 : Tester une erreur (tâche inexistante)**
```bash
./tadmor -P /tmp/test-erraid/pipes -r 999
```

**Résultat attendu :**
```
ERROR: NOT_FOUND
```

**Explication :** Le démon retourne une erreur car la tâche 999 n'existe pas.

**Code de retour :** `1`

**État :** Démon toujours en cours, aucune modification.

---

**Étape 11 : Arrêter le démon**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu :** (vide, aucune sortie)

**Explication :** Le démon reçoit la requête TERMINATE et s'arrête proprement.

**Vérification Terminal 1 :** Le processus `erraid` se termine, le prompt réapparaît.

**État :** Démon arrêté. Les tâches sont conservées sur le disque dans `/tmp/test-erraid/tasks/`.

---

**Terminal 1 : Redémarrer le démon**

**Étape 12 : Redémarrer le démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu :** Le démon redémarre et reste actif.

**Explication :** Le démon charge automatiquement toutes les tâches depuis le disque au démarrage.

**État :** Démon en cours d'exécution dans Terminal 1.

---

**Terminal 2 : Vérifier la persistance**

**Étape 13 : Vérifier que les tâches persistent**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo Hello
4: - - - ( echo hello | tr a-z A-Z )
8: - - - ( if test -f /tmp/nonexistent ; then echo exists ; else echo not found ; fi )
```

**Explication :** Toutes les tâches sont toujours présentes, confirmant que la persistance fonctionne correctement. Le démon a rechargé les tâches depuis le disque.

**État :** Démon en cours, toutes les tâches sont présentes et fonctionnelles.

---

## Cas supplémentaires et cas limites

### Cas : Reprise après interruption

**Scénario :** Le démon s'arrête (crash, kill, ou arrêt normal) et est redémarré.

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1 (initialement)

**Arrêt du démon dans Terminal 1 :**
```bash
# Si le démon est en cours, l'arrêter avec Ctrl+C ou :
pkill -f "erraid -R"
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Le processus se termine, aucun message visible)
```

**Explication Terminal 1 :**
- Le démon s'arrête, mais les tâches restent sur le disque.
- Le processus `erraid` se termine.
- Aucun message n'est visible (sauf si `-d` était activé, dans ce cas on verrait les derniers messages de debug).

**État du démon :** ❌ **ARRÊTÉ** (après l'arrêt)

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Le démon n'est pas en cours d'exécution, aucun message)
```

**Explication Terminal 1 :**
- Le démon n'est pas actif, donc aucun message n'est visible.
- Aucun processus `erraid` n'est en cours d'exécution.

**Résultat attendu dans Terminal 2 (client) :**
```
Timeout: no response from daemon (is it running?)
```

**Explication Terminal 2 :**
- Le démon n'est plus actif, donc les requêtes timeout après 5 secondes.
- Le client essaie d'ouvrir les pipes et d'envoyer la requête.
- Aucune réponse n'arrive car le démon n'est pas actif.
- Après 5 secondes de timeout, le client affiche ce message d'erreur explicite.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).

**Redémarrage du démon dans Terminal 1 :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(Le démon redémarre sans erreur, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded 6 tasks
```

**Explication Terminal 1 :**
- Le démon redémarre sans erreur.
- Le démon charge automatiquement toutes les tâches depuis `/tmp/test-erraid/tasks/` au démarrage.
- Le démon lit tous les répertoires de tâches et désérialise les commandes.
- Dans cet exemple, 6 tâches sont chargées (les tâches créées avant l'arrêt).
- Le démon est prêt à recevoir des requêtes et à exécuter les tâches selon leur timing.
- Les tâches continuent à s'exécuter automatiquement selon leur timing (par exemple, les tâches avec timing `* * *` s'exécutent toutes les minutes).

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1 (après redémarrage)

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: loaded 6 tasks
[DEBUG] handle_request: LIST found 6 tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST` (opcode=19539).
- Le démon recharge les tâches depuis le disque (qui ont été chargées au démarrage).
- Le démon trouve 6 tâches (toutes les tâches créées avant l'arrêt).
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
0: * * * echo Hello World
1: 0,30 14 * echo Test 14h
2: * * * echo Every minute
3: 0-5 8-12 0-2 echo Test range
4: 0-5,10-15,20-25 8-12,18-22 0-2,5-6 echo Test complex
5: - - - echo Abstract task
```
(Toutes les tâches créées avant l'arrêt sont toujours présentes)

**Explication Terminal 2 :**
- Toutes les tâches créées avant l'arrêt sont toujours présentes.
- La persistance fonctionne : les tâches sont stockées sur le disque et rechargées au démarrage.
- Le client reçoit la liste complète des tâches du démon.
- Le client formate et affiche chaque tâche selon le format attendu.
- Le code de retour est `0` (succès).

**Note importante :**
- Les tâches continuent à s'exécuter automatiquement selon leur timing.
- Par exemple, les tâches 0 et 2 (avec timing `* * *`) s'exécutent toutes les minutes.
- Vous verrez dans Terminal 1 (avec `-d`) des messages comme :
  ```
  [DEBUG] Executing task 0 at timestamp 1767527820
  [DEBUG] Task 0: executing simple command
  [DEBUG] Task 0: execution completed (exitcode=0, stdout_len=12, stderr_len=0)
  [DEBUG] Task 0: execution finished successfully
  ```
- C'est normal : le démon reprend l'exécution des tâches automatiquement.

---

### Cas : Nettoyer complètement pour recommencer à zéro

**Scénario :** Vous voulez supprimer toutes les tâches et recommencer avec un répertoire vide.

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1 (ou ❌ **ARRÊTÉ**)

**Étape 1 : Arrêter le démon (si en cours)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu dans Terminal 2 :**
```
(vide, aucune sortie)
```

**OU si le démon n'est pas en cours :**
```
Timeout: no response from daemon (is it running?)
```

**Explication Terminal 2 :**
- Si le démon est en cours, il s'arrête proprement.
- Si le démon n'est pas en cours, vous obtenez un timeout (normal).

**Vérification dans Terminal 2 :**
```bash
ps aux | grep erraid | grep -v grep
```

**Résultat attendu :**
```
(vide, aucun processus)
```

**Explication :** Aucun processus `erraid` n'est en cours d'exécution.

---

**Étape 2 : Supprimer complètement le répertoire de test**

**Commande dans Terminal 2 (ou Terminal 1) :**
```bash
rm -rf /tmp/test-erraid
```

**Résultat attendu :**
```
(vide, aucune sortie)
```

**Explication :**
- La commande `rm -rf` supprime récursivement tout le répertoire `/tmp/test-erraid`.
- Toutes les tâches, tous les logs, et tous les pipes sont supprimés.
- Le répertoire n'existe plus.

**Vérification :**
```bash
ls -la /tmp/test-erraid 2>&1
```

**Résultat attendu :**
```
ls: cannot access '/tmp/test-erraid': No such file or directory
```

**Explication :** Le répertoire a été complètement supprimé.

---

**Étape 3 : Redémarrer le démon avec un répertoire vide**

**Commande dans Terminal 1 :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 (démon) :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: tasks directory does not exist (normal if no tasks)
```

**Explication Terminal 1 :**
- Le démon démarre avec un répertoire vide.
- Le répertoire `/tmp/test-erraid/tasks/` n'existe pas encore (normal).
- Aucune tâche n'est chargée.
- Le démon est prêt à recevoir de nouvelles requêtes.

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1 (répertoire vide)

---

**Étape 4 : Vérifier que le répertoire est vide**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon, si `-d` est activé) :**
```
[DEBUG] Processing client request (reply pipe reopened: fd=4)
[DEBUG] handle_request: received opcode=19539 (taskid=0)
[DEBUG] handle_request: processing LIST request
[DEBUG] load_all_tasks: loading tasks from /tmp/test-erraid/tasks
[DEBUG] load_all_tasks: tasks directory does not exist (normal if no tasks)
[DEBUG] handle_request: LIST found 0 tasks
[DEBUG] Sending response (opcode=19539, anstype=20299, reply_fd=4)
[DEBUG] Response sent successfully, closing reply_fd
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon ne trouve aucune tâche (0 tâches).
- Le démon envoie une liste vide au client.

**Résultat attendu dans Terminal 2 (client) :**
```
(vide, aucune sortie)
```

**Explication Terminal 2 :**
- Aucune tâche n'est listée (le répertoire est vide).
- Le client affiche une liste vide (aucune ligne).
- Le code de retour est `0` (succès, mais liste vide).

---

**Étape 5 : Créer une nouvelle tâche (commence à l'ID 0)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c echo "Nouvelle tâche"
```

**Résultat attendu dans Terminal 2 (client) :**
```
0
```

**Explication Terminal 2 :**
- La première tâche créée après nettoyage a l'ID `0`.
- Les IDs recommencent à partir de 0.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 2 (client) :**
```
0: * * * echo Nouvelle tâche
```

**Explication Terminal 2 :**
- La nouvelle tâche est listée avec l'ID `0`.
- Le timing par défaut est `* * *` (toutes les minutes).

**État après :** Le répertoire est propre, une nouvelle tâche a été créée avec l'ID 0.

---

**Résumé :**

1. **Arrêter le démon** : `./tadmor -P /tmp/test-erraid/pipes -q`
2. **Supprimer le répertoire** : `rm -rf /tmp/test-erraid`
3. **Redémarrer le démon** : `./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d`
4. **Vérifier** : `./tadmor -P /tmp/test-erraid/pipes -l` (doit être vide)
5. **Créer une nouvelle tâche** : `./tadmor -P /tmp/test-erraid/pipes -c echo "test"` (commence à l'ID 0)

---

### Cas : Conditionnelle avec pipeline dans then (exemple du prof)

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Préparation complète selon l'exemple du prof :**
```bash
# Créer les 4 tâches abstraites
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n host -W 1 informatique.u-paris.fr)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n mr up)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n tail -1)
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo UFR injoignable)

# Créer le pipeline (mr up | tail -1)
TASK5=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK2 $TASK3)

# Créer la conditionnelle avec timing
TASK6=$(./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK5 $TASK4 -m 0)
```

**Vérification :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
X: 0 * * ( if host -W 1 informatique.u-paris.fr ; then ( mr up | tail -1 ) else echo UFR injoignable ; fi )
```

**Explication du format :**
- `0 * *` : Timing (minute 0 de chaque heure, tous les jours)
- `if host -W 1 informatique.u-paris.fr` : Condition (simple, non parenthésée)
- `;` : **Impératif** entre commande simple et `then`
- `then` : Mot-clé
- `( mr up | tail -1 )` : Commande then (combinée, parenthésée)
- **PAS de `;`** entre commande combinée et `else` (règle du prof)
- `else` : Mot-clé
- `echo UFR injoignable` : Commande else (simple, non parenthésée)
- `;` : **Impératif** entre commande simple et `fi`
- `fi` : Mot-clé de fin

**État après :** La conditionnelle complexe est créée avec le format correct.

---

### Cas : Tâche avec code de retour non-zéro

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Création dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" false)
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling CREATE request
[DEBUG] Creating task with command: false
[DEBUG] Task created with ID: X
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec la commande `false`.
- Le démon crée une nouvelle tâche avec un identifiant unique (ex: X).
- Le démon enregistre la tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la tâche)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle tâche créée.
- Le code de retour est `0` (succès de la création).
- Le client envoie une requête `OPCODE_CREATE` au démon.
- Le client reçoit la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication :** `false` retourne toujours le code 1 (échec).

**Action : Attendre l'exécution (1 minute minimum)**

**Vérification de l'historique dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task X
[DEBUG] Task X: Reading times-exitcodes file...
[DEBUG] Task X: Found 1 execution(s)
[DEBUG] Sending response with 1 execution record(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid X.
- Le démon lit le fichier `times-exitcodes` qui contient l'enregistrement d'exécution.
- Le démon envoie la réponse avec les données d'exécution au client.

**Résultat attendu dans Terminal 2 (client) :**
```
2025-12-14 19:10:00 1
```

**Explication Terminal 2 :**
- Le code de retour `1` est enregistré dans l'historique, indiquant que la tâche a échoué.
- Le client reçoit la réponse avec les enregistrements d'exécution.
- Le client formate et affiche chaque enregistrement selon le format `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le client affiche chaque ligne sur `stdout`.

---

### Cas : Tâche tuée par un signal

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Création dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "kill -9 $$")
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling CREATE request
[DEBUG] Creating task with command: sh -c kill -9 $$
[DEBUG] Task created with ID: X
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec la commande `sh -c "kill -9 $$"`.
- Le démon crée une nouvelle tâche avec un identifiant unique (ex: X).
- Le démon enregistre la tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la tâche)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle tâche créée.
- Le code de retour est `0` (succès de la création).
- Le client envoie une requête `OPCODE_CREATE` au démon.
- Le client reçoit la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication :** La commande se tue elle-même avec `kill -9`.

**Action : Attendre l'exécution (1 minute minimum)**

**Vérification de l'historique dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task X
[DEBUG] Task X: Reading times-exitcodes file...
[DEBUG] Task X: Found 1 execution(s)
[DEBUG] Task X: Exit code: 65535 (killed by signal)
[DEBUG] Sending response with 1 execution record(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid X.
- Le démon lit le fichier `times-exitcodes` qui contient l'enregistrement d'exécution.
- Le démon détecte que le code de retour est `65535` (0xFFFF), indiquant que la tâche a été tuée par un signal.
- Le démon envoie la réponse avec les données d'exécution au client.

**Résultat attendu dans Terminal 2 (client) :**
```
2025-12-14 19:10:00 65535
```

**Explication Terminal 2 :**
- Le code `65535` (0xFFFF) indique que la tâche a été tuée par un signal ou n'a pas terminé normalement.
- Le client reçoit la réponse avec les enregistrements d'exécution.
- Le client formate et affiche chaque enregistrement selon le format `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le client affiche chaque ligne sur `stdout`.

---

### Cas : Commande inexistante

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Création dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" nonexistentcommand)
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling CREATE request
[DEBUG] Creating task with command: nonexistentcommand
[DEBUG] Task created with ID: X
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec la commande `nonexistentcommand`.
- Le démon crée une nouvelle tâche avec un identifiant unique (ex: X).
- Le démon enregistre la tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.
- **Note :** La tâche est créée même si la commande n'existe pas. L'erreur se produira lors de l'exécution.

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la tâche)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle tâche créée.
- Le code de retour est `0` (succès de la création).
- Le client envoie une requête `OPCODE_CREATE` au démon.
- Le client reçoit la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication :** La tâche est créée même si la commande n'existe pas.

**Action : Attendre l'exécution (1 minute minimum)**

**Vérification de l'historique dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task X
[DEBUG] Task X: Reading times-exitcodes file...
[DEBUG] Task X: Found 1 execution(s)
[DEBUG] Task X: Exit code: 65535 (execvp failed)
[DEBUG] Sending response with 1 execution record(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_TIMES_EXITCODES` avec le taskid X.
- Le démon lit le fichier `times-exitcodes` qui contient l'enregistrement d'exécution.
- Le démon détecte que le code de retour est `65535` (0xFFFF), indiquant que `execvp` a échoué.
- Le démon envoie la réponse avec les données d'exécution au client.

**Résultat attendu dans Terminal 2 (client) :**
```
2025-12-14 19:10:00 65535
```

**Explication Terminal 2 :**
- Le code `65535` indique que la commande n'a pas pu être exécutée (probablement `execvp` a échoué).
- Le client reçoit la réponse avec les enregistrements d'exécution.
- Le client formate et affiche chaque enregistrement selon le format `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le client affiche chaque ligne sur `stdout`.

**Vérification de stderr dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task X
[DEBUG] Task X: Reading stderr file...
[DEBUG] Sending response with stderr content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid X.
- Le démon lit le fichier `stderr` qui contient le message d'erreur du shell.
- Le démon envoie le contenu au client.

**Résultat attendu dans Terminal 2 (client) :**
```
nonexistentcommand: command not found
```
(ou message d'erreur équivalent du shell)

**Explication Terminal 2 :**
- Message d'erreur du shell (ex: "nonexistentcommand: command not found" ou équivalent).
- Le client reçoit la réponse avec le contenu du fichier `stderr`.
- Le client affiche le contenu directement sur `stdout`.

---

### Cas : Combinaison avec tâche inexistante

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -s 999 1000
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling COMBINE request
[DEBUG] Combining 2 tasks (type=SQ)
[DEBUG] Task 999: Checking if task exists...
[DEBUG] Task 999: NOT_FOUND (task directory does not exist)
[DEBUG] Sending error response (ERRCODE=NOT_FOUND)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_COMBINE` avec le type `SQ` et les taskids 999 et 1000.
- Le démon vérifie que toutes les tâches référencées existent.
- Au moins une des tâches (999 ou 1000) n'existe pas.
- Le démon retourne l'erreur `NOT_FOUND` (0x4e46 = 'NF') au client.
- Sans `-d`, aucun message n'est visible.

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication Terminal 2 :**
- Le démon retourne une erreur car au moins une des tâches (999 ou 1000) n'existe pas.
- Le message est affiché sur `stderr` du client.
- Le code de retour est `1` (erreur).
- Le client envoie une requête `OPCODE_COMBINE` au démon.
- Le client reçoit la réponse avec l'erreur `NOT_FOUND`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

**Même comportement pour :**
- `./tadmor -P /tmp/test-erraid/pipes -p 999 1000` : Pipeline avec tâche(s) inexistante(s)
- `./tadmor -P /tmp/test-erraid/pipes -i 999 1000 1001` : Conditionnelle avec tâche(s) inexistante(s)

**Résultat attendu dans Terminal 2 (client) :**
```
ERROR: NOT_FOUND
```

**Explication Terminal 2 :**
- Dans tous ces cas, le démon vérifie que toutes les tâches référencées existent.
- Si au moins une n'existe pas, le démon retourne `NOT_FOUND`.
- Le client reçoit la réponse avec l'erreur `NOT_FOUND`.
- Le client affiche le message d'erreur sur `stderr`.

**Code de retour :** `1`

---

### Cas : Format d'affichage avec combinaisons imbriquées

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Exemple : Pipeline dans une séquence**

**Préparation dans Terminal 2 :**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2)
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "after")
./tadmor -P /tmp/test-erraid/pipes -s $TASK3 $TASK4
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request (multiple times for nested combinations)
[DEBUG] Handling COMBINE request
[DEBUG] Creating nested pipeline and sequence tasks
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le démon crée d'abord le pipeline (`$TASK3`).
- Puis le démon crée la séquence qui combine `$TASK3` et `$TASK4`.
- Toutes les tâches intermédiaires sont supprimées (consommées).

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la nouvelle séquence)

**Explication Terminal 2 :**
- `X` est l'identifiant de la nouvelle séquence créée.
- Le code de retour est `0` (succès).
- Les tâches `$TASK1`, `$TASK2`, `$TASK3`, `$TASK4` sont supprimées.
- Le client envoie plusieurs requêtes `OPCODE_COMBINE` pour créer les combinaisons imbriquées.
- Le client reçoit les réponses avec les identifiants.
- Le client affiche l'identifiant final sur `stdout`.

**Vérification dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling LIST request
[DEBUG] Sending response with X task(s)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_LIST`.
- Le démon lit toutes les tâches depuis le disque.
- Le démon envoie la liste complète au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X: - - - ( ( echo hello | tr a-z A-Z ) ; echo after )
```

**Explication Terminal 2 :**
- La séquence externe est parenthésée : `( ... )`
- Le pipeline interne est parenthésé : `( echo hello | tr a-z A-Z )`
- Le `;` sépare les deux commandes de la séquence
- Les espaces autour des parenthèses et du `;` sont facultatifs mais présents

---

## Vérification de conformité avec le dépôt du professeur

### Points vérifiés

✅ **Options** : `-R`, `-P`, `-F` pour erraid ; `-P`, `-p`, `-i` pour tadmor
✅ **Format d'affichage `-l`** : Conforme aux règles du prof (parenthèses, `;`, `|`, `if...then...else...fi`)
✅ **Format des timings** : Support de `*`, `-`, plages, listes, combinaisons
✅ **Tâches abstraites** : Format `- - -` correct
✅ **Combinaisons** : Séquences (`-s`), Pipelines (`-p`), Conditionnelles (`-i`)
✅ **Exécution** : Tous les types de commandes sont exécutés correctement
✅ **Persistance** : Les tâches sont sauvegardées et rechargées au démarrage
✅ **Protocole** : Tous les opcodes et codes d'erreur sont implémentés
✅ **Exemple du prof** : La conditionnelle complexe avec pipeline dans then fonctionne

### Tests du professeur

✅ **Jalon 1** : 12/12 tests réussis
✅ **Jalon 2** : 11/11 tests réussis (tadmor) + 11/11 tests réussis (erraid)

---

## Conclusion

Ce guide couvre tous les cas d'utilisation possibles du projet erraid-tadmor avec des explications détaillées pour chaque résultat attendu. Tous les tests du professeur (Jalon 1 et Jalon 2) passent avec succès. Le projet est conforme aux spécifications du dépôt du professeur et est prêt pour la présentation devant les jurys.

**Points clés pour la présentation :**
- Tous les formats d'affichage sont corrects selon les règles du prof
- Tous les cas d'erreur retournent des messages précis
- La persistance fonctionne correctement
- Les combinaisons complexes (imbriquées) sont supportées
- L'exemple du prof avec conditionnelle et pipeline fonctionne parfaitement

---

## Section supplémentaire : Commandes détaillées avec résultats attendus

### Commandes du démon `erraid` (Terminal 1)

#### Option `-h` (Help) - Déjà couvert

Voir section "Terminal 1 : Option `-h` (Help)" ci-dessus.

---

#### Démarrage simple avec `-R` et `-P`

**Terminal utilisé :** Terminal 1

**État initial :** Aucun démon en cours d'exécution

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu dans Terminal 1 :**
- Aucune sortie visible (le démon tourne en silence)
- Le démon reste actif et attend les requêtes
- Le processus reste en cours d'exécution dans ce terminal

**Explication :**
- `-R /tmp/test-erraid` : Définit le répertoire de stockage des tâches.
- `-P /tmp/test-erraid/pipes` : Définit le répertoire des pipes de communication.
- `-F` : Force l'exécution en avant-plan (sans démonisation).
- Le démon crée automatiquement :
  - Le répertoire `/tmp/test-erraid/tasks/` pour les tâches
  - Le répertoire `/tmp/test-erraid/pipes/` pour les pipes
  - Les pipes `erraid-request-pipe` et `erraid-reply-pipe`
- Le démon charge toutes les tâches existantes depuis `/tmp/test-erraid/tasks/`.
- Le démon entre dans sa boucle principale et attend les requêtes client.

**Vérification (dans un autre terminal) :**
```bash
# Vérifier que le démon tourne
ps aux | grep erraid | grep -v grep
```

**Résultat attendu :**
```
ajinou  12345  0.0  0.1  ... ./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Explication :** Le processus `erraid` est visible dans la liste des processus.

**Vérification des répertoires :**
```bash
ls -la /tmp/test-erraid/
```

**Résultat attendu :**
```
drwxrwxr-x 3 ajinou ajinou 4096 ... tasks
drwxrwxr-x 2 ajinou ajinou  80 ... pipes
```

**Explication :** Les répertoires `tasks/` et `pipes/` sont créés automatiquement.

**Vérification des pipes :**
```bash
ls -la /tmp/test-erraid/pipes/
```

**Résultat attendu :**
```
total 0
drwxrwxr-x 2 ajinou ajinou 80 ... .
drwxrwxr-x 3 ajinou ajinou 60 ... ..
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-reply-pipe
prw-rw-r-- 1 ajinou ajinou  0 ... erraid-request-pipe
```

**Explication :**
- `ls -la` affiche toujours `.` (répertoire courant) et `..` (répertoire parent) en premier.
- Les pipes sont ensuite affichés dans l'ordre alphabétique : `erraid-reply-pipe` vient avant `erraid-request-pipe` (car 'r' vient avant 'q' dans l'alphabet).
- Les pipes nommés (FIFO) sont créés. Le `p` au début indique que ce sont des pipes.
- **Note :** Le code crée les pipes dans l'ordre `erraid-request-pipe` puis `erraid-reply-pipe`, mais `ls` les affiche dans l'ordre alphabétique, ce qui est normal et attendu.

**État après :** Le démon est en cours d'exécution dans Terminal 1.

---

#### Démarrage avec logs de debug (`-d`)

**Terminal utilisé :** Terminal 1

**État initial :** Aucun démon en cours d'exécution

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Explication :**
- `-d` active les logs de débogage qui sont écrits sur `stderr`.
- Le message indique que la boucle principale du démon a démarré.
- `request_fd=3` : Descripteur de fichier pour le pipe de requêtes (client → démon).
- `reply_fd=4` : Descripteur de fichier pour le pipe de réponses (démon → client).
- Le démon reste actif et affichera d'autres messages de debug lors des événements.

**Comportement :**
- Les messages de debug apparaissent lors des événements :
  - Démarrage du démon
  - Réception d'une requête client
  - Traitement d'une requête
  - Exécution d'une tâche
  - Arrêt du démon

**Test avec requête client :**

**Terminal 2 : Client**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu dans Terminal 1 (après la requête) :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
[DEBUG] Processing client request
[DEBUG] Processing client request (reply pipe reopened: fd=4)
```

**Résultat attendu dans Terminal 2 :**
```
(Liste des tâches ou liste vide)
```

**Explication :**
- Le démon reçoit la requête `LIST` du client.
- Le démon traite la requête et envoie la réponse.
- Les messages de debug montrent le traitement de la requête.
- Le client reçoit la liste des tâches (ou une liste vide si aucune tâche n'existe).

**État après :** Le démon continue de fonctionner, prêt à recevoir d'autres requêtes.

---

#### Démarrage en arrière-plan

**Terminal utilisé :** Terminal 1

**État initial :** Aucun démon en cours d'exécution

**Commande :**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes > /tmp/erraid.log 2>&1 &
```

**Résultat attendu dans Terminal 1 :**
```
[1] 92682
```
(où `[1]` est le numéro du job en arrière-plan et `92682` est le PID du processus)

**Explication Terminal 1 :**
- `&` : Lance le processus en arrière-plan.
- Bash affiche toujours `[job_number] PID` quand on lance un processus en arrière-plan.
- Le prompt réapparaît immédiatement après.
- Les logs sont redirigés vers `/tmp/erraid.log` (pas de sortie visible dans le terminal).
- `> /tmp/erraid.log` : Redirige stdout vers le fichier de log.
- `2>&1` : Redirige stderr vers stdout (donc aussi vers le fichier de log).
- Le démon continue de fonctionner même si le terminal est fermé.

**Vérification dans Terminal 2 :**
```bash
# Vérifier que le démon tourne
ps aux | grep erraid | grep -v grep
```

**Résultat attendu dans Terminal 2 :**
```
ajinou     92682  0.0  0.0  76696  1804 pts/2    Sl   10:47   0:00 ./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes
```

**Explication Terminal 2 :**
- Le processus `erraid` est visible dans la liste des processus.
- Le PID correspond à celui affiché dans Terminal 1 (`92682` dans cet exemple).
- **Note importante :** Si vous voyez plusieurs processus `erraid`, il peut y avoir d'anciens processus qui tournent encore. Vous pouvez les tuer avec `kill <PID>` ou `pkill -f "erraid"` avant de redémarrer.

**Voir les logs dans Terminal 2 :**
```bash
# Voir les logs (si -d n'est pas utilisé, le fichier sera vide)
cat /tmp/erraid.log

# OU suivre les logs en temps réel
tail -f /tmp/erraid.log
```

**Résultat attendu dans Terminal 2 :**
- Sans `-d` : Fichier vide (aucun message). **C'est normal**, le démon n'affiche rien sans `-d`.
- Avec `-d` : Messages de debug dans le fichier (ex: `[DEBUG] daemon_loop started, request_fd=3, reply_fd=4`).

**État après :** Le démon est en cours d'exécution en arrière-plan.

---

### Commandes du client `tadmor` avec résultats détaillés

#### Cas supplémentaires pour `-x` (TIMES_EXITCODES)

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Cas : Tâche avec plusieurs exécutions**

**Préparation :**
- Créer une tâche avec timing `* * *` (toutes les minutes)
- **Attendre au moins 3 minutes** pour que le démon exécute la tâche plusieurs fois

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu :**
```
2025-12-14 19:10:00 0
2025-12-14 19:11:00 0
2025-12-14 19:12:00 0
```

**Explication du résultat :**
- Chaque ligne représente une exécution de la tâche.
- Les exécutions sont triées par ordre chronologique (plus ancienne en premier).
- Le format est `YYYY-MM-DD HH:MM:SS EXITCODE`.
- Le code de retour est `0` (succès de la requête).
- Le client envoie une requête `OPCODE_TIMES_EXITCODES` au démon.
- Le démon lit le fichier `times-exitcodes` de la tâche.
- Le démon désérialise tous les timestamps et codes de retour.
- Le démon envoie la réponse au client avec tous les enregistrements.
- Le client formate et affiche chaque ligne.

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Processing client request
[DEBUG] Handling TIMES_EXITCODES request for task 1
[DEBUG] Task 1: Found 3 execution(s)
[DEBUG] Sending response with 3 execution record(s)
```

**Format :**
- `YYYY-MM-DD HH:MM:SS EXITCODE`
- Une ligne par exécution
- Les exécutions sont triées par ordre chronologique (plus ancienne en premier)
- Le code de retour peut être :
  - `0` à `255` : Code de retour normal de la commande
  - `0xFFFF` (65535) : La tâche a été tuée par un signal ou n'a pas terminé normalement

---

#### Cas supplémentaires pour `-o` (STDOUT)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Cas : Tâche avec sortie multi-lignes**

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'line1'; echo 'line2'; echo 'line3'")
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling CREATE request
[DEBUG] Creating task with command: sh -c echo 'line1'; echo 'line2'; echo 'line3'
[DEBUG] Task created with ID: X
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec la commande.
- Le démon crée une nouvelle tâche avec un identifiant unique (ex: X).
- Le démon enregistre la tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la tâche)

**Explication Terminal 2 :**
- `TASKID` contient l'identifiant (ex: `X`).
- Le code de retour est `0` (succès de la création).
- Le client envoie une requête `OPCODE_CREATE` au démon.
- Le client reçoit la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication :** Crée une tâche qui produit plusieurs lignes sur stdout.

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDOUT request for task X
[DEBUG] Task X: Reading stdout file (size=18 bytes)
[DEBUG] Sending response with stdout content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDOUT` avec le taskid X.
- Le démon lit le fichier `stdout` de la tâche (contient les 3 lignes).
- Le démon envoie le contenu complet au client.

**Résultat attendu dans Terminal 2 (client) :**
```
line1
line2
line3
```

**Explication Terminal 2 :**
- Chaque ligne est préservée avec son saut de ligne.
- L'ordre des lignes est conservé.
- Le démon capture toute la sortie standard, y compris les retours à la ligne.
- Le client envoie une requête `OPCODE_STDOUT` au démon.
- Le client reçoit la réponse avec le contenu du fichier `stdout`.
- Le client affiche le contenu directement sur `stdout`.
- Le code de retour est `0` (succès).

**État après :** La sortie multi-lignes est correctement affichée.

---

#### Cas supplémentaires pour `-e` (STDERR)

**État du démon :** ✅ **EN COURS D'EXÉCUTION** dans Terminal 1

**Cas : Tâche avec erreur multi-lignes**

**Préparation dans Terminal 2 :**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" sh -c "echo 'error1' >&2; echo 'error2' >&2")
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling CREATE request
[DEBUG] Creating task with command: sh -c echo 'error1' >&2; echo 'error2' >&2
[DEBUG] Task created with ID: X
[DEBUG] Sending response (opcode=21581, anstype=20299, reply_fd=4)
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_CREATE` avec la commande.
- Le démon crée une nouvelle tâche avec un identifiant unique (ex: X).
- Le démon enregistre la tâche sur le disque.
- Le démon envoie la réponse avec l'identifiant au client.

**Résultat attendu dans Terminal 2 (client) :**
```
X
```
(où `X` est l'identifiant de la tâche)

**Explication Terminal 2 :**
- `TASKID` contient l'identifiant (ex: `X`).
- Le code de retour est `0` (succès de la création).
- Le client envoie une requête `OPCODE_CREATE` au démon.
- Le client reçoit la réponse avec l'identifiant de la tâche.
- Le client affiche l'identifiant sur `stdout`.

**Explication :** Crée une tâche qui produit plusieurs lignes sur stderr.

**Action : Attendre l'exécution (1 minute minimum)**

**Commande dans Terminal 2 :**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu dans Terminal 1 (démon) :**
```
(vide, aucun message visible si -d n'est pas activé)
```

**OU si `-d` est activé :**
```
[DEBUG] Processing client request
[DEBUG] Handling STDERR request for task X
[DEBUG] Task X: Reading stderr file (size=14 bytes)
[DEBUG] Sending response with stderr content
```

**Explication Terminal 1 :**
- Le démon reçoit la requête `OPCODE_STDERR` avec le taskid X.
- Le démon lit le fichier `stderr` de la tâche (contient les 2 lignes d'erreur).
- Le démon envoie le contenu complet au client.

**Résultat attendu dans Terminal 2 (client) :**
```
error1
error2
```

**Explication Terminal 2 :**
- Chaque ligne est préservée avec son saut de ligne.
- L'ordre des lignes est conservé.
- Le démon capture toute la sortie d'erreur, y compris les retours à la ligne.
- Le client envoie une requête `OPCODE_STDERR` au démon.
- Le client reçoit la réponse avec le contenu du fichier `stderr`.
- Le client affiche le contenu directement sur `stdout`.
- Le code de retour est `0` (succès).

**État après :** La sortie d'erreur multi-lignes est correctement affichée.

---

### Tests manuels supplémentaires

#### Test : Vérification de l'exécution automatique des tâches

**Terminal 1 : Démon**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Terminal 2 : Client - Créer une tâche**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./tadmor -P /tmp/test-erraid/pipes -c echo "Test execution"
```

**Résultat attendu :** `1` (identifiant de la tâche)

**Action : Attendre au moins 1 minute** (jusqu'à la seconde 0 de la minute suivante)

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Checking tasks to execute...
[DEBUG] Task 1: Should execute now
[DEBUG] Executing task 1
[DEBUG] Task 1: Execution completed, exitcode=0
```

**Explication :**
- Le démon vérifie toutes les secondes quelles tâches doivent être exécutées.
- À la seconde 0 de chaque minute, le démon exécute les tâches éligibles.
- Les messages de debug montrent le processus d'exécution.

**Terminal 2 : Vérifier l'historique**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu :**
```
2025-12-14 19:10:00 0
```

**Explication :** La tâche a été exécutée et l'historique est enregistré.

---

#### Test : Vérification des fichiers créés par le démon

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Terminal 2 : Client - Créer et exécuter une tâche**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c echo "test")
# Attendre 1 minute
```

**Vérification des fichiers :**
```bash
# Vérifier la structure de la tâche
ls -la /tmp/test-erraid/tasks/$TASKID/
```

**Résultat attendu :**
```
drwxrwxr-x 2 ajinou ajinou 4096 ... cmd
-rw-rw-r-- 1 ajinou ajinou   20 ... timing
-rw-rw-r-- 1 ajinou ajinou   50 ... times-exitcodes
-rw-rw-r-- 1 ajinou ajinou    5 ... stdout
-rw-rw-r-- 1 ajinou ajinou    0 ... stderr
```

**Explication :**
- `cmd/` : Répertoire contenant la commande (type et arguments)
- `timing` : Fichier binaire contenant le timing de la tâche
- `times-exitcodes` : Fichier binaire contenant l'historique des exécutions
- `stdout` : Fichier texte contenant la sortie standard de la dernière exécution
- `stderr` : Fichier texte contenant la sortie d'erreur de la dernière exécution

**Vérification du contenu :**
```bash
# Voir la sortie standard
cat /tmp/test-erraid/tasks/$TASKID/stdout
```

**Résultat attendu :**
```
test
```

**Explication :** Le contenu du fichier `stdout` correspond à la sortie de la commande `echo "test"`.

---

#### Test : Vérification de la persistance après redémarrage

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Terminal 2 : Client - Créer plusieurs tâches**
```bash
./tadmor -P /tmp/test-erraid/pipes -c echo "task1"
./tadmor -P /tmp/test-erraid/pipes -c echo "task2"
./tadmor -P /tmp/test-erraid/pipes -c echo "task3"
```

**Résultat attendu :** `1`, `2`, `3` (identifiants des tâches)

**Terminal 2 : Lister les tâches**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo task1
2: * * * echo task2
3: * * * echo task3
```

**Terminal 2 : Arrêter le démon**
```bash
./tadmor -P /tmp/test-erraid/pipes -q
```

**Résultat attendu :** (vide, aucune sortie)

**Vérification Terminal 1 :** Le processus `erraid` se termine, le prompt réapparaît.

**Terminal 1 : Redémarrer le démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F
```

**Résultat attendu :** Le démon redémarre sans erreur.

**Terminal 2 : Vérifier que les tâches persistent**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
1: * * * echo task1
2: * * * echo task2
3: * * * echo task3
```

**Explication :**
- Le démon charge automatiquement toutes les tâches depuis `/tmp/test-erraid/tasks/` au démarrage.
- Les tâches sont désérialisées depuis les fichiers sur disque.
- Toutes les tâches créées avant l'arrêt sont toujours présentes.
- La persistance fonctionne correctement.

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Loading tasks from /tmp/test-erraid/tasks/
[DEBUG] Loaded task 1
[DEBUG] Loaded task 2
[DEBUG] Loaded task 3
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Explication :** Les messages de debug montrent que le démon charge les tâches au démarrage.

---

#### Test : Vérification de l'exécution avec code de retour non-zéro

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer une tâche qui échoue**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" false)
```

**Résultat attendu :** `TASKID` contient l'identifiant (ex: `X`).

**Explication :** `false` retourne toujours le code 1 (échec).

**Action : Attendre l'exécution (1 minute minimum)**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Task X: Should execute now
[DEBUG] Executing task X
[DEBUG] Task X: Execution completed, exitcode=1
```

**Explication :** Le démon exécute la tâche et enregistre le code de retour 1 (échec).

**Terminal 2 : Vérifier l'historique**
```bash
./tadmor -P /tmp/test-erraid/pipes -x $TASKID
```

**Résultat attendu :**
```
2025-12-14 19:10:00 1
```

**Explication :**
- Le code de retour `1` est enregistré dans l'historique.
- Cela indique que la tâche a échoué (code de retour non-zéro).
- Le démon enregistre correctement les codes de retour même s'ils sont non-zéro.

---

#### Test : Vérification de l'exécution avec commande inexistante

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer une tâche avec commande inexistante**
```bash
TASKID=$(./tadmor -P /tmp/test-erraid/pipes -c -m "*" -H "*" -d "*" nonexistentcommand)
```

**Résultat attendu :** `TASKID` contient l'identifiant (ex: `X`).

**Explication :** La tâche est créée même si la commande n'existe pas.

**Action : Attendre l'exécution (1 minute minimum)**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Task X: Should execute now
[DEBUG] Executing task X
[DEBUG] Task X: Execution failed (execvp error)
[DEBUG] Task X: Execution completed, exitcode=65535
```

**Explication :**
- Le démon essaie d'exécuter la commande avec `execvp`.
- `execvp` échoue car la commande n'existe pas.
- Le démon enregistre le code de retour spécial `65535` (0xFFFF) pour indiquer une erreur d'exécution.

**Terminal 2 : Vérifier l'historique**
```bash
./tadmor -P /tmp/test-erraid/pipes -x $TASKID
```

**Résultat attendu :**
```
2025-12-14 19:10:00 65535
```

**Explication :** Le code `65535` indique que la commande n'a pas pu être exécutée.

**Terminal 2 : Vérifier stderr**
```bash
./tadmor -P /tmp/test-erraid/pipes -e $TASKID
```

**Résultat attendu :** Message d'erreur du shell (ex: "nonexistentcommand: command not found" ou équivalent).

**Explication :** Le shell a probablement écrit un message d'erreur sur stderr avant que `execvp` n'échoue.

---

### Tests supplémentaires : Cas d'erreur détaillés

#### Test : Client avec démon non démarré

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** **ARRÊTÉ** (pas en cours d'exécution)

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -l
```

**Résultat attendu :**
```
Timeout: no response from daemon (is it running?)
```

**Explication du résultat :**
- Le client ouvre les pipes en mode non-bloquant.
- Le client écrit la requête dans le pipe de requêtes.
- Le client attend 5 secondes (timeout) une réponse du démon.
- Comme le démon n'est pas actif, aucune réponse n'arrive.
- Le client affiche ce message d'erreur explicite après le timeout.
- **Ce n'est PAS** l'erreur `NOT_RUN` (qui concerne les tâches jamais exécutées).

**Code de retour :** `1`

**Action corrective :** Démarrer le démon dans Terminal 1.

---

#### Test : Client avec répertoire de pipes inexistant

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** Peu importe (l'erreur se produit avant la communication)

**Commande :**
```bash
./tadmor -P /tmp/nonexistent/pipes -l
```

**Résultat attendu :**
```
open_pipes_client: No such file or directory
```
(ou message `perror` équivalent)

**Explication du résultat :**
- Le répertoire `/tmp/nonexistent/pipes` n'existe pas.
- Le client essaie d'ouvrir les pipes dans ce répertoire.
- `open()` ou `mkfifo()` échoue avec `ENOENT` (No such file or directory).
- Le client affiche le message d'erreur et se termine.
- L'erreur se produit avant même d'essayer de communiquer avec le démon.

**Code de retour :** `1`

**Action corrective :** Utiliser un répertoire existant ou créer le répertoire avec `mkdir -p`.

---

#### Test : Création de tâche avec timing invalide

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -c -m "60" echo "test"
```

**Résultat attendu :**
```
Invalid minutes format: 60
```

**Explication du résultat :**
- `-m "60"` : La valeur 60 est hors limites (les minutes vont de 0 à 59).
- Le client valide le format avant d'envoyer la requête au démon.
- Le parsing détecte que 60 est hors limites.
- Le client affiche un message d'erreur explicite.
- Le code de retour est `2` (erreur de parsing).

**Code de retour :** `2`

**Même comportement pour :**
- `-H "24"` : Heure 24 hors limites (0-23)
- `-d "7"` : Jour 7 hors limites (0-6)
- `-m "5-3"` : Plage invalide (début > fin)
- `-m "abc"` : Format invalide (non numérique)

---

#### Test : Combinaison avec moins de 2 tâches pour pipeline

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -p 1
```

**Résultat attendu :**
```
Error: -p requires at least 2 taskids
```

**Explication du résultat :**
- `-p` nécessite au moins 2 tâches pour créer un pipeline.
- Le client valide le nombre de tâches avant d'envoyer la requête.
- Le parsing détecte qu'il n'y a qu'une seule tâche.
- Le client affiche un message d'erreur explicite.
- Le code de retour est `2` (erreur de parsing).

**Code de retour :** `2`

**Même comportement pour :**
- `./tadmor -P /tmp/test-erraid/pipes -p` : Aucune tâche fournie

---

#### Test : Combinaison conditionnelle avec moins de 2 tâches

**Terminal utilisé :** Terminal 2 (client)

**État du démon :** En cours d'exécution dans Terminal 1

**Commande :**
```bash
./tadmor -P /tmp/test-erraid/pipes -i 1
```

**Résultat attendu :**
```
Error: -i requires 2 or 3 taskids
```

**Explication du résultat :**
- `-i` nécessite 2 ou 3 tâches (condition + then, ou condition + then + else).
- Le client valide le nombre de tâches avant d'envoyer la requête.
- Le parsing détecte qu'il n'y a qu'une seule tâche.
- Le client affiche un message d'erreur explicite.
- Le code de retour est `2` (erreur de parsing).

**Code de retour :** `2`

**Même comportement pour :**
- `./tadmor -P /tmp/test-erraid/pipes -i` : Aucune tâche fournie
- `./tadmor -P /tmp/test-erraid/pipes -i 1 2 3 4` : Plus de 3 tâches fournies

---

### Tests avec exécution automatique du démon

#### Test : Vérification de l'exécution à la seconde 0

**Terminal 1 : Démon**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
rm -rf /tmp/test-erraid*
mkdir -p /tmp/test-erraid/pipes
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Résultat attendu dans Terminal 1 :**
```
[DEBUG] daemon_loop started, request_fd=3, reply_fd=4
```

**Terminal 2 : Client - Créer une tâche**
```bash
cd /home/ajinou/Bureau/Projet\ System/projet-systeme-l3
./tadmor -P /tmp/test-erraid/pipes -c echo "Executed at second 0"
```

**Résultat attendu :** `1` (identifiant de la tâche)

**Action : Attendre jusqu'à la seconde 0 de la minute suivante**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Checking tasks to execute...
[DEBUG] Task 1: Should execute now (current time matches timing)
[DEBUG] Executing task 1
[DEBUG] Task 1: Forked child process (pid=XXXXX)
[DEBUG] Task 1: Child process completed, exitcode=0
[DEBUG] Task 1: Execution completed, exitcode=0
```

**Explication :**
- Le démon vérifie toutes les secondes quelles tâches doivent être exécutées.
- À la seconde 0 de chaque minute, le démon exécute les tâches éligibles.
- Le démon crée un processus enfant avec `fork()`.
- Le processus enfant exécute la commande avec `execvp()`.
- Le démon attend la fin du processus enfant avec `waitpid()`.
- Le démon enregistre le code de retour et les sorties.

**Terminal 2 : Vérifier l'historique immédiatement après**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu :**
```
2025-12-14 19:10:00 0
```

**Explication :** La tâche a été exécutée à la seconde 0 de la minute 19:10.

**Terminal 2 : Vérifier la sortie**
```bash
./tadmor -P /tmp/test-erraid/pipes -o 1
```

**Résultat attendu :**
```
Executed at second 0
```

**Explication :** La sortie standard de la dernière exécution est disponible.

---

#### Test : Vérification de l'exécution avec timing précis

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer une tâche avec timing précis**
```bash
# Créer une tâche qui s'exécute à 14h00 et 14h30 chaque jour
./tadmor -P /tmp/test-erraid/pipes -c -m 0,30 -H 14 echo "14h00 or 14h30"
```

**Résultat attendu :** `1` (identifiant de la tâche)

**Action : Attendre jusqu'à 14h00 ou 14h30 (selon l'heure actuelle)**

**Résultat attendu dans Terminal 1 (si `-d` est activé et que l'heure correspond) :**
```
[DEBUG] Checking tasks to execute...
[DEBUG] Task 1: Should execute now (current time matches timing: minute=0 or 30, hour=14)
[DEBUG] Executing task 1
[DEBUG] Task 1: Execution completed, exitcode=0
```

**Explication :**
- Le démon vérifie que l'heure actuelle correspond au timing de la tâche.
- La tâche s'exécute uniquement aux minutes 0 et 30 de l'heure 14.
- Le démon compare l'heure actuelle avec le timing de chaque tâche.
- Si le timing correspond, la tâche est exécutée.

**Terminal 2 : Vérifier l'historique après exécution**
```bash
./tadmor -P /tmp/test-erraid/pipes -x 1
```

**Résultat attendu :**
```
2025-12-14 14:00:00 0
```
(ou `2025-12-14 14:30:00 0` selon l'heure d'exécution)

**Explication :** La tâche a été exécutée uniquement aux heures spécifiées.

---

#### Test : Vérification de l'exécution de séquences

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer une séquence**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "first")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "second")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -s $TASK1 $TASK2 -m "*" -H "*" -d "*")
```

**Résultat attendu :** `TASK3` contient l'identifiant (ex: `3`).

**Action : Attendre l'exécution (1 minute minimum)**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Task 3: Should execute now
[DEBUG] Executing task 3 (sequence with 2 sub-commands)
[DEBUG] Task 3: Executing sub-command 0: echo first
[DEBUG] Task 3: Sub-command 0 completed, exitcode=0
[DEBUG] Task 3: Executing sub-command 1: echo second
[DEBUG] Task 3: Sub-command 1 completed, exitcode=0
[DEBUG] Task 3: Execution completed, exitcode=0
```

**Explication :**
- Le démon exécute les commandes de la séquence **séquentiellement**.
- Chaque commande est exécutée une après l'autre.
- Le code de retour final est celui de la dernière commande.
- Les sorties de toutes les commandes sont capturées.

**Terminal 2 : Vérifier la sortie**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASK3
```

**Résultat attendu :**
```
first
second
```

**Explication :**
- La sortie standard contient les sorties de toutes les commandes de la séquence.
- Les sorties sont concaténées dans l'ordre d'exécution.

---

#### Test : Vérification de l'exécution de pipelines

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer un pipeline**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "hello")
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n tr a-z A-Z)
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -p $TASK1 $TASK2 -m "*" -H "*" -d "*")
```

**Résultat attendu :** `TASK3` contient l'identifiant (ex: `3`).

**Action : Attendre l'exécution (1 minute minimum)**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Task 3: Should execute now
[DEBUG] Executing task 3 (pipeline with 2 sub-commands)
[DEBUG] Task 3: Creating pipe between commands
[DEBUG] Task 3: Executing sub-command 0: echo hello
[DEBUG] Task 3: Executing sub-command 1: tr a-z A-Z
[DEBUG] Task 3: Sub-command 0 completed, exitcode=0
[DEBUG] Task 3: Sub-command 1 completed, exitcode=0
[DEBUG] Task 3: Execution completed, exitcode=0
```

**Explication :**
- Le démon crée un pipe entre les deux commandes.
- La sortie de `echo "hello"` est connectée à l'entrée de `tr a-z A-Z`.
- Les commandes s'exécutent en parallèle (pas séquentiellement).
- Le code de retour final est celui de la dernière commande.

**Terminal 2 : Vérifier la sortie**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASK3
```

**Résultat attendu :**
```
HELLO
```

**Explication :**
- La sortie standard contient uniquement la sortie de la dernière commande du pipeline.
- `echo "hello"` produit "hello" qui est passé à `tr a-z A-Z` qui produit "HELLO".
- Seule "HELLO" est capturée (sortie de la dernière commande).

---

#### Test : Vérification de l'exécution de conditionnelles

**Terminal 1 : Démon**
```bash
./erraid -R /tmp/test-erraid -P /tmp/test-erraid/pipes -F -d
```

**Terminal 2 : Client - Créer une conditionnelle**
```bash
TASK1=$(./tadmor -P /tmp/test-erraid/pipes -c -n test -f /tmp/nonexistent)
TASK2=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "file exists")
TASK3=$(./tadmor -P /tmp/test-erraid/pipes -c -n echo "file not found")
TASK4=$(./tadmor -P /tmp/test-erraid/pipes -i $TASK1 $TASK2 $TASK3 -m "*" -H "*" -d "*")
```

**Résultat attendu :** `TASK4` contient l'identifiant (ex: `4`).

**Action : Attendre l'exécution (1 minute minimum)**

**Résultat attendu dans Terminal 1 (si `-d` est activé) :**
```
[DEBUG] Task 4: Should execute now
[DEBUG] Executing task 4 (conditional with 3 sub-commands)
[DEBUG] Task 4: Executing condition: test -f /tmp/nonexistent
[DEBUG] Task 4: Condition completed, exitcode=1
[DEBUG] Task 4: Condition failed (exitcode=1), executing else branch
[DEBUG] Task 4: Executing else: echo file not found
[DEBUG] Task 4: Else branch completed, exitcode=0
[DEBUG] Task 4: Execution completed, exitcode=0
```

**Explication :**
- Le démon exécute d'abord la condition (`test -f /tmp/nonexistent`).
- La condition retourne 1 (échec) car le fichier n'existe pas.
- Comme le code de retour est non-zéro, le démon exécute la branche **else**.
- Le démon n'exécute **pas** la branche then.
- Le code de retour final est celui de la branche else (0 dans ce cas).

**Terminal 2 : Vérifier la sortie**
```bash
./tadmor -P /tmp/test-erraid/pipes -o $TASK4
```

**Résultat attendu :**
```
file not found
```

**Explication :**
- La sortie standard contient uniquement la sortie de la branche exécutée (else).
- La branche then n'a pas été exécutée, donc sa sortie n'est pas capturée.

---

### Résumé des terminaux utilisés

#### Terminal 1 : Démon `erraid`

**Utilisé pour :**
- Démarrer le démon (`./erraid -R ... -P ... -F`)
- Démarrer le démon avec debug (`./erraid -R ... -P ... -F -d`)
- Voir les logs de debug en temps réel
- Voir les messages d'arrêt du démon

**État du démon :**
- **En cours d'exécution** : Le processus `erraid` est actif, le prompt n'est pas visible (sauf avec `-F` en avant-plan)
- **Arrêté** : Le processus s'est terminé, le prompt est visible

**Messages visibles :**
- Sans `-d` : Aucun message (sauf erreurs)
- Avec `-d` : Messages de debug sur `stderr` :
  - Démarrage : `[DEBUG] daemon_loop started, request_fd=X, reply_fd=Y`
  - Requêtes : `[DEBUG] Processing client request`
  - Exécutions : `[DEBUG] Executing task X`
  - Arrêt : `[DEBUG] daemon_loop exiting (g_stop=1, iterations=5)`

#### Terminal 2 : Client `tadmor`

**Utilisé pour :**
- Toutes les commandes client (`./tadmor -P ...`)
- Création de tâches (`-c`, `-s`, `-p`, `-i`)
- Consultation de tâches (`-l`, `-x`, `-o`, `-e`)
- Suppression de tâches (`-r`)
- Arrêt du démon (`-q`)

**État du démon :**
- **En cours d'exécution** : Les requêtes fonctionnent normalement
- **Arrêté** : Les requêtes timeout avec "Timeout: no response from daemon"

**Messages visibles :**
- Résultats des commandes (identifiants, listes, sorties)
- Messages d'erreur (`ERROR: NOT_FOUND`, `ERROR: NOT_RUN`)
- Messages de timeout si le démon n'est pas actif

---

### Guide de vérification complète

Pour vérifier que tout fonctionne correctement, suivez ces étapes dans l'ordre :

1. **Terminal 1** : Démarrer le démon avec `-d` pour voir les logs
2. **Terminal 2** : Créer quelques tâches simples
3. **Terminal 2** : Lister les tâches pour vérifier qu'elles sont créées
4. **Attendre 1-2 minutes** : Laisser le démon exécuter les tâches
5. **Terminal 1** : Vérifier les logs de debug pour voir les exécutions
6. **Terminal 2** : Vérifier l'historique des tâches (`-x`)
7. **Terminal 2** : Vérifier les sorties des tâches (`-o`, `-e`)
8. **Terminal 2** : Créer des combinaisons (séquences, pipelines, conditionnelles)
9. **Terminal 2** : Vérifier le format d'affichage (`-l`)
10. **Terminal 2** : Arrêter le démon (`-q`)
11. **Terminal 1** : Vérifier que le démon s'est arrêté
12. **Terminal 1** : Redémarrer le démon
13. **Terminal 2** : Vérifier que les tâches persistent (`-l`)

Toutes ces étapes doivent fonctionner sans erreur pour confirmer que le projet est complet et fonctionnel.


