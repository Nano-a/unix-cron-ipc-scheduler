#ifndef TASK_TREE_H
#define TASK_TREE_H

#include <stddef.h>
#include <stdint.h>

#include "serialization.h"

#define MAX_PATH_LEN 512

typedef struct {
    uint64_t taskid;
    timing_t timing;
    command_t *cmd;
} task_t;

int init_task_directory(const char *orig_path);
int build_task_dir_path(char *path, size_t path_size, const char *run_dir, uint64_t taskid);
int build_task_path(char *path, size_t path_size, const char *run_dir, uint64_t taskid, const char *filename);

int load_task_from_dir(const char *run_dir, uint64_t taskid, task_t **task);
int save_task_to_dir(const char *run_dir, const task_t *task);
void free_task(task_t *task);

int append_execution_log(const char *run_dir, uint64_t taskid, int64_t timestamp, uint16_t exitcode);
int save_stdout(const char *run_dir, uint64_t taskid, const char *output, size_t len);
int save_stderr(const char *run_dir, uint64_t taskid, const char *output, size_t len);

#endif // TASK_TREE_H
