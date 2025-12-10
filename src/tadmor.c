// TODO: À compléter par la personne responsable du client
// Référence : enonce.md, ARCHITECTURE_T1.1.md

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

#include "protocol.h"    
#include "serialization.h" 




//Helper
static void compute_default_run_dir(char *out, size_t outlen) {
    const char *user = getenv("USER");
    if (!user) {
        struct passwd *pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }
    snprintf(out, outlen, "/tmp/%s/erraid", user);
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

//Parse daysofweek
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

//Parse single taskid
static int parse_taskid(const char *s, uint64_t *out) {
    if (!s) return -1;
    char *end = NULL;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    *out = (uint64_t)v;
    return 0;
}

/*Print response
static void print_response(const response_t *resp) {
    if (!resp) return;
    if (resp->anstype == ANSTYPE_OK) {
        puts("OK:");
        puts("  (response: OK)");
    } else if (resp->anstype == ANSTYPE_ERROR) {
        fprintf(stderr, "ERROR: errcode=0x%04x\n", resp->u.error.errcode);
    } else {
        fprintf(stderr, "Unknown response anstype=0x%04x\n", resp->anstype);
    }
}*/

/*Free helper
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
}*/

int main(int argc, char *argv[]) {
    int opt;
    
    char run_dir[512];
    compute_default_run_dir(run_dir, sizeof(run_dir));

    int flag_list = 0;
    int flag_terminate = 0;
    int flag_create = 0;
    int flag_combine = 0;
    int flag_remove = 0;
    int flag_times = 0; // -x
    int flag_stdout = 0; // -o
    int flag_stderr = 0; // -e
    int flag_no_timing = 0; // -n

    //int timing_set_any = 0;
    uint64_t minutes = 0;
    uint32_t hours = 0;
    uint8_t daysofweek = 0;

    //-c
    char *c_first = NULL; 
    char **c_extra = NULL; 
    //uint32_t c_extra_count = 0;

    //-s
    char *s_first = NULL;
    uint64_t *s_taskids = NULL;
    uint32_t s_nbtasks = 0;

    //one arg
    uint64_t single_taskid = 0; //-r, -x, -o, -e
    int have_single_taskid = 0;

    const char *optstr = "lx:o:e:c:s:r:qm:H:d:np:";

    //getopt parse
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
                break;
            case 'H':
                if (optarg) {
                    unsigned long h;
                    if (parse_uint_range(optarg, 0, 23, &h) != 0) {
                        fprintf(stderr, "Invalid hours value: %s (expected 0-23)\n", optarg);
                        return 2;
                    }
                    hours = (uint32_t)h;
                    ////timing_set_any = 1;
                }
                break;
            case 'd':
                if (optarg) {
                    if (parse_daysofweek(optarg, &daysofweek) != 0) {
                        fprintf(stderr, "Invalid daysofweek: %s (expected comma-separated numbers 0..6)\n", optarg);
                        return 2;
                    }
                    //////timing_set_any = 1;
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

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-m") == 0 && i+1 < argc) {
            unsigned long mm;
            if (parse_uint_range(argv[i+1], 0, 59, &mm) != 0) {
                fprintf(stderr, "Invalid minutes value: %s (expected 0-59)\n", argv[i+1]);
                return 2;
            }
            minutes = (uint64_t)mm;
            ////timing_set_any = 1;
        }
    }

    if (flag_create && c_first) {

        int rem = argc - optind;
        uint32_t total = 1 + (rem > 0 ? (uint32_t)rem : 0);
        c_extra = calloc(total, sizeof(char*));
        if (!c_extra) { perror("calloc"); return 1; }
        c_extra[0] = strdup(c_first);
        for (uint32_t i = 1; i < total; ++i) {
            c_extra[i] = strdup(argv[optind + (i - 1)]);
        }
        ////c_extra_count = total;
    } else if (flag_combine && s_first) {
        int rem = argc - optind;
        uint32_t total = 1 + (rem > 0 ? (uint32_t)rem : 0);
        s_taskids = calloc(total, sizeof(uint64_t));
        if (!s_taskids) { perror("calloc"); return 1; }
        if (parse_taskid(s_first, &s_taskids[0]) != 0) {
            fprintf(stderr, "Invalid taskid: %s\n", s_first);
            free(s_taskids); return 2;
        }
        s_nbtasks = 1;
        for (uint32_t i = 1; i < total; ++i) {
            uint64_t tid;
            if (parse_taskid(argv[optind + (i - 1)], &tid) != 0) {
                fprintf(stderr, "Invalid taskid: %s\n", argv[optind + (i - 1)]);
                free(s_taskids); return 2;
            }
            s_taskids[i] = tid;
            s_nbtasks++;
        }
    }

    //combinaisons valides
    int n_actions = flag_list + flag_terminate + flag_create + flag_combine + flag_remove + flag_times + flag_stdout + flag_stderr;
    if (n_actions == 0) {
        fprintf(stderr, "No action specified. Use -l, -c, -s, -r, -x, -o, -e or -q.\n");
        return 2;
    }
    //combinaisons non valides
    if ((flag_create || flag_combine) && (flag_list || flag_terminate || flag_times || flag_stdout || flag_stderr || flag_remove)) {
        fprintf(stderr, "Invalid combination: -c/-s cannot be combined with -l/-x/-o/-e/-r/-q\n");
        return 2;
    }
    //-c et -s
    if (flag_create && flag_combine) {
        fprintf(stderr, "-c and -s are mutually exclusive\n");
        return 2;
    }
    //Pour -n
    if (flag_no_timing && !(flag_create || flag_combine)) {
        fprintf(stderr, "-n (no-timing) must be combined with -c or -s\n");
        return 2;
    }
    //Pour -x/-o/-e/-r
    if ((flag_times || flag_stdout || flag_stderr || flag_remove) && !have_single_taskid) {
        fprintf(stderr, "Option requires a taskid argument\n");
        return 2;
    }

    
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
}

    /*request_t
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
        req->u.combine.type = type_from_str("CB");
        req->u.combine.nbtasks = s_nbtasks;
        req->u.combine.taskids = calloc(s_nbtasks, sizeof(uint64_t));
        if (!req->u.combine.taskids) { perror("calloc"); free(req); return 1; }
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

    //Wait
    response_t *resp = NULL;
    if (receive_response(rep_fd, &resp) != 0) {
        perror("receive_response");
        free_request_allocs(req);
        free(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    //Print response
    print_response(resp);

    //Cleanup
    free_response(resp);
    free_request_allocs(req);
    free(req);
    close(req_fd);
    close(rep_fd);

    //Free 
    if (c_extra) {
        for (uint32_t i = 0; i < c_extra_count; ++i) free(c_extra[i]);
        free(c_extra);
    }
    if (s_taskids) free(s_taskids);

    return 0;
}*/
