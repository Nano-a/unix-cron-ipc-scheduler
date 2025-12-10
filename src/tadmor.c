// tadmor.c - client argument parsing + consultative requests (T2.5 + T2.6)
// Version modifiée pour T2.6 : implémentation des requêtes consultatives

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <inttypes.h>
#include <pwd.h>
#include <sys/types.h>
#include <time.h>

#include "protocol.h"
#include "serialization.h"
#include "task_tree.h" // pour task_t

// Helper
static void compute_default_run_dir(char *out, size_t outlen) {
    const char *user = getenv("USER");
    if (!user) {
        struct passwd *pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }
    snprintf(out, outlen, "/tmp/%s/erraid", user);
}

// Parse unsigned int in range
static int parse_uint_range(const char *s, unsigned long min, unsigned long max, unsigned long *out) {
    char *end;
    errno = 0;
    unsigned long v = strtoul(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    if (v < min || v > max) return -1;
    *out = v;
    return 0;
}

// Parse daysofweek list "0,1,2" -> bitmask
static int parse_daysofweek(const char *s, uint8_t *out_mask) {
    if (!s || !*s) { *out_mask = 0; return 0; }
    uint8_t mask = 0;
    char *tmp = strdup(s);
    if (!tmp) return -1;
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, ",", &saveptr);
    while (tok) {
        unsigned long d;
        if (parse_uint_range(tok, 0, 6, &d) != 0) { free(tmp); return -1; }
        mask |= (1u << d);
        tok = strtok_r(NULL, ",", &saveptr);
    }
    free(tmp);
    *out_mask = mask;
    return 0;
}

// Parse single taskid
static int parse_taskid(const char *s, uint64_t *out) {
    if (!s) return -1;
    char *end = NULL;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    *out = (uint64_t)v;
    return 0;
}

// Free request allocations that were dynamically created locally
static void free_request_allocs(request_t *req) {
    if (!req) return;
    if (req->opcode == OPCODE_CREATE) {
        if (req->u.create.argv) {
            for (uint32_t i = 0; i < req->u.create.argc; ++i) {
                free(req->u.create.argv[i]);
            }
            free(req->u.create.argv);
            req->u.create.argv = NULL;
        }
    } else if (req->opcode == OPCODE_COMBINE) {
        if (req->u.combine.taskids) {
            free(req->u.combine.taskids);
            req->u.combine.taskids = NULL;
        }
    }
}

// Format timing for printing
static void format_timing(const timing_t *t, char *out, size_t outlen) {
    if (!t) { snprintf(out, outlen, "(none)"); return; }
    if (t->minutes == 0 && t->hours == 0 && t->daysofweek == 0) {
        snprintf(out, outlen, "(no timing)");
        return;
    }
    char days[64] = "";
    int first = 1;
    for (int d = 0; d <= 6; ++d) {
        if (t->daysofweek & (1u << d)) {
            if (!first) strncat(days, ",", sizeof(days)-strlen(days)-1);
            char tmp[4]; snprintf(tmp, sizeof(tmp), "%d", d);
            strncat(days, tmp, sizeof(days)-strlen(days)-1);
            first = 0;
        }
    }
    if (days[0] == '\0') strncpy(days, "*", sizeof(days)-1);
    snprintf(out, outlen, "min=%" PRIu64 " hour=%u days=%s", t->minutes, t->hours, days);
}

// Join argv into a single command line for printing
static char *join_argv(uint32_t argc, char **argv) {
    if (argc == 0 || !argv) return strdup("");
    size_t total = 0;
    for (uint32_t i = 0; i < argc; ++i) total += strlen(argv[i]) + 1;
    char *res = malloc(total + 1);
    if (!res) return NULL;
    res[0] = '\0';
    for (uint32_t i = 0; i < argc; ++i) {
        if (i) strncat(res, " ", total - strlen(res));
        strncat(res, argv[i], total - strlen(res));
    }
    return res;
}

// Helpers to print error codes
static const char *errcode_to_str(uint16_t err) {
    switch (err) {
        case ERRCODE_NOT_FOUND: return "NOT_FOUND";
        case ERRCODE_NOT_RUN: return "NOT_RUN";
        default: return "UNKNOWN_ERROR";
    }
}

