# Jalon 1 : Démon avec Arborescence Statique

## Vue d'Ensemble

Le Jalon 1 consiste à implémenter un démon `erraid` capable d'exécuter des tâches depuis une arborescence **statique et préexistante**.

### Objectif

Le démon doit :
- ✅ Lire une arborescence de tâches depuis le disque
- ✅ Exécuter les tâches aux dates prescrites
- ✅ Enregistrer les résultats dans les fichiers de log

### Limitations

- ❌ Pas de communication client-démon
- ❌ L'arborescence est statique (ne peut pas être modifiée)
- ❌ Le client `tadmor` n'est pas utilisé

---

## Fonctionnement

### Démarrage

1. Le démon lit l'arborescence depuis `RUN_DIRECTORY/tasks/`
2. Charge toutes les tâches en mémoire
3. Démarre un thread d'exécution des tâches
4. Entre dans une boucle infinie

### Exécution des Tâches

Le démon vérifie **toutes les secondes** quelles tâches doivent être exécutées :

1. **Vérification du timing** : Compare l'heure actuelle avec le timing de chaque tâche
2. **Exécution** : Si une tâche doit être exécutée, elle est lancée dans un thread séparé
3. **Enregistrement** : Les résultats sont enregistrés dans les fichiers de log

### Timing d'Exécution

Les tâches sont exécutées **uniquement à la seconde 0** de chaque minute (comme `cron`).

Le timing est défini par :
- **Minutes** : bits 0-59 (ex: `0x0000000000000001` = minute 0)
- **Heures** : bits 0-23 (ex: `0x00000001` = heure 0)
- **Jours de la semaine** : bits 0-6 (0=dimanche, 6=samedi)

**Exemple** :
- Tâche exécutée tous les lundis à 9h00 : `minutes=0x0000000000000001, hours=0x00000200, daysofweek=0x02`

---

## Format des Tâches

### Tâche Simple

Une tâche simple exécute une commande unique :

**Exemple** :
```
echo "Hello World"
```

**Structure dans l'arborescence** :
```
tasks/0/
├── timing
├── cmd/
├── times-exitcodes
├── stdout
└── stderr
```

### Tâche Séquentielle

Une tâche séquentielle exécute plusieurs commandes à la suite :

**Exemple** :
```
( echo "Première" ; echo "Deuxième" ; echo "Troisième" )
```

**Structure dans l'arborescence** :
```
tasks/4/
├── timing
├── cmd/          # Contient la séquence
├── times-exitcodes
├── stdout
└── stderr
```

---

## Fichiers de Log

Pour chaque exécution, le démon enregistre :

### `times-exitcodes`

Format binaire big-endian :
- Série de paires `(timestamp int64, exitcode uint16)`
- Timestamp : secondes depuis epoch (1970-01-01 00:00:00 UTC)
- Exitcode : code de retour de la commande (ou 0xFFFF si erreur)

**Exemple** :
```
[timestamp1][exitcode1][timestamp2][exitcode2]...
```

### `stdout`

Format texte brut :
- Contenu exact de la sortie standard de la dernière exécution complète

**Exemple** :
```
Hello World
```

### `stderr`

Format texte brut :
- Contenu exact de la sortie d'erreur de la dernière exécution complète

**Exemple** :
```
(peut être vide si pas d'erreur)
```

---

## Exécution des Commandes

### Isolation des Processus

Chaque commande est exécutée dans un processus séparé avec `setsid()`, ce qui :
- Détache le processus du groupe du démon
- Permet aux commandes longues de continuer même si le démon reçoit un `SIGTERM`

### Capture des Sorties

Les sorties standard et d'erreur sont capturées et enregistrées dans les fichiers `stdout` et `stderr`.

---

## Commandes du Démon

### Syntaxe

```bash
./erraid [OPTIONS]
```

### Options

#### `-r RUN_DIRECTORY`

Spécifie le répertoire de stockage des tâches.

**Valeur par défaut** : `/tmp/$USER/erraid`

**Exemple** :
```bash
./erraid -r /tmp/test-jalon1
```

#### `-d` (Debug)

Active les logs de débogage (écrits sur `stderr`).

**Exemple** :
```bash
./erraid -r /tmp/test-jalon1 -d
```

#### `-h` (Help)

Affiche l'aide et quitte.

---

## Exemple d'Utilisation

### 1. Préparer une Arborescence

```bash
# Extraire une arborescence d'exemple
cd '/home/ajinou/Bureau/Projet System/sy5-2025-2026/Projet/exemples-arborescences'
tar -xzf exemple-arborescence-1.tar.gz

# Copier dans un répertoire de test
mkdir -p /tmp/test-jalon1
cp -r tmp-username-erraid/* /tmp/test-jalon1/
```

### 2. Démarrer le Démon

```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./erraid -r /tmp/test-jalon1
```

**Résultat** : Le démon démarre et reste actif.

### 3. Attendre l'Exécution

Attendez au moins 1 minute pour que les tâches s'exécutent.

### 4. Vérifier les Résultats

```bash
# Vérifier l'historique d'exécution
hexdump -C /tmp/test-jalon1/tasks/0/times-exitcodes | head -5

# Vérifier la sortie standard
cat /tmp/test-jalon1/tasks/0/stdout

# Vérifier la sortie d'erreur
cat /tmp/test-jalon1/tasks/0/stderr
```

---

## Tests

### Tests Automatiques

Voir [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md#jalon-1--tests-du-démon) pour les détails complets.

**Commande** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
python3 test-jalon-1.py
```

**Résultat attendu** :
```
Jalon 1 : 12/12 tests réussis, 0 échecs
```

### Tests Manuels

Voir [TESTS_MANUELS.md](TESTS_MANUELS.md) pour un guide de test manuel.

---

## Prochaines Étapes

- **Jalon 2** : [JALON_2.md](JALON_2.md)
- **Commandes du démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Tests** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)

