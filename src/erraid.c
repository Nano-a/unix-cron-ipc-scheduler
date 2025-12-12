#define _POSIX_C_SOURCE 200809L

#include "execution.h"
#include "task_tree.h"

#include "protocol.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
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
static int g_debug_logs = 0; // 0 = désactivé, 1 = activé

// Macro pour logs de debug
#define DEBUG_LOG(...) do { if (g_debug_logs) fprintf(stderr, __VA_ARGS__); } while(0)

// Forward declarations
static int should_execute_task_simple(const task_t *task);
static void* task_execution_thread(void *arg);
int execute_task(const char *run_dir, const task_t *task);

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
        errno = ENAMETOOLONG;
        return -1;
    }

    DIR *dir = opendir(tasks_dir);
    if (!dir) {
        if (errno == ENOENT) {
            *tasks_out = NULL;
            *count_out = 0;
            return 0;
        }
        return -1;
    }

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

    *tasks_out = tasks;
    *count_out = count;
    return 0;
}

// Structure pour mémoriser la dernière minute d'exécution de chaque tâche
typedef struct {
    uint64_t taskid;
    int last_executed_minute; // -1 si jamais exécutée
} task_execution_memory_t;

// Version simplifiée pour le thread (comme jalon 1)
static int should_execute_task_simple(const task_t *task) {
    if (!task) {
        return 0;
    }
    time_t now = time(NULL);
    struct tm tm_now;
    if (localtime_r(&now, &tm_now) == NULL) {
        return 0;
    }

    // DEBUG: Log chaque vérification
    DEBUG_LOG("[DEBUG] should_execute_task_simple: taskid=%lu, time=%02d:%02d:%02d, tm_sec=%d\n",
              (unsigned long)task->taskid, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, tm_now.tm_sec);

    // Exécution uniquement au début de la minute (comme cron)
    if (tm_now.tm_sec != 0) {
        DEBUG_LOG("[DEBUG] Rejected: tm_sec != 0 (tm_sec=%d)\n", tm_now.tm_sec);
        return 0;
    }

    if (!(task->timing.minutes & (1ULL << tm_now.tm_min))) {
        DEBUG_LOG("[DEBUG] Rejected: minutes bit not set (minute=%d)\n", tm_now.tm_min);
        return 0;
    }
    if (!(task->timing.hours & (1U << tm_now.tm_hour))) {
        DEBUG_LOG("[DEBUG] Rejected: hours bit not set (hour=%d)\n", tm_now.tm_hour);
        return 0;
    }
    if (!(task->timing.daysofweek & (1U << tm_now.tm_wday))) {
        DEBUG_LOG("[DEBUG] Rejected: daysofweek bit not set (wday=%d)\n", tm_now.tm_wday);
        return 0;
    }
    
    DEBUG_LOG("[DEBUG] ✅ Task %lu SHOULD EXECUTE at %02d:%02d:%02d\n",
              (unsigned long)task->taskid, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
    return 1;
}

// Thread pour l'exécution périodique des tâches (exactement comme jalon 1)
static void* task_execution_thread(void *arg) {
    const char *run_dir = (const char *)arg;
    time_t start_time = time(NULL);
    struct tm tm_start;
    localtime_r(&start_time, &tm_start);
    
    DEBUG_LOG("[DEBUG] 🚀 Task execution thread started at %02d:%02d:%02d\n",
              tm_start.tm_hour, tm_start.tm_min, tm_start.tm_sec);
    
    int iteration = 0;
    // Exactement comme dans le jalon 1 : boucle simple avec sleep(1)
    while (!g_stop) {
        iteration++;
        time_t before_load = time(NULL);
        struct tm tm_before;
        localtime_r(&before_load, &tm_before);
        
        task_t **tasks = NULL;
        size_t count = 0;

        DEBUG_LOG("[DEBUG] 🔄 Iteration %d: Loading tasks at %02d:%02d:%02d\n",
                  iteration, tm_before.tm_hour, tm_before.tm_min, tm_before.tm_sec);

        if (load_all_tasks(run_dir, &tasks, &count) == 0) {
            time_t after_load = time(NULL);
            struct tm tm_after;
            localtime_r(&after_load, &tm_after);
            
            DEBUG_LOG("[DEBUG] ✅ Loaded %zu tasks in %ld seconds (loaded at %02d:%02d:%02d)\n",
                      count, after_load - before_load, tm_after.tm_hour, tm_after.tm_min, tm_after.tm_sec);
            
            for (size_t i = 0; i < count && !g_stop; ++i) {
                if (should_execute_task_simple(tasks[i])) {
                    time_t exec_time = time(NULL);
                    struct tm tm_exec;
                    localtime_r(&exec_time, &tm_exec);
                    DEBUG_LOG("[DEBUG] 🎯 EXECUTING task %lu at %02d:%02d:%02d\n",
                              (unsigned long)tasks[i]->taskid, tm_exec.tm_hour, tm_exec.tm_min, tm_exec.tm_sec);
                    execute_task(run_dir, tasks[i]);
                    time_t after_exec = time(NULL);
                    DEBUG_LOG("[DEBUG] ✅ Task %lu executed in %ld seconds\n",
                              (unsigned long)tasks[i]->taskid, after_exec - exec_time);
                }
            }
            free_task_array(tasks, count);
        } else {
            DEBUG_LOG("[DEBUG] ❌ Failed to load tasks\n");
        }

        time_t before_sleep = time(NULL);
        struct tm tm_sleep;
        localtime_r(&before_sleep, &tm_sleep);
        DEBUG_LOG("[DEBUG] 😴 Sleeping at %02d:%02d:%02d\n",
                  tm_sleep.tm_hour, tm_sleep.tm_min, tm_sleep.tm_sec);
        sleep(1); // Exactement comme dans le jalon 1
        time_t after_sleep = time(NULL);
        DEBUG_LOG("[DEBUG] ⏰ Woke up at %02d:%02d:%02d (slept for %ld seconds)\n",
                  tm_sleep.tm_hour, tm_sleep.tm_min, tm_sleep.tm_sec, after_sleep - before_sleep);
    }
    
    DEBUG_LOG("[DEBUG] 🛑 Task execution thread stopping\n");
    return NULL;
}

int execute_task(const char *run_dir, const task_t *task) {
    if (!run_dir || !task || !task->cmd) {
        errno = EINVAL;
        return -1;
    }

    char *stdout_buf = NULL;
    char *stderr_buf = NULL;
    size_t stdout_len = 0;
    size_t stderr_len = 0;
    uint16_t exitcode = 255;
    int rc;

    if (task->cmd->nb_cmds > 0) {
        rc = execute_sequence_command(task->cmd, &stdout_buf, &stdout_len,
                                      &stderr_buf, &stderr_len, &exitcode);
    } else {
        rc = execute_simple_command(task->cmd, &stdout_buf, &stdout_len,
                                    &stderr_buf, &stderr_len, &exitcode);
    }
    if (rc < 0) {
        free(stdout_buf);
        free(stderr_buf);
        return -1;
    }

    int64_t timestamp = (int64_t)time(NULL);
    append_execution_log(run_dir, task->taskid, timestamp, exitcode);
    save_stdout(run_dir, task->taskid, stdout_buf, stdout_len);
    save_stderr(run_dir, task->taskid, stderr_buf, stderr_len);

    free(stdout_buf);
    free(stderr_buf);
    return 0;
}

static void handle_request(const char *run_dir, int request_fd, int reply_fd) {
    request_t *req = NULL;
    if (receive_request(request_fd, &req) < 0) {
        perror("receive_request");
        return;
    }

    response_t *resp = calloc(1, sizeof(response_t));
    if (!resp) {
        perror("malloc response");
        free_request(req);
        return;
    }

    switch (req->opcode) {
    case OPCODE_LIST: {
        task_t **tasks = NULL;
        size_t count = 0;
        if (load_all_tasks(run_dir, &tasks, &count) < 0) {
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
        } else {
            resp->anstype = ANSTYPE_OK;
            resp->u.list_ok.nbtasks = (uint32_t)count;
            resp->u.list_ok.tasks = tasks;
        }
        break;
    }
    case OPCODE_TIMES_EXITCODES: {
        char path[MAX_PATH_LEN];
        struct stat st;
        if (build_task_dir_path(path, sizeof(path), run_dir, req->u.query.taskid) < 0 ||
            stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        int64_t *timestamps = NULL;
        uint16_t *exitcodes = NULL;
        uint32_t nbruns = 0;
        if (read_execution_logs(run_dir, req->u.query.taskid, &timestamps, &exitcodes, &nbruns) < 0) {
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        if (nbruns == 0) {
            free(timestamps);
            free(exitcodes);
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_RUN;
            break;
        }
        resp->anstype = ANSTYPE_OK;
        resp->u.times_exitcodes_ok.nbruns = nbruns;
        resp->u.times_exitcodes_ok.timestamps = timestamps;
        resp->u.times_exitcodes_ok.exitcodes = exitcodes;
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
            resp->anstype = ANSTYPE_ERROR;
            resp->u.error.errcode = ERRCODE_NOT_FOUND;
            break;
        }
        resp->anstype = ANSTYPE_OK;
        resp->u.output_ok.output = output;
        resp->u.output_ok.len = len;
        break;
    }
    case OPCODE_TERMINATE:
        resp->anstype = ANSTYPE_OK;
        g_stop = 1;
        break;
    default:
        resp->anstype = ANSTYPE_ERROR;
        resp->u.error.errcode = ERRCODE_NOT_FOUND;
        break;
    }

    if (send_response(reply_fd, resp) < 0) {
        perror("send_response");
    }

    free_request(req);
    free_response(resp);
}

void daemon_loop(const char *run_dir, int request_fd, int reply_fd) {
    int maxfd = request_fd;
    pthread_t task_thread;
    
    // Créer un thread séparé pour l'exécution des tâches (comme jalon 1)
    // Cela garantit que les tâches sont vérifiées toutes les secondes avec sleep(1)
    if (pthread_create(&task_thread, NULL, task_execution_thread, (void *)run_dir) != 0) {
        perror("pthread_create");
        return;
    }
    
    // Thread principal : gère uniquement les requêtes client avec select() (jalon 2)
    while (!g_stop) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(request_fd, &readfds);

        // Timeout plus long maintenant qu'on a un thread séparé pour les tâches
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(maxfd + 1, &readfds, NULL, NULL, &tv);

        if (ret < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        // Une requête client disponible
        if (ret > 0 && FD_ISSET(request_fd, &readfds)) {
            handle_request(run_dir, request_fd, reply_fd);
        }
    }
    
    // Attendre que le thread se termine
    pthread_join(task_thread, NULL);
}


static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-r <run_dir>] [-d]\n", prog);
    fprintf(stderr, "  -r <run_dir>  Répertoire d'exécution (défaut: /tmp/$USER/erraid)\n");
    fprintf(stderr, "  -d            Activer les logs de debug\n");
    fprintf(stderr, "  -h, --help    Afficher cette aide\n");
}

int main(int argc, char **argv) {
    const char *run_dir = NULL;
    char default_run_dir[512];
    char pipes_dir[512];
    int opt;
    int request_fd = -1;
    int reply_fd = -1;

    // Vérifier --help et -h avant getopt
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    while ((opt = getopt(argc, argv, "r:hd")) != -1) {
        switch (opt) {
        case 'r':
            run_dir = optarg;
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

    if (snprintf(pipes_dir, sizeof(pipes_dir), "%s/pipes", run_dir) < 0 || strlen(pipes_dir) >= sizeof(pipes_dir)) {
        fprintf(stderr, "Erreur: chemin pipes trop long\n");
        return EXIT_FAILURE;
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

    if (open_pipes_daemon(pipes_dir, &request_fd, &reply_fd) < 0) {
        perror("open_pipes_daemon");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    daemon_loop(run_dir, request_fd, reply_fd);

    if (request_fd >= 0) close(request_fd);
    if (reply_fd >= 0) close(reply_fd);
    return EXIT_SUCCESS;
}
