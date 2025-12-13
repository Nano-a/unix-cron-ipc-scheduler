# Commandes du Démon erraid

## Syntaxe

```bash
./erraid [OPTIONS]
```

---

## Options

### `-r RUN_DIRECTORY`

Spécifie le répertoire de stockage des tâches et des pipes.

**Valeur par défaut** : `/tmp/$USER/erraid`

**Exemple** :
```bash
./erraid -r /tmp/test-erraid
```

**Comportement** :
- Crée le répertoire s'il n'existe pas
- Crée le sous-répertoire `tasks/` pour les tâches
- Crée le sous-répertoire `pipes/` pour les tubes nommés
- Initialise les pipes `erraid-request-pipe` et `erraid-reply-pipe`

---

### `-p PIPES_DIR`

Spécifie le répertoire contenant les tubes de communication.

**Valeur par défaut** : `RUN_DIRECTORY/pipes`

**Exemple** :
```bash
./erraid -r /tmp/test-erraid -p /tmp/mes-pipes
```

**Note** : En général, on utilise `-r` seul, et les pipes sont automatiquement créés dans `RUN_DIRECTORY/pipes`.

---

### `-d` (Debug)

Active les logs de débogage (écrits sur `stderr`).

**Exemple** :
```bash
./erraid -r /tmp/test-erraid -d
```

**Comportement** :
- Affiche des messages de débogage sur `stderr`
- Utile pour diagnostiquer les problèmes

---

### `-h` (Help)

Affiche l'aide et quitte.

**Exemple** :
```bash
./erraid -h
```

---

## Comportement

### Démarrage

1. **Initialisation** :
   - Initialise les répertoires et pipes
   - Charge toutes les tâches depuis `tasks/`
   - Démarre un thread d'exécution des tâches
   - Ouvre les pipes pour la communication client
   - Entre dans la boucle principale (`daemon_loop`)

2. **Exécution des tâches** :
   - Un thread séparé vérifie toutes les secondes quelles tâches doivent être exécutées
   - Les tâches sont exécutées de manière asynchrone (chaque tâche dans son propre thread)
   - Les résultats sont enregistrés dans les fichiers de log

3. **Réception des requêtes** :
   - La boucle principale utilise `select()` pour attendre les requêtes client
   - Timeout de 1 seconde pour permettre la vérification périodique
   - Traite chaque requête et envoie la réponse

4. **Arrêt** :
   - Réception d'une requête `TERMINATE` (`-q` du client)
   - Réception d'un signal `SIGTERM` ou `SIGINT`
   - Fermeture propre des pipes et arrêt des threads

---

## Exemples d'Utilisation

### Démarrage Simple

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./erraid -r /tmp/test-erraid
```

**Résultat** : Le démon démarre et reste actif (aucun message visible).

---

### Démarrage avec Logs Debug

```bash
./erraid -r /tmp/test-erraid -d
```

**Résultat** : Le démon démarre et affiche des logs de débogage sur `stderr`.

---

### Démarrage en Arrière-Plan

```bash
./erraid -r /tmp/test-erraid > /tmp/erraid.log 2>&1 &
```

**Résultat** : Le démon démarre en arrière-plan, les logs sont redirigés vers `/tmp/erraid.log`.

**Vérification** :
```bash
# Vérifier que le démon tourne
ps aux | grep erraid

# Voir les logs
tail -f /tmp/erraid.log
```

---

### Arrêt du Démon

#### Méthode 1 : Via le Client

```bash
./tadmor -q -p /tmp/test-erraid/pipes
```

#### Méthode 2 : Via un Signal

```bash
# Trouver le PID du démon
ps aux | grep erraid

# Envoyer SIGTERM
kill <PID>
```

---

## Codes de Retour

- **0** : Arrêt normal
- **1** : Erreur au démarrage (impossible d'initialiser les répertoires/pipes)

---

## Prochaines Étapes

- **Commandes du client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)
- **Tests** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)
- **Gestion des erreurs** : [ERREURS.md](ERREURS.md)

