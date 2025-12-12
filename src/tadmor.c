//tadmor.c - client argument parsing + consultative requests (T2.5 + T2.6)
//Version corrigée : parsing complet des timings, formatage correct des commandes

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
#include "task_tree.h"

//Helper
static void compute_default_run_dir(char *out, size_t outlen) {
    const char *user = getenv("USER");
    if (!user) {
        struct passwd *pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }
    snprintf(out, outlen, "/tmp/%s/erraid/pipes", user);
}

//Parse int non signé
static int parse_uint_range(const char *s, unsigned long min, unsigned long max, unsigned long *out) {
    char *end;
    errno = 0;
    unsigned long v = strtoul(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    if (v < min || v > max) return -1;
    *out = v;
    return 0;
}

//Parse minutes avec support des listes et "*"
static int parse_minutes(const char *s, uint64_t *out) {
    if (!s || !*s) {
        *out = 0;
        return 0;
    }
    if (strcmp(s, "*") == 0) {
        *out = 0xFFFFFFFFFFFFFFFFULL;
        return 0;
    }
    uint64_t mask = 0;
    char *tmp = strdup(s);
    if (!tmp) return -1;
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, ",", &saveptr);
    while (tok) {
        unsigned long m;
        if (parse_uint_range(tok, 0, 59, &m) != 0) {
            free(tmp);
            return -1;
        }
        mask |= (1ULL << m);
        tok = strtok_r(NULL, ",", &saveptr);
    }
    free(tmp);
    *out = mask;
    return 0;
}

//Parse hours avec support des listes et "*"
static int parse_hours(const char *s, uint32_t *out) {
    if (!s || !*s) {
        *out = 0;
        return 0;
    }
    if (strcmp(s, "*") == 0) {
        *out = 0xFFFFFFFF;
        return 0;
    }
    uint32_t mask = 0;
    char *tmp = strdup(s);
    if (!tmp) return -1;
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, ",", &saveptr);
    while (tok) {
        unsigned long h;
        if (parse_uint_range(tok, 0, 23, &h) != 0) {
            free(tmp);
            return -1;
        }
        mask |= (1U << h);
        tok = strtok_r(NULL, ",", &saveptr);
    }
    free(tmp);
    *out = mask;
    return 0;
}

//Parse daysofweek avec support de "*"
static int parse_daysofweek(const char *s, uint8_t *out_mask) {
    if (!s || !*s) {
        *out_mask = 0;
        return 0;
    }
    if (strcmp(s, "*") == 0) {
        *out_mask = 0x7F;
        return 0;
    }
    uint8_t mask = 0;
    char *tmp = strdup(s);
    if (!tmp) return -1;
    char *saveptr = NULL;
    char *tok = strtok_r(tmp, ",", &saveptr);
    while (tok) {
        unsigned long d;
        if (parse_uint_range(tok, 0, 6, &d) != 0) {
            free(tmp);
            return -1;
        }
        mask |= (1U << d);
        tok = strtok_r(NULL, ",", &saveptr);
    }
    free(tmp);
    *out_mask = mask;
    return 0;
}

//Parse taskid
static int parse_taskid(const char *s, uint64_t *out) {
    if (!s) return -1;
    char *end = NULL;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    *out = (uint64_t)v;
    return 0;
}

//Free request allocs
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

//Format command line (support simple et sequence)
static char *format_command_line_internal(const command_t *cmd, int top_level);
static char *format_command_line(const command_t *cmd) {
    return format_command_line_internal(cmd, 1);
}
static char *format_command_line_internal(const command_t *cmd, int top_level) {
    if (!cmd) return NULL;
    
    if (cmd->type == type_from_str("SI")) {
        // Commande simple : concaténer les arguments
        if (cmd->argc == 0 || !cmd->argv) return strdup("");
        size_t total_len = 0;
        for (uint32_t i = 0; i < cmd->argc; i++) {
            total_len += strlen(cmd->argv[i]) + 1;  // +1 pour l'espace
        }
        
        char *result = malloc(total_len);
        if (!result) return NULL;
        
        size_t pos = 0;
        for (uint32_t i = 0; i < cmd->argc; i++) {
            size_t len = strlen(cmd->argv[i]);
            memcpy(result + pos, cmd->argv[i], len);
            pos += len;
            if (i < cmd->argc - 1) {
                result[pos++] = ' ';
            }
        }
        result[pos] = '\0';
        return result;
    } else if (cmd->type == type_from_str("SQ")) {
        // Commande séquence : formater récursivement
        if (cmd->nb_cmds == 0 || !cmd->cmds) return strdup("()");
        
        char **sub_cmds = malloc(cmd->nb_cmds * sizeof(char*));
        if (!sub_cmds) return NULL;
        
        size_t total_len = (top_level ? 0 : 4);  // "( " et " )" avec espaces si pas top_level
        for (uint32_t i = 0; i < cmd->nb_cmds; i++) {
            sub_cmds[i] = format_command_line_internal(cmd->cmds[i], 0);
            if (!sub_cmds[i]) {
                for (uint32_t j = 0; j < i; j++) free(sub_cmds[j]);
                free(sub_cmds);
                return NULL;
            }
            total_len += strlen(sub_cmds[i]) + 3;  // +3 pour "; "
        }
        
        char *result = malloc(total_len);
        if (!result) {
            for (uint32_t i = 0; i < cmd->nb_cmds; i++) free(sub_cmds[i]);
            free(sub_cmds);
            return NULL;
        }
        
        size_t pos = 0;
        if (!top_level) {
            result[pos++] = '(';
            result[pos++] = ' ';  // Espace après (
        }
        for (uint32_t i = 0; i < cmd->nb_cmds; i++) {
            size_t len = strlen(sub_cmds[i]);
            memcpy(result + pos, sub_cmds[i], len);
            pos += len;
            free(sub_cmds[i]);
            if (i < cmd->nb_cmds - 1) {
                result[pos++] = ' ';
                result[pos++] = ';';
                result[pos++] = ' ';
            }
        }
        if (!top_level) {
            result[pos++] = ' ';  // Espace avant )
            result[pos++] = ')';
        }
        result[pos] = '\0';
        free(sub_cmds);
        return result;
    }
    
    return NULL;
}

