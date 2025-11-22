#!/bin/bash
set -euo pipefail
RUN_DIR="/tmp/erraid_j1_$$"
LOG_DIR="logs-j1"
mkdir -p "$RUN_DIR" "$LOG_DIR"

cleanup() {
  kill $(jobs -p) 2>/dev/null || true
  wait 2>/dev/null || true
  rm -rf "$RUN_DIR"
}
trap cleanup EXIT INT TERM

echo "=== Compilation ===" | tee "$LOG_DIR/steps.log"
make clean && make >>"$LOG_DIR/steps.log" 2>&1

echo "=== Création d'une tâche de test avec programme C ===" | tee -a "$LOG_DIR/steps.log"
# Créer un programme C pour créer une tâche
cat > /tmp/create_task_test.c <<'EOFC'
#define _DEFAULT_SOURCE
#include "task_tree.h"
#include "serialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <run_dir>\n", argv[0]);
        return 1;
    }
    const char *run_dir = argv[1];
    
    if (init_task_directory(run_dir) < 0) {
        perror("init_task_directory");
        return 1;
    }
    
    // Créer une tâche simple
    task_t *task = calloc(1, sizeof(task_t));
    if (!task) {
        perror("calloc task");
        return 1;
    }
    
    task->taskid = 0;
    task->timing.minutes = UINT64_MAX;  // Toutes les minutes
    task->timing.hours = UINT32_MAX;    // Toutes les heures
    task->timing.daysofweek = 0x7F;     // Tous les jours
    
    // Créer commande: echo "Hello J1"
    char *argv_cmd[] = {"echo", "Hello J1", NULL};
    task->cmd = create_simple_command("SI", 2, argv_cmd);
    if (!task->cmd) {
        perror("create_simple_command");
        free(task);
        return 1;
    }
    
    if (save_task_to_dir(run_dir, task) < 0) {
        perror("save_task_to_dir");
        free_command(task->cmd);
        free(task);
        return 1;
    }
    
    free_command(task->cmd);
    free(task);
    printf("Tâche créée avec succès\n");
    return 0;
}
EOFC

gcc -Wall -Wextra -std=c11 -Iinclude -o /tmp/create_task_test /tmp/create_task_test.c src/serialization.c src/task_tree.c
/tmp/create_task_test "$RUN_DIR" >>"$LOG_DIR/steps.log" 2>&1
rm -f /tmp/create_task_test.c /tmp/create_task_test

echo "=== Démarrage du démon ===" | tee -a "$LOG_DIR/steps.log"
./erraid -r "$RUN_DIR" >"$LOG_DIR/erraid.log" 2>&1 &
ERRAID_PID=$!
sleep 3

echo "=== Attente exécution (10 secondes) ===" | tee -a "$LOG_DIR/steps.log"
sleep 10

echo "=== Vérification des logs ===" | tee -a "$LOG_DIR/check.log"
TASK_DIR="$RUN_DIR/erraid/tasks/0"

if [ -f "$TASK_DIR/times-exitcodes" ]; then
    echo "✅ times-exitcodes existe" | tee -a "$LOG_DIR/check.log"
    ls -lh "$TASK_DIR/times-exitcodes" | tee -a "$LOG_DIR/check.log"
    # Afficher le contenu (premiers octets)
    hexdump -C "$TASK_DIR/times-exitcodes" | head -5 | tee -a "$LOG_DIR/check.log"
else
    echo "❌ times-exitcodes manquant" | tee -a "$LOG_DIR/check.log"
    exit 1
fi

if [ -f "$TASK_DIR/stdout" ]; then
    echo "✅ stdout existe" | tee -a "$LOG_DIR/check.log"
    echo "Contenu:" | tee -a "$LOG_DIR/check.log"
    cat "$TASK_DIR/stdout" | tee -a "$LOG_DIR/check.log"
    if grep -q "Hello J1" "$TASK_DIR/stdout"; then
        echo "✅ Contenu stdout correct" | tee -a "$LOG_DIR/check.log"
    else
        echo "❌ Contenu stdout incorrect" | tee -a "$LOG_DIR/check.log"
        exit 1
    fi
else
    echo "❌ stdout manquant" | tee -a "$LOG_DIR/check.log"
    exit 1
fi

if [ -f "$TASK_DIR/stderr" ]; then
    echo "✅ stderr existe" | tee -a "$LOG_DIR/check.log"
    echo "Taille:" | tee -a "$LOG_DIR/check.log"
    ls -lh "$TASK_DIR/stderr" | tee -a "$LOG_DIR/check.log"
else
    echo "⚠️  stderr manquant (peut être normal si pas d'erreur)" | tee -a "$LOG_DIR/check.log"
fi

echo "=== Arrêt du démon ===" | tee -a "$LOG_DIR/steps.log"
kill "$ERRAID_PID" 2>/dev/null || true
wait "$ERRAID_PID" 2>/dev/null || true

echo "✅ Intégration jalon 1 OK" | tee -a "$LOG_DIR/steps.log"
