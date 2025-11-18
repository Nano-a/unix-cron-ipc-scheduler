#define _POSIX_C_SOURCE 200809L

#include "execution.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} buffer_t;

static void buffer_init(buffer_t *buf) {
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static void buffer_free(buffer_t *buf) {
    free(buf->data);
    buffer_init(buf);
}

static int buffer_reserve(buffer_t *buf, size_t min_cap) {
    if (min_cap <= buf->cap) {
        return 0;
    }
    size_t new_cap = buf->cap ? buf->cap : 1024;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }
    char *tmp = realloc(buf->data, new_cap);
    if (!tmp) {
        return -1;
    }
    buf->data = tmp;
    buf->cap = new_cap;
    return 0;
}

static int buffer_append(buffer_t *buf, const char *src, size_t len) {
    if (len == 0) {
        return 0;
    }
    if (buffer_reserve(buf, buf->len + len + 1) < 0) {
        return -1;
    }
    memcpy(buf->data + buf->len, src, len);
    buf->len += len;
    buf->data[buf->len] = '\0';
    return 0;
}

static int read_fd_into_buffer(int fd, buffer_t *buf) {
    char tmp[4096];
    ssize_t rd;
    while ((rd = read(fd, tmp, sizeof(tmp))) > 0) {
        if (buffer_append(buf, tmp, (size_t)rd) < 0) {
            return -1;
        }
    }
    if (rd < 0) {
        return -1;
    }
    return 0;
}

static char **build_exec_argv(const command_t *cmd) {
    if (!cmd || cmd->argc == 0 || !cmd->argv) {
        errno = EINVAL;
        return NULL;
    }
    char **exec_argv = calloc((size_t)cmd->argc + 1, sizeof(char *));
    if (!exec_argv) {
        return NULL;
    }
    for (uint32_t i = 0; i < cmd->argc; ++i) {
        if (!cmd->argv[i] || cmd->argv[i][0] == '\0') {
            free(exec_argv);
            errno = EINVAL;
            return NULL;
        }
        exec_argv[i] = cmd->argv[i];
    }
    exec_argv[cmd->argc] = NULL;
    return exec_argv;
}

static void close_pair(int pipefd[2]) {
    close(pipefd[0]);
    close(pipefd[1]);
}

int execute_simple_command(const command_t *cmd,
                           char **stdout_buf, size_t *stdout_len,
                           char **stderr_buf, size_t *stderr_len,
                           uint16_t *exitcode) {
    if (!cmd || !stdout_buf || !stderr_buf || !stdout_len || !stderr_len || !exitcode) {
        errno = EINVAL;
        return -1;
    }
    if (cmd->nb_cmds > 0) {
        errno = EINVAL;
        return -1;
    }

    *stdout_buf = NULL;
    *stderr_buf = NULL;
    *stdout_len = 0;
    *stderr_len = 0;
    *exitcode = 255;

    char **exec_argv = build_exec_argv(cmd);
    if (!exec_argv) {
        return -1;
    }

    int stdout_pipe[2];
    int stderr_pipe[2];
    if (pipe(stdout_pipe) < 0) {
        free(exec_argv);
        return -1;
    }
    if (pipe(stderr_pipe) < 0) {
        close_pair(stdout_pipe);
        free(exec_argv);
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close_pair(stdout_pipe);
        close_pair(stderr_pipe);
        free(exec_argv);
        return -1;
    }

    if (pid == 0) {
        // Processus enfant
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        if (dup2(stdout_pipe[1], STDOUT_FILENO) < 0 ||
            dup2(stderr_pipe[1], STDERR_FILENO) < 0) {
            _exit(127);
        }
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        execvp(exec_argv[0], exec_argv);
        _exit(127);
    }

    // Processus parent
    free(exec_argv);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    buffer_t out_buf;
    buffer_t err_buf;
    buffer_init(&out_buf);
    buffer_init(&err_buf);

    int result = -1;
    if (read_fd_into_buffer(stdout_pipe[0], &out_buf) < 0) {
        goto cleanup;
    }
    if (read_fd_into_buffer(stderr_pipe[0], &err_buf) < 0) {
        goto cleanup;
    }

    close(stdout_pipe[0]);
    close(stderr_pipe[0]);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        goto cleanup;
    }

    if (WIFEXITED(status)) {
        *exitcode = (uint16_t)WEXITSTATUS(status);
    } else {
        *exitcode = 255;
    }

    if (out_buf.data && out_buf.len && out_buf.data[out_buf.len - 1] != '\0') {
        if (buffer_append(&out_buf, "", 1) < 0) {
            goto cleanup;
        }
    }
    if (err_buf.data && err_buf.len && err_buf.data[err_buf.len - 1] != '\0') {
        if (buffer_append(&err_buf, "", 1) < 0) {
            goto cleanup;
        }
    }

    *stdout_len = out_buf.len ? out_buf.len - 1 : 0;
    *stderr_len = err_buf.len ? err_buf.len - 1 : 0;
    *stdout_buf = out_buf.data;
    *stderr_buf = err_buf.data;
    out_buf.data = NULL;
    err_buf.data = NULL;
    result = 0;

cleanup:
    buffer_free(&out_buf);
    buffer_free(&err_buf);
    return result;
}

