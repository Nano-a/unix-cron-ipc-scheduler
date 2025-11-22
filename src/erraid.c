#define _POSIX_C_SOURCE 200809L

#include "execution.h"
#include "task_tree.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t g_stop = 0;

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

int should_execute_task(const task_t *task) {
    if (!task) {
        return 0;
    }
    time_t now = time(NULL);
    struct tm tm_now;
    if (localtime_r(&now, &tm_now) == NULL) {
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

void daemon_loop(const char *run_dir) {
    while (!g_stop) {
        task_t **tasks = NULL;
        size_t count = 0;

        if (load_all_tasks(run_dir, &tasks, &count) == 0) {
            for (size_t i = 0; i < count && !g_stop; ++i) {
                if (should_execute_task(tasks[i])) {
                    execute_task(run_dir, tasks[i]);
                }
            }
            free_task_array(tasks, count);
        }

        sleep(1);
    }
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s -r <run_dir>\n", prog);
}

int main(int argc, char **argv) {
    const char *run_dir = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "r:")) != -1) {
        switch (opt) {
        case 'r':
            run_dir = optarg;
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!run_dir) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (init_task_directory(run_dir) < 0) {
        perror("init_task_directory");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    daemon_loop(run_dir);
    return EXIT_SUCCESS;
}
