#define _POSIX_C_SOURCE 200809L

#include "execution.h"
#include "task_tree.h"
#include "serialization.h"

#include "protocol.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

static volatile sig_atomic_t g_stop = 0;
static int g_debug_logs = 0;

// Macro pour logs de debug (désactivée par défaut, activée avec -d)
#define DEBUG_LOG(...) do { \
    if (g_debug_logs) { \
        fprintf(stderr, __VA_ARGS__); \
    } \
} while(0)

// Structure pour passer les paramètres d'exécution de tâche à un thread
typedef struct {
    const char *run_dir;
    const task_t *task;
    time_t exec_timestamp;
} task_exec_params_t;

// Forward declarations
static int should_execute_task_simple(const task_t *task);
static void* task_execution_thread(void *arg);
static void* async_task_executor(void *arg);
int execute_task(const char *run_dir, const task_t *task);
int execute_task_with_timestamp(const char *run_dir, const task_t *task, time_t exec_timestamp);

static void handle_signal(int sig) {
    (void)sig;
    g_stop = 1;
}

static int is_number(const char *name) {
    if (!name || *name == '\0') {
        return 0;
    }
    for (const char *p = name; *p; ++p) {
        if (!isdigit((unsigned char)*p)) {
            return 0;
        }
    }
    return 1;
}

static void free_task_array(task_t **tasks, size_t count) {
    if (!tasks) return;
    for (size_t i = 0; i < count; ++i) {
        free_task(tasks[i]);
    }
    free(tasks);
}

static int load_all_tasks(const char *run_dir, task_t ***tasks_out, size_t *count_out) {
    char tasks_dir[MAX_PATH_LEN];
    int len = snprintf(tasks_dir, sizeof(tasks_dir), "%s/tasks", run_dir);
    if (len < 0 || len >= (int)sizeof(tasks_dir)) {
        DEBUG_LOG("[DEBUG] load_all_tasks: path too long for run_dir=%s\n", run_dir);
        errno = ENAMETOOLONG;
        return -1;
    }
    //DEBUG_LOG("[DEBUG] load_all_tasks: loading tasks from %s\n", tasks_dir);
    DIR *dir = opendir(tasks_dir);
    if (!dir) {
        if (errno == ENOENT) {
            // Ne pas logger si le répertoire n'existe pas (c'est normal s'il n'y a pas de tâches)
            // pour éviter de spammer les logs toutes les 10 secondes
            //DEBUG_LOG("[DEBUG] load_all_tasks: tasks directory does not exist (normal if no tasks)\n");
            *tasks_out = NULL;
            *count_out = 0;
            return 0;
        }
        DEBUG_LOG("[DEBUG] load_all_tasks: failed to open directory: %s\n", strerror(errno));
        return -1;
    }
    
    DEBUG_LOG("[DEBUG] load_all_tasks: loading tasks from %s\n", tasks_dir);

    size_t capacity = 8;
    size_t count = 0;
    task_t **tasks = calloc(capacity, sizeof(task_t *));
    if (!tasks) {
        closedir(dir);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (!is_number(entry->d_name)) {
            continue;
        }

        uint64_t taskid = strtoull(entry->d_name, NULL, 10);
        task_t *task = NULL;
        if (load_task_from_dir(run_dir, taskid, &task) == 0) {
            if (count == capacity) {
                capacity *= 2;
                task_t **tmp = realloc(tasks, capacity * sizeof(task_t *));
                if (!tmp) {
                    free_task(task);
                    free_task_array(tasks, count);
                    closedir(dir);
                    return -1;
                }
                tasks = tmp;
            }
            tasks[count++] = task;
        }
    }
    closedir(dir);

    if (count > 0) {
        DEBUG_LOG("[DEBUG] load_all_tasks: loaded %zu tasks\n", count);
    }
    *tasks_out = tasks;
    *count_out = count;
    return 0;
}

// Structure pour mémoriser la dernière minute d'exécution de chaque tâche
typedef struct {
    uint64_t taskid;
    int last_executed_minute; // -1 si jamais exécutée
} task_execution_memory_t;

