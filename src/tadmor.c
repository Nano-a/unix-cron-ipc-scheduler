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
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#include "protocol.h"
#include "serialization.h"
#include "task_tree.h"

// Calcule le répertoire par défaut pour les pipes
static void compute_default_run_dir(char *out, size_t outlen) {
    const char *user = getenv("USER");
    if (!user) {
        struct passwd *pw = getpwuid(getuid());
        user = pw ? pw->pw_name : "unknown";
    }
    snprintf(out, outlen, "/tmp/%s/erraid/pipes", user);
}

// Parse taskid depuis une chaîne de caractères
static int parse_taskid(const char *s, uint64_t *out) {
    if (!s) return -1;
    char *end = NULL;
    errno = 0;
    unsigned long long v = strtoull(s, &end, 10);
    if (errno != 0 || *end != '\0') return -1;
    *out = (uint64_t)v;
    return 0;
}

// Parse une valeur unique ou une plage (ex: "5" ou "5-10")
static int parse_value_or_range(const char *s, int *start_out, int *end_out) {
    if (!s || !start_out || !end_out) return -1;
    
    char *dash = strchr(s, '-');
    if (!dash) {
        // Valeur unique
        char *end = NULL;
        errno = 0;
        long v = strtol(s, &end, 10);
        if (errno != 0 || *end != '\0' || v < 0) return -1;
        *start_out = (int)v;
        *end_out = (int)v;
        return 0;
    } else {
        // Plage
        char *start_str = strdup(s);
        if (!start_str) return -1;
        start_str[dash - s] = '\0';
        
        char *end = NULL;
        errno = 0;
        long start = strtol(start_str, &end, 10);
        if (errno != 0 || *end != '\0' || start < 0) {
            free(start_str);
            return -1;
        }
        
        errno = 0;
        long end_val = strtol(dash + 1, &end, 10);
        if (errno != 0 || *end != '\0' || end_val < start) {
            free(start_str);
            return -1;
        }
        
        free(start_str);
        *start_out = (int)start;
        *end_out = (int)end_val;
        return 0;
    }
}

// Parse une liste de valeurs/plages séparées par des virgules (ex: "0,3,6,9" ou "0-5,10-15")
static int parse_list(const char *s, int max_value, uint64_t *bits_out) {
    if (!s || !bits_out) return -1;
    *bits_out = 0;
    
    if (strcmp(s, "*") == 0) {
        // Tous les bits à 1 (de 0 à max_value inclus, donc max_value+1 bits)
        if (max_value < 63) {
            *bits_out = (1ULL << (max_value + 1)) - 1;
        } else {
            *bits_out = 0xFFFFFFFFFFFFFFFFULL;
        }
        return 0;
    }
    
    if (strcmp(s, "-") == 0) {
        // Aucun bit (déjà à 0)
        return 0;
    }
    
    // Parser la liste : "0,3,6,9" ou "0-5,10"
    char *copy = strdup(s);
    if (!copy) return -1;
    
    char *token = strtok(copy, ",");
    while (token) {
        int start, end;
        if (parse_value_or_range(token, &start, &end) < 0) {
            free(copy);
            return -1;
        }
        if (start > max_value || end > max_value) {
            free(copy);
            return -1;
        }
        for (int i = start; i <= end; i++) {
            *bits_out |= (1ULL << i);
        }
        token = strtok(NULL, ",");
    }
    free(copy);
    return 0;
}

// Parse les minutes (0-59)
static int parse_minutes(const char *s, uint64_t *minutes_out) {
    return parse_list(s, 59, minutes_out);
}

// Parse les heures (0-23)
static int parse_hours(const char *s, uint32_t *hours_out) {
    uint64_t bits = 0;
    if (parse_list(s, 23, &bits) < 0) return -1;
    *hours_out = (uint32_t)bits;
    return 0;
}