//Format timing au format "min hour days" (ex: "0,15,30 * 0,1,2" ou "* * *")
static void format_timing_display(const timing_t *t, char *out, size_t outlen) {
    if (!t || (t->minutes == 0 && t->hours == 0 && t->daysofweek == 0)) {
        snprintf(out, outlen, "- - -");
        return;
    }
    
    out[0] = '\0';
    
    // Minutes
    // Vérifier si tous les bits de 0 à 59 sont set
    if (t->minutes == 0xFFFFFFFFFFFFFFFFULL || t->minutes == ((1ULL << 60) - 1)) {
        strcat(out, "*");
    } else if (t->minutes == 0) {
        strcat(out, "-");
    } else {
        int first = 1;
        for (int i = 0; i < 60; i++) {
            if (t->minutes & (1ULL << i)) {
                if (!first) strcat(out, ",");
                int range_start = i;
                int range_end = i;
                while (range_end + 1 < 60 && (t->minutes & (1ULL << (range_end + 1)))) {
                    range_end++;
                }
                char num[32];
                if (range_start == range_end) {
                    snprintf(num, sizeof(num), "%d", range_start);
                } else {
                    snprintf(num, sizeof(num), "%d-%d", range_start, range_end);
                }
                strcat(out, num);
                i = range_end;
                first = 0;
            }
        }
    }
    strcat(out, " ");
    
    // Heures
    // Vérifier si tous les bits de 0 à 23 sont set
    if (t->hours == 0xFFFFFFFF || t->hours == ((1U << 24) - 1)) {
        strcat(out, "*");
    } else if (t->hours == 0) {
        strcat(out, "-");
    } else {
        int first = 1;
        for (int i = 0; i < 24; i++) {
            if (t->hours & (1U << i)) {
                if (!first) strcat(out, ",");
                int range_start = i;
                int range_end = i;
                while (range_end + 1 < 24 && (t->hours & (1U << (range_end + 1)))) {
                    range_end++;
                }
                char num[32];
                if (range_start == range_end) {
                    snprintf(num, sizeof(num), "%d", range_start);
                } else {
                    snprintf(num, sizeof(num), "%d-%d", range_start, range_end);
                }
                strcat(out, num);
                i = range_end;
                first = 0;
            }
        }
    }
    strcat(out, " ");
    
    // Jours
    if (t->daysofweek == 0x7F) {
        strcat(out, "*");
    } else if (t->daysofweek == 0) {
        strcat(out, "-");
    } else {
        int first = 1;
        for (int i = 0; i < 7; i++) {
            if (t->daysofweek & (1U << i)) {
                if (!first) strcat(out, ",");
                int range_start = i;
                int range_end = i;
                while (range_end + 1 < 7 && (t->daysofweek & (1U << (range_end + 1)))) {
                    range_end++;
                }
                char num[32];
                if (range_start == range_end) {
                    snprintf(num, sizeof(num), "%d", range_start);
                } else {
                    snprintf(num, sizeof(num), "%d-%d", range_start, range_end);
                }
                strcat(out, num);
                i = range_end;
                first = 0;
            }
        }
    }
}

//Helpers 
static const char *errcode_to_str(uint16_t err) {
    switch (err) {
        case ERRCODE_NOT_FOUND: return "NOT_FOUND";
        case ERRCODE_NOT_RUN: return "NOT_RUN";
        default: return "UNKNOWN_ERROR";
    }
}

