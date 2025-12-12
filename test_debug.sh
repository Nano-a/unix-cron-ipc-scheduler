#!/bin/bash

# Script de test pour voir les logs DEBUG

RUNDIR="/tmp/ajinou/test-erraid-debug"
rm -rf "$RUNDIR"
mkdir -p "$RUNDIR/tasks/0/cmd"

# Créer une tâche simple
echo -n "simple" > "$RUNDIR/tasks/0/cmd/type"
echo -n "echo 'Task executed'" > "$RUNDIR/tasks/0/cmd/argv"

# Créer un timing qui s'exécute toutes les minutes
python3 << 'PYTHON'
import struct
with open('/tmp/ajinou/test-erraid-debug/tasks/0/timing', 'wb') as f:
    # minutes: tous (0x0fffffffffffffff)
    f.write(struct.pack('>Q', 0x0fffffffffffffff))
    # hours: tous (0x00ffffff)
    f.write(struct.pack('>I', 0x00ffffff))
    # daysofweek: tous sauf dimanche (0x7e)
    f.write(struct.pack('B', 0x7e))
PYTHON

echo "=== Démarrage du démon (logs stderr) ==="
echo "Appuyez sur Ctrl+C pour arrêter"
echo ""

cd "$(dirname "$0")"
./erraid -r "$RUNDIR" -d 2>&1