// Parse les jours de la semaine (0-6)
static int parse_days(const char *s, uint8_t *days_out) {
    uint64_t bits = 0;
    if (parse_list(s, 6, &bits) < 0) return -1;
    *days_out = (uint8_t)bits;
    return 0;
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

// Affiche la réponse LIST
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

// Affiche la réponse TIMES_EXITCODES
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

// Affiche la réponse STDOUT ou STDERR
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
    int flag_times = 0;     //-x
    int flag_stdout = 0;    //-o
    int flag_stderr = 0;    //-e
    int flag_create = 0;    //-c
    int flag_remove = 0;    //-r
    int flag_combine = 0;   //-s
    int flag_abstract = 0;  //-n (tâche abstraite)

    uint64_t single_taskid = 0; //for -x/-o/-e/-r
    int have_single_taskid = 0;

    // Timings pour CREATE et COMBINE
    const char *minutes_str = NULL;
    const char *hours_str = NULL;
    const char *days_str = NULL;

    // Taskids pour COMBINE
    uint64_t *combine_taskids = NULL;
    uint32_t combine_nbtasks = 0;
    size_t combine_capacity = 0;

    const char *optstr = "lx:o:e:qp:cr:sm:H:d:n";

    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'l': flag_list = 1; break;
            case 'q': flag_terminate = 1; break;
        case 'p':
                if (optarg && optarg[0] != '\0') {
                    strncpy(run_dir, optarg, sizeof(run_dir) - 1);
                    run_dir[sizeof(run_dir) - 1] = '\0';
                } else {
                    fprintf(stderr, "Error: -p requires a directory argument\n");
                    return 2;
                }
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
            case 'c':
                flag_create = 1;
                break;
            case 'r':
                flag_remove = 1;
                if (parse_taskid(optarg, &single_taskid) != 0) {
                    fprintf(stderr, "Invalid taskid for -r: %s\n", optarg);
                    return 2;
                }
                have_single_taskid = 1;
                break;
            case 's':
                flag_combine = 1;
                break;
            case 'm':
                minutes_str = optarg;
                break;
            case 'H':
                hours_str = optarg;
                break;
            case 'd':
                days_str = optarg;
                break;
            case 'n':
                flag_abstract = 1;
                break;
            case '?':
            default:
                fprintf(stderr, "Usage error: invalid option\n");
                return 2;
        }
    }

    // Parser les arguments restants pour CREATE (commande) ou COMBINE (taskids)
    if (flag_create) {
        // Les arguments restants sont la commande à créer
        if (optind >= argc) {
            fprintf(stderr, "Error: -c requires a command\n");
            return 2;
        }
    } else if (flag_combine) {
        // Les arguments restants sont les taskids à combiner
        // Ignorer -p s'il apparaît (il a déjà été traité par getopt)
        combine_capacity = 16; // Capacité initiale
        combine_taskids = calloc(combine_capacity, sizeof(uint64_t));
        if (!combine_taskids) {
            perror("calloc");
            return 1;
        }
        
        for (int i = optind; i < argc; i++) {
            // Ignorer -p s'il apparaît (cas où -p est après -s)
            if (strcmp(argv[i], "-p") == 0) {
                if (i + 1 < argc) {
                    i++; // Ignorer aussi l'argument de -p
                }
                continue;
            }
            
            // Vérifier si c'est une option (commence par -)
            if (argv[i][0] == '-' && strlen(argv[i]) > 1) {
                // C'est une option inconnue, arrêter le parsing
                break;
            }
            
            // Agrandir le tableau si nécessaire
            if (combine_nbtasks >= combine_capacity) {
                combine_capacity *= 2;
                uint64_t *tmp = realloc(combine_taskids, combine_capacity * sizeof(uint64_t));
                if (!tmp) {
                    perror("realloc");
                    free(combine_taskids);
                    return 1;
                }
                combine_taskids = tmp;
            }
            
            // Parser le taskid
            if (parse_taskid(argv[i], &combine_taskids[combine_nbtasks]) != 0) {
                fprintf(stderr, "Invalid taskid for -s: %s\n", argv[i]);
                free(combine_taskids);
                return 2;
            }
            combine_nbtasks++;
        }
        
        if (combine_nbtasks == 0) {
            fprintf(stderr, "Error: -s requires at least one taskid\n");
            free(combine_taskids);
            return 2;
        }
    }

    int n_actions = flag_list + flag_terminate + flag_times + flag_stdout + flag_stderr + flag_create + flag_remove + flag_combine;
    if (n_actions == 0) {
        fprintf(stderr, "No action specified. Use -l, -x, -o, -e, -q, -c, -r or -s.\n");
        return 2;
    }

    if ((flag_times || flag_stdout || flag_stderr || flag_remove) && !have_single_taskid) {
        fprintf(stderr, "Option requires a taskid argument\n");
        return 2;
    }

    if (flag_combine && combine_nbtasks == 0) {
        fprintf(stderr, "Error: -s requires at least one taskid\n");
        if (combine_taskids) free(combine_taskids);
        return 2;
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
        // Construire le timing
        timing_t timing = {0, 0, 0};
        if (flag_abstract) {
            timing.minutes = 0;
            timing.hours = 0;
            timing.daysofweek = 0;
        } else {
            if (minutes_str) {
                if (parse_minutes(minutes_str, &timing.minutes) < 0) {
                    fprintf(stderr, "Invalid minutes format: %s\n", minutes_str);
                    free(req);
                    return 2;
                }
            } else {
                timing.minutes = (1ULL << 60) - 1;
            }
            if (hours_str) {
                if (parse_hours(hours_str, &timing.hours) < 0) {
                    fprintf(stderr, "Invalid hours format: %s\n", hours_str);
                    free(req);
                    return 2;
                }
            } else {
                timing.hours = (1U << 24) - 1;
            }
            if (days_str) {
                if (parse_days(days_str, &timing.daysofweek) < 0) {
                    fprintf(stderr, "Invalid days format: %s\n", days_str);
                    free(req);
                    return 2;
                }
            } else {
                timing.daysofweek = 0x7F;
            }
        }
        req->u.create.timing = timing;
        // Construire les arguments de la commande
        req->u.create.argc = (uint32_t)(argc - optind);
        req->u.create.argv = calloc(req->u.create.argc, sizeof(char *));
        if (!req->u.create.argv) {
            perror("calloc");
            free(req);
            return 1;
        }
        for (uint32_t i = 0; i < req->u.create.argc; i++) {
            req->u.create.argv[i] = strdup(argv[optind + i]);
            if (!req->u.create.argv[i]) {
                for (uint32_t j = 0; j < i; j++) {
                    free(req->u.create.argv[j]);
                }
                free(req->u.create.argv);
                free(req);
                perror("strdup");
                return 1;
            }
        }
    } else if (flag_combine) {
        req->opcode = OPCODE_COMBINE;
        // Construire le timing (même logique que CREATE)
        timing_t timing = {0, 0, 0};
        if (flag_abstract) {
            timing.minutes = 0;
            timing.hours = 0;
            timing.daysofweek = 0;
        } else {
            if (minutes_str) {
                if (parse_minutes(minutes_str, &timing.minutes) < 0) {
                    fprintf(stderr, "Invalid minutes format: %s\n", minutes_str);
                    free(combine_taskids);
                    free(req);
                    return 2;
                }
            } else {
                timing.minutes = (1ULL << 60) - 1;
            }
            if (hours_str) {
                if (parse_hours(hours_str, &timing.hours) < 0) {
                    fprintf(stderr, "Invalid hours format: %s\n", hours_str);
                    free(combine_taskids);
                    free(req);
                    return 2;
                }
            } else {
                timing.hours = (1U << 24) - 1;
            }
            if (days_str) {
                if (parse_days(days_str, &timing.daysofweek) < 0) {
                    fprintf(stderr, "Invalid days format: %s\n", days_str);
                    free(combine_taskids);
                    free(req);
                    return 2;
                }
            } else {
                timing.daysofweek = 0x7F;
            }
        }
        req->u.combine.timing = timing;
        req->u.combine.type = type_from_str("SQ"); // Séquence
        req->u.combine.nbtasks = combine_nbtasks;
        req->u.combine.taskids = combine_taskids;
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
        if (combine_taskids) free(combine_taskids);
        return 2;
    }

    // Ouvrir les tubes nommés
    int req_fd = -1, rep_fd = -1;
    if (open_pipes_client(run_dir, &req_fd, &rep_fd) != 0) {
        perror("open_pipes_client");
        free_request(req);
        return 1;
    }

    if (send_request(req_fd, req) != 0) {
        perror("send_request");
        free_request(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    // Attendre que des données soient disponibles avec un timeout (5 secondes)
    fd_set readfds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_SET(rep_fd, &readfds);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    int select_result = select(rep_fd + 1, &readfds, NULL, NULL, &timeout);
    if (select_result < 0) {
        perror("select");
        free_request(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }
    if (select_result == 0) {
        fprintf(stderr, "Timeout: no response from daemon (is it running?)\n");
        free_request(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }
    if (!FD_ISSET(rep_fd, &readfds)) {
        fprintf(stderr, "Error: reply pipe not ready\n");
        free_request(req);
        close(req_fd);
        close(rep_fd);
        return 1;
    }

    response_t *resp = NULL;
    if (receive_response(rep_fd, &resp, req->opcode) != 0) {
        perror("receive_response");
        free_request(req);
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
        case OPCODE_TERMINATE:
        case OPCODE_REMOVE:
            if (resp->anstype == ANSTYPE_OK) {
                // Pas de sortie pour TERMINATE/REMOVE (succès silencieux)
            } else if (resp->anstype == ANSTYPE_ERROR) {
                fprintf(stderr, "ERROR: %s\n", errcode_to_str(resp->u.error.errcode));
            } else {
                fprintf(stderr, "Unknown response anstype=0x%04x\n", resp->anstype);
            }
            break;
        case OPCODE_CREATE:
        case OPCODE_COMBINE:
            if (resp->anstype == ANSTYPE_OK) {
                // Afficher le taskid de la nouvelle tâche
                printf("%lu\n", (unsigned long)resp->u.create_ok.taskid);
            } else if (resp->anstype == ANSTYPE_ERROR) {
                fprintf(stderr, "ERROR: %s\n", errcode_to_str(resp->u.error.errcode));
            } else {
                fprintf(stderr, "Unknown response anstype=0x%04x\n", resp->anstype);
            }
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

    // Libérer la mémoire
    if (resp) free_response(resp);
    free_request(req);  // free_request fait déjà le free(req)
    close(req_fd);
    close(rep_fd);

    return ret_code;
}