// Vérifie si une tâche doit être exécutée maintenant
// (exécution strictement à la seconde 0 de chaque minute, comme cron)
static int should_execute_task_simple(const task_t *task) {
    if (!task) {
        return 0;
    }
    time_t now = time(NULL);
    struct tm tm_now;
    if (localtime_r(&now, &tm_now) == NULL) {
        return 0;
    }

    // Exécution uniquement au début de la minute (comme cron)
    // Exécution strictement à la seconde 0 de chaque minute
    if (tm_now.tm_sec != 0) {
        return 0;
    }

    if (!(task->timing.minutes & (1ULL << tm_now.tm_min))) {
        return 0;
    }
    if (!(task->timing.hours & (1U << tm_now.tm_hour))) {
        return 0;
    }
    if (!(task->timing.daysofweek & (1U << tm_now.tm_wday))) {
        return 0;
    }
    return 1;
}

// Thread pour l'exécution périodique des tâches (vérification multiple dans première seconde)
static void* task_execution_thread(void *arg) {
    const char *run_dir = (const char *)arg;
    time_t start_time = time(NULL);
    struct tm tm_start;
    localtime_r(&start_time, &tm_start);
    
    // Précharger les tâches une fois au démarrage
    task_t **cached_tasks = NULL;
    size_t cached_count = 0;
    if (load_all_tasks(run_dir, &cached_tasks, &cached_count) < 0) {
        return NULL;
    }
    
    // Mémoriser la dernière minute où chaque tâche a été exécutée
    int *last_executed_minute = NULL;
    if (cached_count > 0) {
        last_executed_minute = calloc(cached_count, sizeof(int));
        for (size_t i = 0; i < cached_count; i++) {
            last_executed_minute[i] = -1;
        }
    }
    
    // Vérifier immédiatement si on est à la seconde 0
    if (tm_start.tm_sec == 0 && cached_tasks) {
        int start_minute_id = tm_start.tm_year * 525600 + tm_start.tm_mon * 43200 + 
                             tm_start.tm_mday * 1440 + tm_start.tm_hour * 60 + tm_start.tm_min;
        for (size_t i = 0; i < cached_count && !g_stop; ++i) {
            if (should_execute_task_simple(cached_tasks[i])) {
                time_t exec_time = start_time;
                execute_task_with_timestamp(run_dir, cached_tasks[i], exec_time);
                if (last_executed_minute) {
                    last_executed_minute[i] = start_minute_id;
                }
            }
        }
    }
    
    // Synchroniser sur la prochaine seconde 0 si nécessaire
    // Utiliser nanosleep avec vérification de g_stop pour arrêt rapide
    int secs_to_wait = 60 - tm_start.tm_sec;
    if (secs_to_wait > 0 && secs_to_wait < 60 && !g_stop) {
        // Diviser le sleep en intervalles de 100ms pour vérifier g_stop fréquemment
        int intervals = secs_to_wait * 10; // 10 intervalles de 100ms par seconde
        for (int i = 0; i < intervals && !g_stop; i++) {
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 100000000; // 100ms
            nanosleep(&ts, NULL);
        }
    }
    
    int iteration = 0;
    const int MAX_ITERATIONS = 1000000; // Limite pour éviter boucle infinie
    int last_checked_minute_id = -1;
    
    while (!g_stop && iteration < MAX_ITERATIONS) {
        iteration++;
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);
        
        // Recharger les tâches périodiquement pour détecter les nouvelles tâches
        // Avec un sleep de 1ms, 60000 itérations = 60 secondes
        // Ne pas recharger à la seconde 0 pour éviter d'interférer avec l'exécution
        if (iteration % 60000 == 0 && tm_now.tm_sec != 0 && !g_stop) { // Toutes les 60 secondes, sauf à la seconde 0
            // Sauvegarder l'ancien état AVANT de libérer
            int *old_last_executed = last_executed_minute;
            size_t old_count = cached_count;
            
            if (cached_tasks) {
                free_task_array(cached_tasks, cached_count);
                cached_tasks = NULL;
                cached_count = 0;
            }
            last_executed_minute = NULL; // Temporairement NULL
            
            if (!g_stop && load_all_tasks(run_dir, &cached_tasks, &cached_count) == 0) {
                if (!g_stop) {
                    // Si le nombre de tâches est le même, réutiliser l'ancien tableau
                    if (old_count == cached_count && old_last_executed) {
                        last_executed_minute = old_last_executed;
                    } else {
                        // Sinon, réallouer
                        if (old_last_executed) {
                            free(old_last_executed);
                        }
                        last_executed_minute = calloc(cached_count, sizeof(int));
                        if (last_executed_minute) {
                            for (size_t i = 0; i < cached_count; i++) {
                                last_executed_minute[i] = -1;
                            }
                        }
                    }
                }
            } else if (old_last_executed) {
                // Si le rechargement a échoué, restaurer
                last_executed_minute = old_last_executed;
            }
        }
        
        // Vérifier les tâches uniquement à la seconde 0 de chaque minute
        // Éviter de vérifier plusieurs fois dans la même minute
        if (tm_now.tm_sec == 0) {
            // Calculer un identifiant unique pour la minute actuelle
            int current_minute_id = tm_now.tm_year * 525600 + tm_now.tm_mon * 43200 + 
                                    tm_now.tm_mday * 1440 + tm_now.tm_hour * 60 + tm_now.tm_min;
            
            // Vérifier seulement si on n'a pas déjà vérifié cette minute
            if (last_checked_minute_id != current_minute_id) {
                last_checked_minute_id = current_minute_id;
                
                // Vérifier et exécuter les tâches
                if (cached_tasks) {
                    for (size_t i = 0; i < cached_count && !g_stop; ++i) {
                        // Éviter les doubles exécutions dans la même minute
                        if (last_executed_minute && last_executed_minute[i] == current_minute_id) {
                            continue;
                        }
                        
                        if (should_execute_task_simple(cached_tasks[i])) {
                            // Le timestamp d'exécution est exactement à la seconde 0
                            time_t exec_time = now;
                            
                            // Exécuter la tâche de manière asynchrone pour ne pas bloquer le thread
                            task_exec_params_t *params = malloc(sizeof(task_exec_params_t));
                            if (params) {
                                params->run_dir = run_dir;
                                params->task = cached_tasks[i];
                                params->exec_timestamp = exec_time;
                                
                                pthread_t exec_thread;
                                pthread_attr_t attr;
                                pthread_attr_init(&attr);
                                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                                if (pthread_create(&exec_thread, &attr, async_task_executor, params) != 0) {
                                    free(params);
                                }
                                pthread_attr_destroy(&attr);
                            }
                            
                            // Mémoriser l'exécution pour éviter les doubles exécutions
                            if (last_executed_minute) {
                                last_executed_minute[i] = current_minute_id;
                            }
                        }
                    }
                }
            }
        } else {
            // Réinitialiser le flag quand on n'est plus à la seconde 0
            // Cela permet de vérifier à nouveau à la prochaine seconde 0
            if (last_checked_minute_id >= 0) {
                last_checked_minute_id = -1;
            }
        }

        // Attendre 1ms avant la prochaine vérification (extrêmement fréquent pour meilleure précision avec valgrind)
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 1000000; // 1ms (extrêmement fréquent pour valgrind)
        nanosleep(&ts, NULL);
    }
    
    // Nettoyer
    if (cached_tasks) {
        free_task_array(cached_tasks, cached_count);
    }
    if (last_executed_minute) {
        free(last_executed_minute);
    }
    
    return NULL;
}

