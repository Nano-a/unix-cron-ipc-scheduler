# Architecture du Système erraid-tadmor

## Vue d'Ensemble

Le système erraid-tadmor est composé de deux processus qui communiquent via des **tubes nommés (FIFO)** :

```
┌─────────────┐         Tubes Nommés          ┌─────────────┐
│   tadmor    │ ────────────────────────────> │   erraid    │
│   (client)  │ <──────────────────────────── │   (démon)   │
└─────────────┘                               └─────────────┘
```

---

## Communication Client-Démon

### Tubes Nommés (FIFO)

Le client et le démon communiquent via **deux tubes nommés** :

1. **`erraid-request-pipe`** : Client → Démon (requêtes)
2. **`erraid-reply-pipe`** : Démon → Client (réponses)

### Emplacement des Pipes

Les pipes sont créés dans un répertoire dédié :

**Par défaut** : `/tmp/$USER/erraid/pipes/`

**Personnalisable** :
- Démon : `./erraid -p /chemin/vers/pipes`
- Client : `./tadmor -p /chemin/vers/pipes`

### Format de Communication

Tous les messages sont sérialisés en **big-endian** (format réseau) :

- **Requêtes** : OPCODE + données spécifiques
- **Réponses** : ANSTYPE (OK/ERROR) + données spécifiques

**Voir** : `sy5-2025-2026/Projet/protocole.md` pour le format détaillé.

---

## Structure de Stockage

### Arborescence des Tâches

Le démon stocke toutes les informations dans une arborescence :

```
/tmp/$USER/erraid/
├── pipes/
│   ├── erraid-request-pipe    # Tube de requêtes
│   └── erraid-reply-pipe      # Tube de réponses
└── tasks/
    ├── 0/                     # Tâche ID 0
    │   ├── timing             # Horaires d'exécution (binaire)
    │   ├── cmd/               # Commande à exécuter (binaire)
    │   ├── times-exitcodes    # Historique d'exécution (binaire)
    │   ├── stdout             # Sortie standard (texte)
    │   └── stderr             # Sortie d'erreur (texte)
    ├── 1/
    │   └── ...
    └── ...
```

### Format des Fichiers

#### `timing`

Format binaire big-endian :
- `minutes` : uint64 (bits 0-59)
- `hours` : uint32 (bits 0-23)
- `daysofweek` : uint8 (bits 0-6, 0=dimanche, 6=samedi)

#### `cmd/`

Format binaire big-endian :
- Type de commande (simple ou séquence)
- Arguments ou sous-commandes

#### `times-exitcodes`

Format binaire big-endian :
- Série de paires `(timestamp int64, exitcode uint16)`
- Timestamp : secondes depuis epoch (1970-01-01 00:00:00 UTC)

#### `stdout` / `stderr`

Format texte brut (contenu exact de la sortie de la commande).

---

## Format de Sérialisation

### Principes

Toutes les données sont sérialisées en **big-endian** (format réseau) :

- **Entiers** : `uint16`, `uint32`, `uint64`, `int64`
- **Chaînes** : longueur (uint32) + données (bytes)
- **Structures** : timing, commandes, etc.

### Exemple : Sérialisation d'une Chaîne

```
"Hello" → [00 00 00 05] [48 65 6c 6c 6f]
          └ longueur ─┘ └─── données ──┘
```

### Exemple : Sérialisation d'un Timing

```
minutes=0x0000000000000001 (minute 0)
hours=0x00000001 (heure 0)
daysofweek=0x01 (dimanche)

→ [00 00 00 00 00 00 00 01] [00 00 00 01] [01]
  └────── minutes ────────┘ └── hours ──┘ └dow┘
```

**Voir** : `sy5-2025-2026/Projet/serialisation.md` pour le format complet.

---

## Architecture du Démon

### Processus Principal

Le démon `erraid` utilise une architecture **multithread** :

```
┌─────────────────────────────────────┐
│         Processus Principal         │
│                                     │
│  ┌────────────────────────────────┐ │
│  │   Thread d'Exécution           │ │
│  │   (vérifie toutes les secondes)│ │
│  └────────────────────────────────┘ │
│                                     │
│  ┌───────────────────────────────┐  │
│  │   Boucle Principale           │  │
│  │   (select() pour requêtes)    │  │
│  └───────────────────────────────┘  │
│                                     │
│  ┌───────────────────────────────┐  │
│  │   Threads d'Exécution         │  │
│  │   (une par tâche, asynchrone) │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

### Thread d'Exécution des Tâches

- Vérifie **toutes les secondes** quelles tâches doivent être exécutées
- Exécute les tâches de manière **asynchrone** (chaque tâche dans son propre thread)
- Enregistre les résultats dans les fichiers de log

### Boucle Principale

- Utilise `select()` pour attendre les requêtes client
- Timeout de 1 seconde pour permettre la vérification périodique
- Traite chaque requête et envoie la réponse

---

## Architecture du Client

### Processus Simple

Le client `tadmor` est un processus **synchrone** :

1. Parse les arguments de la ligne de commande
2. Construit la requête
3. Ouvre les tubes nommés
4. Envoie la requête
5. Attend la réponse (avec timeout de 5 secondes)
6. Affiche le résultat
7. Termine

### Gestion du Timeout

Si le démon ne répond pas dans les 5 secondes, le client affiche :
```
Timeout: no response from daemon (is it running?)
```

---

## Exécution des Tâches

### Timing

Les tâches sont exécutées **uniquement à la seconde 0** de chaque minute (comme `cron`).

### Isolation des Processus

Chaque commande exécutée est lancée dans un processus séparé avec `setsid()`, ce qui :
- Détache le processus du groupe du démon
- Permet aux commandes longues de continuer même si le démon reçoit un `SIGTERM`

### Capture des Sorties

Les sorties standard et d'erreur sont capturées et enregistrées dans les fichiers `stdout` et `stderr`.

---

## Prochaines Étapes

- **Comprendre le Jalon 1** : [JALON_1.md](JALON_1.md)
- **Comprendre le Jalon 2** : [JALON_2.md](JALON_2.md)
- **Utiliser le démon** : [COMMANDES_ERRAID.md](COMMANDES_ERRAID.md)
- **Utiliser le client** : [COMMANDES_TADMOR.md](COMMANDES_TADMOR.md)

