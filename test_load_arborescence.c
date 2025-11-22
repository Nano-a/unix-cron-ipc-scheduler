#define _POSIX_C_SOURCE 200809L
#include "task_tree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <run_dir> <taskid>\n", argv[0]);
        return 1;
    }

    const char *run_dir = argv[1];
    uint64_t taskid = strtoull(argv[2], NULL, 10);

    task_t *task = NULL;
    if (load_task_from_dir(run_dir, taskid, &task) < 0) {
        perror("load_task_from_dir");
        return 1;
    }

    printf("Tâche %lu chargée avec succès\n", taskid);
    printf("Timing: minutes=0x%016llx, hours=0x%08x, daysofweek=0x%02x\n",
           (unsigned long long)task->timing.minutes,
           (unsigned)task->timing.hours,
           (unsigned)task->timing.daysofweek);
    printf("Commande type: 0x%04x\n", task->cmd->type);
    
    if (task->cmd->type == type_from_str("SI")) {
        printf("Commande simple avec %u arguments:\n", task->cmd->argc);
        for (uint32_t i = 0; i < task->cmd->argc; ++i) {
            printf("  argv[%u] = \"%s\"\n", i, task->cmd->argv[i]);
        }
    } else {
        printf("Commande complexe avec %u sous-commandes\n", task->cmd->nb_cmds);
    }

    free_task(task);
    return 0;
}