// Thread pour exécuter une tâche de manière asynchrone
static void* async_task_executor(void *arg) {
    task_exec_params_t *params = (task_exec_params_t *)arg;
    if (!params) {
        return NULL;
    }
    
    execute_task_with_timestamp(params->run_dir, params->task, params->exec_timestamp);
    free(params);
    return NULL;
}

int execute_task(const char *run_dir, const task_t *task) {
    return execute_task_with_timestamp(run_dir, task, time(NULL));
}

int execute_task_with_timestamp(const char *run_dir, const task_t *task, time_t exec_timestamp) {
    if (!run_dir || !task || !task->cmd) {
        DEBUG_LOG("[DEBUG] execute_task_with_timestamp: invalid parameters (run_dir=%p, task=%p)\n", 
                  (void*)run_dir, (void*)task);
        errno = EINVAL;
        return -1;
    }

    DEBUG_LOG("[DEBUG] Executing task %lu at timestamp %ld\n", task->taskid, (long)exec_timestamp);

    char *stdout_buf = NULL;
    char *stderr_buf = NULL;
    size_t stdout_len = 0;
    size_t stderr_len = 0;
    uint16_t exitcode = 255;
    int rc;

    if (task->cmd->nb_cmds > 0) {
        // Détecter le type de combinaison
        uint16_t cmd_type = task->cmd->type;
        uint16_t type_pl = type_from_str("PL");
        uint16_t type_if = type_from_str("IF");
        
        if (cmd_type == type_pl) {
            DEBUG_LOG("[DEBUG] Task %lu: executing pipeline command (%u sub-commands)\n", 
                      task->taskid, task->cmd->nb_cmds);
            rc = execute_pipeline_command(task->cmd, &stdout_buf, &stdout_len,
                                          &stderr_buf, &stderr_len, &exitcode);
        } else if (cmd_type == type_if) {
            DEBUG_LOG("[DEBUG] Task %lu: executing conditional command (%u sub-commands)\n", 
                      task->taskid, task->cmd->nb_cmds);
            rc = execute_conditional_command(task->cmd, &stdout_buf, &stdout_len,
                                            &stderr_buf, &stderr_len, &exitcode);
        } else {
            // Par défaut, traiter comme séquence (SQ ou autre type non reconnu)
            DEBUG_LOG("[DEBUG] Task %lu: executing sequence command (%u sub-commands, type=0x%04x)\n", 
                      task->taskid, task->cmd->nb_cmds, cmd_type);
            rc = execute_sequence_command(task->cmd, &stdout_buf, &stdout_len,
                                          &stderr_buf, &stderr_len, &exitcode);
        }
    } else {
        DEBUG_LOG("[DEBUG] Task %lu: executing simple command\n", task->taskid);
        rc = execute_simple_command(task->cmd, &stdout_buf, &stdout_len,
                                    &stderr_buf, &stderr_len, &exitcode);
    }
    if (rc < 0) {
        DEBUG_LOG("[DEBUG] Task %lu: execution failed: %s\n", task->taskid, strerror(errno));
        free(stdout_buf);
        free(stderr_buf);
        return -1;
    }

    DEBUG_LOG("[DEBUG] Task %lu: execution completed (exitcode=%u, stdout_len=%zu, stderr_len=%zu)\n",
              task->taskid, exitcode, stdout_len, stderr_len);

    // Utiliser le timestamp passé en paramètre (celui de la vérification à la seconde 0)
    int64_t timestamp = (int64_t)exec_timestamp;
    if (append_execution_log(run_dir, task->taskid, timestamp, exitcode) < 0) {
        DEBUG_LOG("[DEBUG] Task %lu: failed to append execution log: %s\n", task->taskid, strerror(errno));
    }
    if (save_stdout(run_dir, task->taskid, stdout_buf, stdout_len) < 0) {
        DEBUG_LOG("[DEBUG] Task %lu: failed to save stdout: %s\n", task->taskid, strerror(errno));
    }
    if (save_stderr(run_dir, task->taskid, stderr_buf, stderr_len) < 0) {
        DEBUG_LOG("[DEBUG] Task %lu: failed to save stderr: %s\n", task->taskid, strerror(errno));
    }

    free(stdout_buf);
    free(stderr_buf);
    DEBUG_LOG("[DEBUG] Task %lu: execution finished successfully\n", task->taskid);
    return 0;
}