//Print list
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
        return;
    }
    for (uint32_t i = 0; i < n; ++i) {
        task_t *t = tasks[i];
        if (!t) continue;
        char timing_buf[256];
        format_timing_display(&t->timing, timing_buf, sizeof(timing_buf));
        char *cmdline = format_command_line(t->cmd);
        if (!cmdline) cmdline = strdup("(out of memory)");
        printf("%lu: %s %s\n", (unsigned long)t->taskid, timing_buf, cmdline);
        free(cmdline);
    }
}

//Print times & exitcodes
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
        if (localtime_r(&sec, &tm)) {
            if (strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm) == 0) {
                strncpy(buf, "(time format error)", sizeof(buf)-1);
            }
        } else {
            strncpy(buf, "(invalid time)", sizeof(buf)-1);
        }
        printf("%s %u\n", buf, (unsigned)ec[i]);
    }
}

//Print stdout/stderr response
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
        return;  // Pas de sortie, ne rien afficher
    }
    fwrite(out, 1, len, stdout);
}

int main(int argc, char *argv[]) {
    int opt;

    char run_dir[512];
    compute_default_run_dir(run_dir, sizeof(run_dir));

    int flag_list = 0;
    int flag_terminate = 0; //-q
    int flag_create = 0;    //-c
    int flag_combine = 0;   //-s
    int flag_remove = 0;    //-r
    int flag_times = 0;     //-x
    int flag_stdout = 0;    //-o
    int flag_stderr = 0;    //-e

    int flag_no_timing = 0; //-n

    uint64_t minutes = 0;
    uint32_t hours = 0;
    uint8_t daysofweek = 0;

    //-c args
    char *c_first = NULL;
    char **c_extra = NULL;
    uint32_t c_extra_count = 0;

    //-s args
    char *s_first = NULL;
    uint64_t *s_taskids = NULL;
    uint32_t s_nbtasks = 0;

    uint64_t single_taskid = 0; //for -r/-x/-o/-e
    int have_single_taskid = 0;

    const char *optstr = "lx:o:e:c:s:r:qm:H:d:n:p:";

    //Track if any timing option was provided explicitly
    int timing_option_used = 0;

    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'l': flag_list = 1; break;
            case 'q': flag_terminate = 1; break;
            case 'm':
                if (optarg) {
                    if (parse_minutes(optarg, &minutes) != 0) {
                        fprintf(stderr, "Invalid minutes value: %s (expected 0-59 or comma-separated list or *)\n", optarg);
                        return 2;
                    }
                    timing_option_used = 1;
                }
                break;
            case 'H':
                if (optarg) {
                    if (parse_hours(optarg, &hours) != 0) {
                        fprintf(stderr, "Invalid hours value: %s (expected 0-23 or comma-separated list or *)\n", optarg);
                        return 2;
                    }
                    timing_option_used = 1;
                }
                break;
            case 'd':
                if (optarg) {
                    if (parse_daysofweek(optarg, &daysofweek) != 0) {
                        fprintf(stderr, "Invalid daysofweek: %s (expected comma-separated numbers 0..6 or *)\n", optarg);
                        return 2;
                    }
                    timing_option_used = 1;
                }
                break;
            case 'n':
                flag_no_timing = 1;
                break;
        case 'p':
                if (optarg && optarg[0] != '\0') {
                    strncpy(run_dir, optarg, sizeof(run_dir) - 1);
                    run_dir[sizeof(run_dir) - 1] = '\0';
                } else {
                    fprintf(stderr, "Error: -p requires a directory argument\n");
                    return 2;
                }
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

    if (flag_create && c_first) {
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

    //Timing
    if (timing_option_used && !(flag_create || flag_combine)) {
        fprintf(stderr, "Timing options (-m, -H, -d) must be combined with -c or -s\n");
        return 2;
    }
    if (flag_no_timing && timing_option_used) {
        fprintf(stderr, "-n (no-timing) cannot be combined with -m/-H/-d\n");
        return 2;
    }

    //Build timing structure
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

    //Build request
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
        req->u.combine.type = type_from_str("SQ"); // Sequence
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

    //Open pipes
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
    if (receive_response(rep_fd, &resp, req->opcode) != 0) {
        perror("receive_response");
        free_request_allocs(req);
        free(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    int ret_code = 0;
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
            if (resp->anstype == ANSTYPE_OK) {
                puts("OK");
            } else if (resp->anstype == ANSTYPE_ERROR) {
                fprintf(stderr, "ERROR: errcode=0x%04x (%s)\n", resp->u.error.errcode, errcode_to_str(resp->u.error.errcode));
            } else {
                fprintf(stderr, "Unknown response anstype=0x%04x\n", resp->anstype);
            }
            break;
    }
    if (resp && resp->anstype == ANSTYPE_ERROR) {
        ret_code = 1;
    }

    //Cleanup
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

    return ret_code;
}
