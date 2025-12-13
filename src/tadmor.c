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

    uint64_t single_taskid = 0; //for -x/-o/-e
    int have_single_taskid = 0;

    const char *optstr = "lx:o:e:qp:";

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
            case '?':
            default:
                fprintf(stderr, "Usage error: invalid option\n");
                return 2;
        }
    }

    int n_actions = flag_list + flag_terminate + flag_times + flag_stdout + flag_stderr;
    if (n_actions == 0) {
        fprintf(stderr, "No action specified. Use -l, -x, -o, -e or -q.\n");
        return 2;
    }

    if ((flag_times || flag_stdout || flag_stderr) && !have_single_taskid) {
        fprintf(stderr, "Option requires a taskid argument\n");
        return 2;
    }

    //Build request
    request_t *req = calloc(1, sizeof(request_t));
    if (!req) { perror("calloc"); return 1; }

    if (flag_list) {
        req->opcode = OPCODE_LIST;
    } else if (flag_terminate) {
        req->opcode = OPCODE_TERMINATE;
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
            if (resp->anstype == ANSTYPE_OK) {
                // Pas de sortie pour TERMINATE (succès silencieux)
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