static void handle_request(const char *run_dir, int request_fd, int *reply_fd_ptr) {
    int reply_fd = *reply_fd_ptr;
    request_t *req = NULL;
    if (receive_request(request_fd, &req) < 0) {
        // Si l'erreur est due à une déconnexion du client ou à une fin de fichier, c'est normal
        // Ne pas afficher d'erreur dans ce cas pour éviter le spam
        // EAGAIN/EWOULDBLOCK : pas de données disponibles (ne devrait pas arriver avec select)
        // EPIPE : pipe fermé par l'autre extrémité
        // EBADF : descripteur invalide
        // EBADMSG : message invalide (fin de fichier)
        if (errno != EPIPE && errno != EBADF && errno != EBADMSG && 
            errno != EAGAIN && errno != EWOULDBLOCK) {
            // Afficher l'erreur seulement si ce n'est pas une erreur "normale"
            DEBUG_LOG("[DEBUG] receive_request error: %s\n", strerror(errno));
        }
        return;
    }

    response_t *resp = calloc(1, sizeof(response_t));
    if (!resp) {
        perror("malloc response");
        free_request(req);
        return;
    }

    DEBUG_LOG("[DEBUG] handle_request: received opcode=%d (taskid=%lu)\n", 
              req->opcode, (req->opcode == OPCODE_TIMES_EXITCODES || req->opcode == OPCODE_STDOUT || req->opcode == OPCODE_STDERR) ? req->u.query.taskid : 0);
    
    switch (req->opcode) {
    case OPCODE_LIST: {
        DEBUG_LOG("[DEBUG] handle_request: processing LIST request\n");
        task_t **tasks = NULL;
        size_t count = 0;
        if (load_all_tasks(run_dir, &tasks, &count) < 0) {
            DEBUG_LOG("[DEBUG] handle_request: LIST failed to load tasks: %s\n", strerror(errno));
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
        } else {
            DEBUG_LOG("[DEBUG] handle_request: LIST found %zu tasks\n", count);
            resp->anstype = ANSTYPE_OK;
            resp->u.list_ok.nbtasks = (uint32_t)count;
            resp->u.list_ok.tasks = tasks;
            resp->opcode_used = OPCODE_LIST;
        }
        break;
    }
    case OPCODE_TIMES_EXITCODES: {
        DEBUG_LOG("[DEBUG] handle_request: processing TIMES_EXITCODES for taskid=%lu\n", req->u.query.taskid);
        char path[MAX_PATH_LEN];
        struct stat st;
        if (build_task_dir_path(path, sizeof(path), run_dir, req->u.query.taskid) < 0 ||
            stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
            DEBUG_LOG("[DEBUG] handle_request: TIMES_EXITCODES task %lu not found\n", req->u.query.taskid);
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        int64_t *timestamps = NULL;
        uint16_t *exitcodes = NULL;
        uint32_t nbruns = 0;
        if (read_execution_logs(run_dir, req->u.query.taskid, &timestamps, &exitcodes, &nbruns) < 0) {
            DEBUG_LOG("[DEBUG] handle_request: TIMES_EXITCODES failed to read logs for task %lu: %s\n", 
                      req->u.query.taskid, strerror(errno));
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        DEBUG_LOG("[DEBUG] handle_request: TIMES_EXITCODES found %u runs for task %lu\n", 
                  nbruns, req->u.query.taskid);
        // Si nbruns == 0, on retourne quand même OK avec une liste vide (pas d'erreur)
        resp->anstype = ANSTYPE_OK;
        resp->u.times_exitcodes_ok.nbruns = nbruns;
        // Si nbruns == 0, timestamps et exitcodes peuvent être NULL, on les initialise à NULL
        resp->u.times_exitcodes_ok.timestamps = timestamps;
        resp->u.times_exitcodes_ok.exitcodes = exitcodes;
        resp->opcode_used = OPCODE_TIMES_EXITCODES;
        break;
    }
    case OPCODE_STDOUT:
    case OPCODE_STDERR: {
        char path[MAX_PATH_LEN];
        struct stat st;
        if (build_task_dir_path(path, sizeof(path), run_dir, req->u.query.taskid) < 0 ||
            stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        char *output = NULL;
        size_t len = 0;
        int rc = (req->opcode == OPCODE_STDOUT)
                 ? read_stdout(run_dir, req->u.query.taskid, &output, &len)
                 : read_stderr(run_dir, req->u.query.taskid, &output, &len);
        if (rc < 0) {
            // Vérifier si c'est parce que la tâche n'a jamais été exécutée
            // read_stdout/read_stderr retourne -1 si le fichier n'existe pas
            // On doit distinguer "tâche non trouvée" de "tâche jamais exécutée"
            if (errno == ENOENT) {
                // Le fichier n'existe pas = tâche jamais exécutée
                resp->anstype = ANSTYPE_ERROR;
                resp->u.error.errcode = ERRCODE_NOT_RUN;
            } else {
                // Autre erreur = tâche non trouvée
                resp->anstype = ANSTYPE_ERROR;
                resp->u.error.errcode = ERRCODE_NOT_FOUND;
            }
            break;
        }
        resp->anstype = ANSTYPE_OK;
        // Si le fichier est vide (len == 0), output peut être NULL, c'est correct
        // Le client vérifie len == 0 avant d'afficher
        resp->u.output_ok.output = output;
        resp->u.output_ok.len = len;
        resp->opcode_used = req->opcode; // OPCODE_STDOUT ou OPCODE_STDERR
        break;
    }
    case OPCODE_TERMINATE:
        DEBUG_LOG("[DEBUG] Handling TERMINATE request\n");
        resp->anstype = ANSTYPE_OK;
        resp->opcode_used = OPCODE_TERMINATE;
        g_stop = 1;
        DEBUG_LOG("[DEBUG] TERMINATE: g_stop set to 1, sending response\n");
        break;
    case OPCODE_CREATE: {
        DEBUG_LOG("[DEBUG] handle_request: processing CREATE request\n");
        // Créer une commande simple à partir des arguments
        command_t *cmd = create_simple_command("SI", req->u.create.argc, req->u.create.argv);
        if (!cmd) {
            DEBUG_LOG("[DEBUG] handle_request: CREATE failed to create command: %s\n", strerror(errno));
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        // Générer un ID unique
        uint64_t new_taskid = generate_task_id(run_dir);
        // Créer la tâche
        task_t *task = calloc(1, sizeof(task_t));
        if (!task) {
            free_command(cmd);
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        task->taskid = new_taskid;
        task->timing = req->u.create.timing;
        task->cmd = cmd;
        // Sauvegarder la tâche
        if (save_task_to_dir(run_dir, task) < 0) {
            DEBUG_LOG("[DEBUG] handle_request: CREATE failed to save task: %s\n", strerror(errno));
            free_task(task);
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        DEBUG_LOG("[DEBUG] handle_request: CREATE created task %lu\n", new_taskid);
        resp->anstype = ANSTYPE_OK;
        resp->u.create_ok.taskid = new_taskid;
        resp->opcode_used = OPCODE_CREATE;
        free_task(task); // La tâche est sauvegardée, on peut libérer
        break;
    }
    case OPCODE_REMOVE: {
        DEBUG_LOG("[DEBUG] handle_request: processing REMOVE request for taskid=%lu\n", req->u.query.taskid);
        if (remove_task(run_dir, req->u.query.taskid) < 0) {
            DEBUG_LOG("[DEBUG] handle_request: REMOVE failed for task %lu: %s\n", 
                      req->u.query.taskid, strerror(errno));
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        DEBUG_LOG("[DEBUG] handle_request: REMOVE removed task %lu\n", req->u.query.taskid);
        resp->anstype = ANSTYPE_OK;
        resp->opcode_used = OPCODE_REMOVE;
        break;
    }
    case OPCODE_COMBINE: {
        DEBUG_LOG("[DEBUG] handle_request: processing COMBINE request for %u tasks\n", req->u.combine.nbtasks);
        uint64_t new_taskid;
        if (combine_tasks(run_dir, req->u.combine.taskids, req->u.combine.nbtasks, 
                         req->u.combine.type, &req->u.combine.timing, &new_taskid) < 0) {
            DEBUG_LOG("[DEBUG] handle_request: COMBINE failed: %s\n", strerror(errno));
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        DEBUG_LOG("[DEBUG] handle_request: COMBINE created task %lu\n", new_taskid);
        resp->anstype = ANSTYPE_OK;
        resp->u.create_ok.taskid = new_taskid;
        resp->opcode_used = OPCODE_COMBINE;
        break;
    }
    default:
        resp->anstype = ANSTYPE_ERROR;
        resp->u.error.errcode = ERRCODE_NOT_FOUND;
        break;
    }

    // Rouvrir le tube de réponse si nécessaire (il a été fermé après la requête précédente)
    if (reply_fd < 0) {
        char reply_path[1024];
        int len = snprintf(reply_path, sizeof(reply_path), "%s/pipes/erraid-reply-pipe", run_dir);
        if (len >= 0 && len < (int)sizeof(reply_path)) {
            // Essayer d'ouvrir en O_WRONLY (écriture seule) avec retry
            // Le client doit avoir ouvert le tube en lecture avant que nous puissions l'ouvrir en écriture
            int attempts = 0;
            const int max_attempts = 1000; // 1000 * 1ms = 1 seconde
            while (attempts < max_attempts) {
                reply_fd = open(reply_path, O_WRONLY | O_NONBLOCK);
                if (reply_fd >= 0) {
                    // Remettre en mode bloquant pour l'écriture
                    int flags = fcntl(reply_fd, F_GETFL);
                    fcntl(reply_fd, F_SETFL, flags & ~O_NONBLOCK);
                    DEBUG_LOG("[DEBUG] Reply pipe reopened: fd=%d (after %d attempts)\n", reply_fd, attempts + 1);
                    *reply_fd_ptr = reply_fd;
                    break;
                }
                if (errno != ENXIO) {
                    // Erreur autre que "pas de lecteur"
                    DEBUG_LOG("[DEBUG] Failed to reopen reply pipe: %s\n", strerror(errno));
                    perror("open reply pipe");
                    free_request(req);
                    free_response(resp);
                    return;
                }
                // ENXIO = pas encore de lecteur, attendre 1ms et réessayer
                struct timespec ts = {0, 1000000}; // 1ms
                nanosleep(&ts, NULL);
                attempts++;
            }
            if (reply_fd < 0) {
                DEBUG_LOG("[DEBUG] Timeout waiting for client to open reply pipe after %d attempts\n", max_attempts);
                free_request(req);
                free_response(resp);
                return;
            }
        }
    }
    
    DEBUG_LOG("[DEBUG] Sending response (opcode=%d, anstype=%d, reply_fd=%d)\n", 
              req->opcode, resp->anstype, reply_fd);
    if (send_response(reply_fd, resp) < 0) {
        DEBUG_LOG("[DEBUG] send_response failed: %s\n", strerror(errno));
        perror("send_response");
    } else {
        DEBUG_LOG("[DEBUG] Response sent successfully\n");
    }
    
    // Toujours fermer le tube de réponse après avoir tenté d'envoyer la réponse
    // pour indiquer au client que la réponse est complète (EOF)
    // Le close() envoie automatiquement l'EOF au lecteur
    DEBUG_LOG("[DEBUG] Closing reply_fd\n");
    close(reply_fd);
    *reply_fd_ptr = -1; // Indiquer que le tube est fermé

    free_request(req);
    free_response(resp);
}

void daemon_loop(const char *run_dir, int request_fd, int reply_fd) {
    int maxfd = request_fd;
    
    DEBUG_LOG("[DEBUG] daemon_loop started, request_fd=%d, reply_fd=%d\n", request_fd, reply_fd);
    
    // Le thread d'exécution des tâches est déjà démarré dans main()
    // Thread principal : gère uniquement les requêtes client avec select() (jalon 2)
    int daemon_iterations = 0;
    const int MAX_DAEMON_ITERATIONS = 1000000; // Limite pour éviter boucle infinie
    while (!g_stop && daemon_iterations < MAX_DAEMON_ITERATIONS) {
        daemon_iterations++;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(request_fd, &readfds);

        // Timeout plus long maintenant qu'on a un thread séparé pour les tâches
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(maxfd + 1, &readfds, NULL, NULL, &tv);

        if (ret < 0) {
            if (errno == EINTR) {
                DEBUG_LOG("[DEBUG] select interrupted, continuing...\n");
                continue;
            }
            // Ne pas arrêter le démon sur une erreur select, juste logger
            DEBUG_LOG("[DEBUG] select error: %s\n", strerror(errno));
            continue; // Continuer au lieu de break
        }

        // Une requête client disponible
        if (ret > 0 && FD_ISSET(request_fd, &readfds)) {
            // Vérifier que le fd est toujours valide avant de lire
            if (request_fd < 0) {
                DEBUG_LOG("[DEBUG] request_fd invalid, breaking\n");
                break;
            }
            
            DEBUG_LOG("[DEBUG] Processing client request\n");
            
            // Traiter la requête (qui rouvrira le tube de réponse si nécessaire)
            handle_request(run_dir, request_fd, &reply_fd);
        }
        // ret == 0 signifie timeout, c'est normal, on continue
    }
    
    if (daemon_iterations >= MAX_DAEMON_ITERATIONS) {
        DEBUG_LOG("[DEBUG] ⚠️ Daemon loop stopped: max iterations reached (%d)\n", MAX_DAEMON_ITERATIONS);
    }
    DEBUG_LOG("[DEBUG] daemon_loop exiting (g_stop=%d, iterations=%d)\n", (int)g_stop, daemon_iterations);
}


static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-R <run_dir>] [-P <pipes_dir>] [-F] [-d]\n", prog);
    fprintf(stderr, "  -R <run_dir>  Répertoire d'exécution (défaut: /tmp/$USER/erraid)\n");
    fprintf(stderr, "  -P <pipes_dir> Répertoire des pipes (défaut: <run_dir>/pipes)\n");
    fprintf(stderr, "  -F            Exécution en avant-plan (sans démonisation)\n");
    fprintf(stderr, "  -d            Activer les logs de debug\n");
    fprintf(stderr, "  -h, --help    Afficher cette aide\n");
}

int main(int argc, char **argv) {
    const char *run_dir = NULL;
    const char *pipes_dir_arg = NULL;
    char default_run_dir[512];
    char pipes_dir[512];
    int opt;
    int request_fd = -1;
    int reply_fd = -1;
    int foreground __attribute__((unused)) = 0;  // Flag pour -F (exécution en avant-plan)

    // Vérifier --help et -h avant getopt
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    // Support des deux formats : ancien (-r, -p) et nouveau (-R, -P) pour compatibilité
    while ((opt = getopt(argc, argv, "R:r:P:p:Fhd")) != -1) {
        switch (opt) {
        case 'R':
        case 'r':  // Support ancien format pour compatibilité avec les tests
            run_dir = optarg;
            break;
        case 'P':
        case 'p':  // Support ancien format pour compatibilité avec les tests
            pipes_dir_arg = optarg;
            break;
        case 'F':
            foreground = 1;
            break;
        case 'd':
            g_debug_logs = 1;
            break;
        case 'h':
            usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Si run_dir n'est pas fourni, utiliser la valeur par défaut
    if (!run_dir) {
        const char *user = getenv("USER");
        if (!user) {
            fprintf(stderr, "Erreur: variable d'environnement USER non définie\n");
            return EXIT_FAILURE;
        }
        int len = snprintf(default_run_dir, sizeof(default_run_dir), "/tmp/%s/erraid", user);
        if (len < 0 || len >= (int)sizeof(default_run_dir)) {
            fprintf(stderr, "Erreur: chemin par défaut trop long\n");
            return EXIT_FAILURE;
        }
        run_dir = default_run_dir;
    }

    // Si pipes_dir n'est pas fourni, utiliser <run_dir>/pipes par défaut
    if (!pipes_dir_arg) {
        if (snprintf(pipes_dir, sizeof(pipes_dir), "%s/pipes", run_dir) < 0 || strlen(pipes_dir) >= sizeof(pipes_dir)) {
            fprintf(stderr, "Erreur: chemin pipes trop long\n");
            return EXIT_FAILURE;
        }
    } else {
        if (snprintf(pipes_dir, sizeof(pipes_dir), "%s", pipes_dir_arg) < 0 || strlen(pipes_dir) >= sizeof(pipes_dir)) {
            fprintf(stderr, "Erreur: chemin pipes trop long\n");
            return EXIT_FAILURE;
        }
    }

    if (init_task_directory(run_dir) < 0) {
        perror("init_task_directory");
        return EXIT_FAILURE;
    }

    if (mkdir(pipes_dir, 0777) < 0 && errno != EEXIST) {
        perror("mkdir pipes_dir");
        return EXIT_FAILURE;
    }

    if (init_pipes(pipes_dir) < 0) {
        perror("init_pipes");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Démarrer le thread d'exécution des tâches avant d'ouvrir les pipes
    pthread_t task_thread;
    if (pthread_create(&task_thread, NULL, task_execution_thread, (void *)run_dir) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if (open_pipes_daemon(pipes_dir, &request_fd, &reply_fd) < 0) {
        perror("open_pipes_daemon");
        g_stop = 1;
        pthread_join(task_thread, NULL);
        return EXIT_FAILURE;
    }

    // Fermer immédiatement le reply_fd initial pour éviter les problèmes de fermeture du tube
    // Le tube sera rouvert pour chaque requête dans handle_request()
    if (reply_fd >= 0) {
        close(reply_fd);
        reply_fd = -1;
    }

    daemon_loop(run_dir, request_fd, reply_fd);
    
    // Attendre que le thread se termine
    pthread_join(task_thread, NULL);

    if (request_fd >= 0) close(request_fd);
    if (reply_fd >= 0) close(reply_fd);
    return EXIT_SUCCESS;
}