// Print list response in a readable format
static void handle_list_response(const response_t *resp) {
    if (!resp) return;
    if (resp->anstype == ANSTYPE_ERROR) {
        fprintf(stderr, "ERROR: %s\n", errcode_to_str(resp->u.error.errcode));
        return;
    }
    if (resp->anstype != ANSTYPE_OK) {
        fprintf(stderr, "Unexpected response type: 0x%04x\n", resp->anstype);
        return;
    }
    uint32_t n = resp->u.list_ok.nbtasks;
    task_t **tasks = resp->u.list_ok.tasks;
    if (n == 0) {
        puts("No tasks registered.");
        return;
    }
    for (uint32_t i = 0; i < n; ++i) {
        task_t *t = tasks[i];
    if (!t) continue;
    char timing_buf[128]; format_timing(&t->timing, timing_buf, sizeof(timing_buf));
    uint32_t argc = t->cmd ? t->cmd->argc : 0;
    char **argv = t->cmd ? t->cmd->argv : NULL;
    char *cmdline = join_argv(argc, argv);
    if (!cmdline) cmdline = strdup("(out of memory)");
    printf("taskid=%" PRIu64 " | %s\n cmd: %s\n", t->taskid, timing_buf, cmdline);
    free(cmdline);
    }
}

// Print times & exitcodes
static void handle_times_exitcodes_response(const response_t *resp) {
    if (!resp) return;
    if (resp->anstype == ANSTYPE_ERROR) {
        fprintf(stderr, "ERROR: %s\n", errcode_to_str(resp->u.error.errcode));
        return;
    }
    if (resp->anstype != ANSTYPE_OK) {
        fprintf(stderr, "Unexpected response type: 0x%04x\n", resp->anstype);
        return;
    }
    uint32_t n = resp->u.times_exitcodes_ok.nbruns;
    if (n == 0) {
        puts("No runs recorded for this task.");
        return;
    }
    int64_t *ts = resp->u.times_exitcodes_ok.timestamps;
    uint16_t *ec = resp->u.times_exitcodes_ok.exitcodes;
    for (uint32_t i = 0; i < n; ++i) {
        time_t sec = (time_t)ts[i];
        struct tm tm;
        char buf[64];
        if (gmtime_r(&sec, &tm)) {
            if (strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &tm) == 0) strncpy(buf, "(time format error)", sizeof(buf)-1);
        } else {
            strncpy(buf, "(invalid time)", sizeof(buf)-1);
        }
        printf("%s  exitcode=%u\n", buf, (unsigned)ec[i]);
    }
}

// Print stdout/stderr response
static void handle_output_response(const response_t *resp) {
    if (!resp) return;
    if (resp->anstype == ANSTYPE_ERROR) {
        fprintf(stderr, "ERROR: %s\n", errcode_to_str(resp->u.error.errcode));
        return;
    }
    if (resp->anstype != ANSTYPE_OK) {
        fprintf(stderr, "Unexpected response type: 0x%04x\n", resp->anstype);
        return;
    }
    char *out = resp->u.output_ok.output;
    size_t len = resp->u.output_ok.len;
    if (!out || len == 0) {
        puts("(no output)");
        return;
    }
    // Print raw bytes; ensure null termination for safe printing
    char *buf = malloc(len + 1);
    if (!buf) {
        // fallback: write directly to stdout
        fwrite(out, 1, len, stdout);
        return;
    }
    memcpy(buf, out, len);
    buf[len] = '\0';
    fwrite(buf, 1, len, stdout);
    if (buf[len-1] != '\n') putchar('\n');
    free(buf);
}

