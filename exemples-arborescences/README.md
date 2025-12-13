# Dossier `exemples-arborescences/`

Ce dossier contient des **exemples d'arborescences de tâches** au format tar.gz, utilisables pour tester le démon `erraid`.

## 📁 Contenu

Chaque fichier `.tar.gz` contient une arborescence complète de tâches au format attendu par le démon.

### Fichiers Disponibles

1. **`exemple-arborescence-1.tar.gz`**
   - Arborescence simple avec 2 tâches
   - Tâche 0 : `echo "Feed me lasagna"`
   - Tâche 1 : `echo "I hate mondays"`

2. **`exemple-arborescence-2.tar.gz`**
   - Arborescence avec séquences
   - Tâche 4 : Séquence avec `date`

3. **`exemple-arborescence-5.tar.gz`**
   - Commandes avec code de retour non-zéro
   - Tâche 15 : Commande qui retourne 1

4. **`exemple-arborescence-6-fork.tar.gz`**
   - Commandes avec fork
   - Teste l'exécution de commandes qui créent des processus

5. **`exemple-arborescence-7-commandes-longues.tar.gz`**
   - Commandes de longue durée (ex: `sleep 110`)
   - Teste que les commandes continuent après SIGTERM

6. **`exemple-arborescence-8-commandes-longues-chevauchement.tar.gz`**
   - Commandes longues avec chevauchement
   - Teste l'exécution simultanée

7. **`exemple-arborescence-9-commandes-longues-stdout.tar.gz`**
   - Commandes longues avec écriture sur stdout
   - Teste la capture de sortie

10. **`exemple-arborescence-10-commandes-externes.tar.gz`**
    - Commandes externes imprévisibles
    - Teste la gestion de commandes externes

11. **`exemple-arborescence-11-arborescence-profonde.tar.gz`**
    - Arborescence profonde avec séquences complexes
    - Teste la gestion récursive

13. **`exemple-arborescence-13-taches-simultanees.tar.gz`**
    - Tâches qui s'exécutent simultanément
    - Teste le multithreading

14. **`exemple-arborescence-14-tache-jamais-lancee.tar.gz`**
    - Tâche qui ne doit jamais être exécutée
    - Teste la gestion des timings

## 🚀 Utilisation

### Extraction d'une Arborescence

```bash
cd exemples-arborescences
tar -xzf exemple-arborescence-1.tar.gz
```

**Résultat** : Crée un répertoire `tmp-username-erraid/` avec la structure complète.

### Utilisation avec le Démon

```bash
# Extraire l'arborescence
tar -xzf exemple-arborescence-1.tar.gz

# Copier dans un répertoire de test
mkdir -p /tmp/test-jalon1
cp -r tmp-username-erraid/* /tmp/test-jalon1/

# Lancer le démon
cd ../..
./erraid -r /tmp/test-jalon1
```

### Structure d'une Arborescence Extraite

```
tmp-username-erraid/
├── tasks/
│   ├── 0/
│   │   ├── timing
│   │   ├── cmd/
│   │   │   ├── type
│   │   │   └── argv
│   │   ├── times-exitcodes
│   │   ├── stdout
│   │   └── stderr
│   ├── 1/
│   │   └── ...
│   └── ...
└── pipes/
    ├── erraid-request-pipe
    └── erraid-reply-pipe
```

## 📝 Format des Fichiers

### `timing`
Format binaire big-endian :
- `minutes` : uint64 (8 octets)
- `hours` : uint32 (4 octets)
- `daysofweek` : uint8 (1 octet)

### `cmd/type`
Format binaire big-endian :
- Type de commande : uint16 (2 octets)
  - `0x5349` = "SI" (simple)
  - `0x5351` = "SQ" (séquence)

### `cmd/argv`
Format binaire big-endian :
- `argc` : uint32 (4 octets)
- Pour chaque argument :
  - `length` : uint32 (4 octets)
  - `data` : char[length] (length octets)

### `times-exitcodes`
Format binaire big-endian :
- Série de paires `(timestamp int64, exitcode uint16)`
- Timestamp : secondes depuis epoch (8 octets)
- Exitcode : code de retour (2 octets)

### `stdout` / `stderr`
Format texte brut (contenu exact de la sortie).

## 🔍 Vérification du Format

Pour vérifier le format binaire d'un fichier :

```bash
# Vérifier le timing
hexdump -C tasks/0/timing

# Vérifier le type de commande
hexdump -C tasks/0/cmd/type

# Vérifier les logs d'exécution
hexdump -C tasks/0/times-exitcodes

# Voir la sortie standard (texte)
cat tasks/0/stdout
```

## 📚 Documentation

Pour plus de détails sur le format, voir :
- `../sy5-2025-2026/Projet/arborescence.md` - Structure de stockage
- `../sy5-2025-2026/Projet/serialisation.md` - Format de sérialisation

## ⚠️ Notes

- Les arborescences sont **statiques** : elles ne peuvent pas être modifiées par le client (Jalon 1 et 2)
- Les fichiers binaires sont en **big-endian** (format réseau)
- Les timestamps sont en **secondes depuis epoch** (1970-01-01 00:00:00 UTC)

