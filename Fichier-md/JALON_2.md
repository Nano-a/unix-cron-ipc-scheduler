# Jalon 2 : Requêtes Consultatives

## Vue d'Ensemble

Le Jalon 2 ajoute la communication client-démon pour les requêtes **consultatives** uniquement.

### Objectif

Le démon doit :
- ✅ Maintenir toutes les fonctionnalités du Jalon 1
- ✅ Écouter les requêtes client via les tubes nommés
- ✅ Répondre aux requêtes consultatives
- ✅ Continuer d'exécuter les tâches périodiquement

### Limitations

- ❌ Pas de création de tâches (`CREATE`)
- ❌ Pas de suppression de tâches (`REMOVE`)
- ❌ Pas de combinaison de tâches (`COMBINE`)
- Les tâches doivent toujours être chargées depuis une arborescence préexistante

---

## Communication Client-Démon

### Tubes Nommés

Le client et le démon communiquent via deux tubes nommés :

1. **`erraid-request-pipe`** : Client → Démon (requêtes)
2. **`erraid-reply-pipe`** : Démon → Client (réponses)

### Emplacement

**Par défaut** : `/tmp/$USER/erraid/pipes/`

**Personnalisable** :
- Démon : `./erraid -p /chemin/vers/pipes`
- Client : `./tadmor -p /chemin/vers/pipes`

---

## Types de Requêtes

### 1. LIST (OPCODE: 0x4c53 'LS')

**Commande client** : `./tadmor -l`

**Requête** :
```
OPCODE='LS' <uint16>
```

**Réponse OK** :
```
ANSTYPE='OK' <uint16>
NBTASKS=N <uint32>
TASK[0].TASKID <uint64>
TASK[0].TIMING <timing>
TASK[0].COMMANDLINE <commandline>
...
```

**Affichage client** :
```
0: * * * echo Feed me lasagna
1: 0 * 1 echo I hate mondays
4: * * * echo -n Nous sommes le  ; date +%d/%m/%Y ; echo -n Il est  ; date +%H:%M:%S
```

**Voir** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md#1-list--lister-toutes-les-tâches) pour plus de détails.

---

### 2. TIMES_EXITCODES (OPCODE: 0x5458 'TX')

**Commande client** : `./tadmor -x TASKID`

**Requête** :
```
OPCODE='TX' <uint16>
TASKID <uint64>
```

**Réponse OK** :
```
ANSTYPE='OK' <uint16>
NBRUNS=N <uint32>
RUN[0].TIME <int64>
RUN[0].EXITCODE <uint16>
...
```

**Réponse ERROR** :
```
ANSTYPE='ER' <uint16>
ERRCODE='NF' <uint16>  // Tâche non trouvée
```

**Affichage client** :
```
2025-12-13 14:08:00 0
2025-12-13 14:09:00 0
2025-12-13 14:10:00 0
```

**Cas spéciaux** :
- Si `NBRUNS=0` : Affiche `"No runs recorded for this task."`
- Si tâche inexistante : Affiche `"ERROR: NOT_FOUND"`

**Voir** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md#2-times_exitcodes--x-tâcheid) pour plus de détails.

---

### 3. STDOUT (OPCODE: 0x534f 'SO')

**Commande client** : `./tadmor -o TASKID`

**Requête** :
```
OPCODE='SO' <uint16>
TASKID <uint64>
```

**Réponse OK** :
```
ANSTYPE='OK' <uint16>
OUTPUT <string>
```

**Réponse ERROR** :
```
ANSTYPE='ER' <uint16>
ERRCODE <uint16>  // 'NF' (non trouvé) ou 'NR' (non exécutée)
```

**Affichage client** :
- Si OK : Affiche le contenu de `stdout` directement sur `stdout`
- Si ERROR : Affiche `"ERROR: NOT_FOUND"` ou `"ERROR: NOT_RUN"` sur `stderr`

**Voir** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md#3-stdout--o-tâcheid) pour plus de détails.

---

### 4. STDERR (OPCODE: 0x5345 'SE')

**Commande client** : `./tadmor -e TASKID`

**Fonctionnement identique à STDOUT**, mais lit le fichier `stderr` au lieu de `stdout`.

**Voir** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md#4-stderr--e-tâcheid) pour plus de détails.

---

### 5. TERMINATE (OPCODE: 0x544d 'TM')

**Commande client** : `./tadmor -q`

**Requête** :
```
OPCODE='TM' <uint16>
```

**Réponse OK** :
```
ANSTYPE='OK' <uint16>
```

**Comportement** :
- Le démon arrête proprement son exécution
- Le client n'affiche rien (succès silencieux)
- Le démon ferme les pipes et termine

**Voir** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md#5-terminate--q) pour plus de détails.

---

## Architecture du Démon

### Multithreading

Le démon utilise une architecture multithread :

1. **Thread d'exécution des tâches** : Vérifie toutes les secondes quelles tâches doivent être exécutées
2. **Boucle principale** : Utilise `select()` pour attendre les requêtes client
3. **Threads d'exécution** : Chaque tâche est exécutée dans son propre thread

### Gestion des Requêtes

1. Le démon attend les requêtes avec `select()` (timeout 1 seconde)
2. Quand une requête arrive, elle est traitée
3. La réponse est envoyée et le tube de réponse est fermé
4. Le tube de réponse est rouvert avant la prochaine requête

---

## Exemple d'Utilisation

### 1. Préparer une Arborescence

```bash
cd '/home/ajinou/Bureau/Projet System/sy5-2025-2026/Projet/exemples-arborescences'
tar -xzf exemple-arborescence-2.tar.gz
mkdir -p /tmp/test-jalon2
cp -r tmp-username-erraid/* /tmp/test-jalon2/
```

### 2. Démarrer le Démon

**Terminal 1** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'
./erraid -r /tmp/test-jalon2
```

### 3. Utiliser le Client

**Terminal 2** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3'

# Lister les tâches
./tadmor -l -p /tmp/test-jalon2/pipes

# Voir l'historique d'une tâche
./tadmor -x 0 -p /tmp/test-jalon2/pipes

# Voir la sortie standard
./tadmor -o 0 -p /tmp/test-jalon2/pipes

# Arrêter le démon
./tadmor -q -p /tmp/test-jalon2/pipes
```

---

## Tests

### Tests Automatiques

Voir [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md#jalon-2--tests-du-client-tadmor) pour les détails complets.

**Client** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
bash run-tadmor-tests-jalon-2.sh
```

**Démon** :
```bash
cd '/home/ajinou/Bureau/Projet System/projet-systeme-l3/tests-prof'
python3 run-erraid-tests-jalon-2.py
```

**Résultat attendu** :
```
Jalon 2 : 11/11 tests réussis, 0 échecs
```

### Tests Manuels

Voir [TESTS_MANUELS.md](TESTS_MANUELS.md) pour un guide de test manuel.

---

## Prochaines Étapes

- **Commandes du client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)
- **Commandes du démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Tests** : [TESTS_PROFESSEUR.md](TESTS_PROFESSEUR.md)