int main(int argc, char *argv[]) {
    int opt;

    char run_dir[512];
    compute_default_run_dir(run_dir, sizeof(run_dir));

    int flag_list = 0;
    int flag_terminate = 0; // -q
    int flag_create = 0;    // -c
    int flag_combine = 0;   // -s
    int flag_remove = 0;    // -r
    int flag_times = 0;     // -x
    int flag_stdout = 0;    // -o
    int flag_stderr = 0;    // -e

    int flag_no_timing = 0; // -n

    uint64_t minutes = 0;
    uint32_t hours = 0;
    uint8_t daysofweek = 0;

    // -c args
    char *c_first = NULL;
    char **c_extra = NULL;
    uint32_t c_extra_count = 0;

    // -s args
    char *s_first = NULL;
    uint64_t *s_taskids = NULL;
    uint32_t s_nbtasks = 0;

    uint64_t single_taskid = 0; // for -r/-x/-o/-e
    int have_single_taskid = 0;

    const char *optstr = "lx:o:e:c:s:r:qm:H:d:n:p:m:"; // include -m here

    // Track if any timing option was provided explicitly
    int timing_option_used = 0;

    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'l': flag_list = 1; break;
            case 'q': flag_terminate = 1; break;
            case 'p':
                if (optarg) {
                    strncpy(run_dir, optarg, sizeof(run_dir)-1);
                    run_dir[sizeof(run_dir)-1] = '\0';
                }
                break;
            case 'm':
                if (optarg) {
                    unsigned long mm;
                    if (parse_uint_range(optarg, 0, 59, &mm) != 0) {
                        fprintf(stderr, "Invalid minutes value: %s (expected 0-59)\n", optarg);
                        return 2;
                    }
                    minutes = (uint64_t)mm;
                    timing_option_used = 1;
                }
                break;
            case 'H':
                if (optarg) {
                    unsigned long h;
                    if (parse_uint_range(optarg, 0, 23, &h) != 0) {
                        fprintf(stderr, "Invalid hours value: %s (expected 0-23)\n", optarg);
                        return 2;
                    }
                    hours = (uint32_t)h;
                    timing_option_used = 1;
                }
                break;
            case 'd':
                if (optarg) {
                    if (parse_daysofweek(optarg, &daysofweek) != 0) {
                        fprintf(stderr, "Invalid daysofweek: %s (expected comma-separated numbers 0..6)\n", optarg);
                        return 2;
                    }
                    timing_option_used = 1;
                }
                break;
            case 'n':
                flag_no_timing = 1;
                break;
            case 'c':
                flag_create = 1;
                c_first = optarg;
                break;
            case 's':
                flag_combine = 1;
                s_first = optarg;
                break;
            case 'r':
                flag_remove = 1;
                if (parse_taskid(optarg, &single_taskid) != 0) {
                    fprintf(stderr, "Invalid taskid for -r: %s\n", optarg);
                    return 2;
                }
                have_single_taskid = 1;
                break;
            case 'x':
                flag_times = 1;
                if (parse_taskid(optarg, &single_taskid) != 0) {
                    fprintf(stderr, "Invalid taskid for -x: %s\n", optarg);
                    return 2;
                }
                have_single_taskid = 1;
                break;
            case 'o':
                flag_stdout = 1;
                if (parse_taskid(optarg, &single_taskid) != 0) {
                    fprintf(stderr, "Invalid taskid for -o: %s\n", optarg);
                    return 2;
                }
                have_single_taskid = 1;
                break;
            case 'e':
                flag_stderr = 1;
                if (parse_taskid(optarg, &single_taskid) != 0) {
                    fprintf(stderr, "Invalid taskid for -e: %s\n", optarg);
                    return 2;
                }
                have_single_taskid = 1;
                break;
            case '?':
            default:
                fprintf(stderr, "Usage error: invalid option\n");
                return 2;
        }
    }

    // Remaining args after options are at argv[optind .. argc-1]
    if (flag_create && c_first) {
        // Build argv list for the created command: first is c_first then the remaining args
        int rem = argc - optind;
        c_extra_count = 1 + (rem > 0 ? rem : 0);
        c_extra = calloc(c_extra_count, sizeof(char*));
        if (!c_extra) { perror("calloc"); return 1; }
        c_extra[0] = strdup(c_first);
        for (int i = 0; i < rem; ++i) {
            c_extra[1 + i] = strdup(argv[optind + i]);
        }
    } else if (flag_combine && s_first) {
        int rem = argc - optind;
        s_nbtasks = 1 + (rem > 0 ? rem : 0);
        s_taskids = calloc(s_nbtasks, sizeof(uint64_t));
        if (!s_taskids) { perror("calloc"); return 1; }
        if (parse_taskid(s_first, &s_taskids[0]) != 0) {
            fprintf(stderr, "Invalid taskid: %s\n", s_first);
            free(s_taskids); return 2;
        }
        for (int i = 0; i < rem; ++i) {
            uint64_t tid;
            if (parse_taskid(argv[optind + i], &tid) != 0) {
                fprintf(stderr, "Invalid taskid: %s\n", argv[optind + i]);
                free(s_taskids); return 2;
            }
            s_taskids[1 + i] = tid;
        }
    }

    // Validate basic action combinations
    int n_actions = flag_list + flag_terminate + flag_create + flag_combine + flag_remove + flag_times + flag_stdout + flag_stderr;
    if (n_actions == 0) {
        fprintf(stderr, "No action specified. Use -l, -c, -s, -r, -x, -o, -e or -q.\n");
        return 2;
    }

    if ((flag_create || flag_combine) && (flag_list || flag_terminate || flag_times || flag_stdout || flag_stderr || flag_remove)) {
        fprintf(stderr, "Invalid combination: -c/-s cannot be combined with -l/-x/-o/-e/-r/-q\n");
        return 2;
    }
    if (flag_create && flag_combine) {
        fprintf(stderr, "-c and -s are mutually exclusive\n");
        return 2;
    }
    if (flag_no_timing && !(flag_create || flag_combine)) {
        fprintf(stderr, "-n (no-timing) must be combined with -c or -s\n");
        return 2;
    }
    if ((flag_times || flag_stdout || flag_stderr || flag_remove) && !have_single_taskid) {
        fprintf(stderr, "Option requires a taskid argument\n");
        return 2;
    }

    // Timing validity rules
    if (timing_option_used && !(flag_create || flag_combine)) {
        fprintf(stderr, "Timing options (-m, -H, -d) must be combined with -c or -s\n");
        return 2;
    }
    if (flag_no_timing && timing_option_used) {
        fprintf(stderr, "-n (no-timing) cannot be combined with -m/-H/-d\n");
        return 2;
    }

    // Build timing structure
    timing_t timing;
    memset(&timing, 0, sizeof(timing));
    if (flag_no_timing) {
        timing.minutes = 0;
        timing.hours = 0;
        timing.daysofweek = 0;
    } else {
        timing.minutes = minutes;
        timing.hours = hours;
        timing.daysofweek = daysofweek;
    }

    // Build request
    request_t *req = calloc(1, sizeof(request_t));
    if (!req) { perror("calloc"); return 1; }

    if (flag_list) {
        req->opcode = OPCODE_LIST;
    } else if (flag_terminate) {
        req->opcode = OPCODE_TERMINATE;
    } else if (flag_create) {
        req->opcode = OPCODE_CREATE;
        req->u.create.timing = timing;
        req->u.create.argc = c_extra_count;
        req->u.create.argv = calloc(c_extra_count, sizeof(char*));
        if (!req->u.create.argv) { perror("calloc"); free(req); return 1; }
        for (uint32_t i = 0; i < c_extra_count; ++i) {
            req->u.create.argv[i] = strdup(c_extra[i]);
        }
    } else if (flag_combine) {
        req->opcode = OPCODE_COMBINE;
        req->u.combine.timing = timing;
#ifdef HAVE_TYPE_FROM_STR
        req->u.combine.type = type_from_str("CB");
#endif
        req->u.combine.nbtasks = s_nbtasks;
        req->u.combine.taskids = calloc(s_nbtasks, sizeof(uint64_t));
        if (!req->u.combine.taskids) { perror("calloc"); free_request_allocs(req); free(req); return 1; }
        for (uint32_t i = 0; i < s_nbtasks; ++i) req->u.combine.taskids[i] = s_taskids[i];
    } else if (flag_remove) {
        req->opcode = OPCODE_REMOVE;
        req->u.query.taskid = single_taskid;
    } else if (flag_times) {
        req->opcode = OPCODE_TIMES_EXITCODES;
        req->u.query.taskid = single_taskid;
    } else if (flag_stdout) {
        req->opcode = OPCODE_STDOUT;
        req->u.query.taskid = single_taskid;
    } else if (flag_stderr) {
        req->opcode = OPCODE_STDERR;
        req->u.query.taskid = single_taskid;
    } else {
        fprintf(stderr, "No valid action detected after parsing\n");
        free(req);
        return 2;
    }

    // Open pipes to server
    int req_fd = -1, rep_fd = -1;
    if (open_pipes_client(run_dir, &req_fd, &rep_fd) != 0) {
        perror("open_pipes_client");
        free_request_allocs(req);
        free(req);
        return 1;
    }

    if (send_request(req_fd, req) != 0) {
        perror("send_request");
        free_request_allocs(req);
        free(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    response_t *resp = NULL;
    if (receive_response(rep_fd, &resp) != 0) {
        perror("receive_response");
        free_request_allocs(req);
        free(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    // Handle consultative responses with formatting
    switch (req->opcode) {
        case OPCODE_LIST:
            handle_list_response(resp);
            break;
        case OPCODE_TIMES_EXITCODES:
            handle_times_exitcodes_response(resp);
            break;
        case OPCODE_STDOUT:
        case OPCODE_STDERR:
            handle_output_response(resp);
            break;
        default:
            // For non-consultative, fall back to generic OK/ERROR
            if (resp->anstype == ANSTYPE_OK) {
                puts("OK");
            } else if (resp->anstype == ANSTYPE_ERROR) {
                fprintf(stderr, "ERROR: errcode=0x%04x (%s)\n", resp->u.error.errcode, errcode_to_str(resp->u.error.errcode));
            } else {
                fprintf(stderr, "Unknown response anstype=0x%04x\n", resp->anstype);
            }
            break;
    }

    // Cleanup
    if (resp) free_response(resp);
    free_request_allocs(req);
    free(req);
    close(req_fd);
    close(rep_fd);

    if (c_extra) {
        for (uint32_t i = 0; i < c_extra_count; ++i) free(c_extra[i]);
        free(c_extra);
    }
    if (s_taskids) free(s_taskids);

    return 0;
}
