#define _DEFAULT_SOURCE
#include "serialization.h"
#include <endian.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>      // printf, perror
#include <fcntl.h>      // open, O_CREAT, O_WRONLY, O_TRUNC, O_RDONLY
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define CMD_SERIAL_KIND_SIMPLE   0
#define CMD_SERIAL_KIND_SEQUENCE 1





// ----------------------
// Fonctions d'écriture
// ----------------------

// Écrit un uint8 (1 octet) - pas besoin de conversion endian
int write_uint8(int fd, uint8_t value) {
    ssize_t written = write(fd, &value, sizeof(uint8_t));
    if (written != sizeof(uint8_t)) return -1;
    return 0;
}

// Écrit un uint16 (2 octets) en big-endian
int write_uint16(int fd, uint16_t value) {
    uint16_t be_value = htobe16(value);
    ssize_t written = write(fd, &be_value, sizeof(uint16_t));
    if (written != sizeof(uint16_t)) return -1;
    return 0;
}

// Écrit un uint32 (4 octets) en big-endian
int write_uint32(int fd, uint32_t value) {
    uint32_t be_value = htobe32(value);
    ssize_t written = write(fd, &be_value, sizeof(uint32_t));
    if (written != sizeof(uint32_t)) return -1;
    return 0;
}

// Écrit un uint64 (8 octets) en big-endian
int write_uint64(int fd, uint64_t value) {
    uint64_t be_value = htobe64(value);
    ssize_t written = write(fd, &be_value, sizeof(uint64_t));
    if (written != sizeof(uint64_t)) return -1;
    return 0;
}

// Écrit un int64 (8 octets) en big-endian
int write_int64(int fd, int64_t value) {
    int64_t be_value = htobe64((uint64_t)value);
    ssize_t written = write(fd, &be_value, sizeof(int64_t));
    if (written != sizeof(int64_t)) return -1;
    return 0;
}

// ----------------------
// Fonctions de lecture
// ----------------------

// Lit un uint8 (1 octet)
int read_uint8(int fd, uint8_t *value) {
    ssize_t rd = read(fd, value, sizeof(uint8_t));
    if (rd != sizeof(uint8_t)) return -1;
    return 0;
}

// Lit un uint16 (2 octets) en big-endian
int read_uint16(int fd, uint16_t *value) {
    uint16_t tmp;
    ssize_t rd = read(fd, &tmp, sizeof(uint16_t));
    if (rd == 0) {
        // Fin de fichier (client déconnecté)
        errno = EBADMSG;
        return -1;
    }
    if (rd != sizeof(uint16_t)) {
        // Erreur de lecture
        return -1;
    }
    *value = be16toh(tmp);
    return 0;
}

// Lit un uint32 (4 octets) en big-endian
int read_uint32(int fd, uint32_t *value) {
    uint32_t tmp;
    ssize_t rd = read(fd, &tmp, sizeof(uint32_t));
    if (rd != sizeof(uint32_t)) return -1;
    *value = be32toh(tmp);
    return 0;
}

// Lit un uint64 (8 octets) en big-endian
int read_uint64(int fd, uint64_t *value) {
    uint64_t tmp;
    ssize_t rd = read(fd, &tmp, sizeof(uint64_t));
    if (rd != sizeof(uint64_t)) return -1;
    *value = be64toh(tmp);
    return 0;
}

// Lit un int64 (8 octets) en big-endian
int read_int64(int fd, int64_t *value) {
    int64_t tmp;
    ssize_t rd = read(fd, &tmp, sizeof(int64_t));
    if (rd == 0) {
        // Fin de fichier (EOF)
        errno = EBADMSG;
        return -1;
    }
    if (rd != sizeof(int64_t)) {
        // Erreur de lecture
        return -1;
    }
    *value = (int64_t)be64toh((uint64_t)tmp);
    return 0;
}

// -------------------------------------------------------------------------------
// Fonctions auxiliaires pour s'assurer qu'on lise/écrit le nombre d'octets demandé
// -------------------------------------------------------------------------------


// Pour bien écrire le nombre d'octets demandé
static ssize_t robust_write(int fd, const void *buf, size_t count) {
    const uint8_t *p = (const uint8_t *)buf;
    size_t left = count;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (w == 0) {
            /* shouldn't happen for regular fds, treat as error */
            errno = EIO;
            return -1;
        }
        p += w;
        left -= (size_t)w;
    }
    return (ssize_t)count;
}

// Pour bien lire le nombre d'octets demandé
static ssize_t robust_read(int fd, void *buf, size_t count) {
    uint8_t *p = (uint8_t *)buf;
    size_t left = count;
    while (left > 0) {
        ssize_t r = read(fd, p, left);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) { /* EOF */
            errno = EBADMSG; /* indicate unexpected EOF while reading binary */
            return -1;
        }
        p += r;
        left -= (size_t)r;
    }
    return (ssize_t)count;
}

// --------------------
// Fonctions read/write
// --------------------


int write_string(int fd, const char *str) {

    if (!str) {
        errno = EINVAL;
        return -1;
    }

    size_t len = strlen(str);
    if (len > UINT32_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    uint32_t L = (uint32_t)len;
    if (write_uint32(fd, L) < 0) return -1;

    if (L == 0) return 0; 

    if (robust_write(fd, (const void *)str, L) != (ssize_t)L) return -1;
    return 0;
}

int read_string(int fd, char **out) {
    if (!out) {
        errno = EINVAL;
        return -1;
    }

    uint32_t L;
    if (read_uint32(fd, &L) < 0) return -1;

    size_t alloc_size = (size_t)L + 1;
    if (alloc_size <= (size_t)L) {
        errno = EOVERFLOW;
        return -1;
    }

    char *buf = malloc(alloc_size);
    if (!buf) return -1;

    if (L > 0) {
        if (robust_read(fd, buf, L) != (ssize_t)L) {
            free(buf);
            return -1;
        }
    }
    buf[L] = '\0';
    *out = buf;
    return 0;
}

// --------------------
// Fonctions Timing
// --------------------

int write_timing(int fd, const timing_t *t) {

    if (!t) {
        errno = EINVAL;
        return -1;
    }

    if (write_uint64(fd, t->minutes) < 0) return -1;
    if (write_uint32(fd, t->hours) < 0) return -1;
    if (write_uint8(fd, t->daysofweek) < 0) return -1;

    return 0;
}

int read_timing(int fd, timing_t *t) {

    if (!t) {
        errno = EINVAL;
        return -1;
    }

    if (read_uint64(fd, &t->minutes) < 0) return -1;
    if (read_uint32(fd, &t->hours) < 0) return -1;
    if (read_uint8(fd, &t->daysofweek) < 0) return -1;

    return 0;
}

// --------------------
// Fonctions Arguments
// --------------------

int write_arguments(int fd, uint32_t argc, char *argv[]) {
    if (argc < 1 || argv == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* ARGV[0] non vide */
    if (argv[0] == NULL || argv[0][0] == '\0') {
        errno = EINVAL;
        return -1;
    }

    if (write_uint32(fd, argc) < 0) return -1;

    for (uint32_t i = 0; i < argc; ++i) {
        if (argv[i] == NULL) {
            errno = EINVAL;
            return -1;
        }
        if (write_string(fd, argv[i]) < 0) return -1;
    }
    return 0;
}

int read_arguments(int fd, uint32_t *argc_out, char ***argv_out) {
    if (!argc_out || !argv_out) {
        errno = EINVAL;
        return -1;
    }

    uint32_t argc;
    if (read_uint32(fd, &argc) < 0) return -1;

    if (argc < 1) {
        errno = EPROTO; 
        return -1;
    }

    /*if (argc > SIZE_MAX / sizeof(char *)) { 
        errno = EOVERFLOW;
        return -1;
    }*/

    char **argv = malloc((size_t)argc * sizeof(char *));
    if (!argv) return -1;

    for (uint32_t i = 0; i < argc; ++i) {
        argv[i] = NULL;
    }

    for (uint32_t i = 0; i < argc; ++i) {
        if (read_string(fd, &argv[i]) < 0) {
            
            for (uint32_t j = 0; j < i; ++j) {
                free(argv[j]);
            }

            free(argv);
            return -1;
        }
    }

    /* ARGV[0] non vide */ 
    if (argv[0] == NULL || argv[0][0] == '\0') {
        for (uint32_t i = 0; i < argc; ++i) free(argv[i]);
        free(argv);
        errno = EPROTO;
        return -1;
    }

    *argc_out = argc;
    *argv_out = argv;
    return 0;
}

// --------------------
// Fonctions Commands
// --------------------

// Pour qu'on ait bien deux char au moins
static int validate_type2(const char *type2) {

    if (!type2) return 0;
    if ((unsigned char)type2[0] == 0 || (unsigned char)type2[1] == 0) return 0;
    return 1;
}

// Créer commande simple
command_t *create_simple_command(const char *type2, uint32_t argc, char *argv[]) {

    if (!validate_type2(type2) || argc < 1 || argv == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (!argv[0] || argv[0][0] == '\0') {
        errno = EINVAL;
        return NULL;
    }

    command_t *cmd = malloc(sizeof(command_t));
    if (!cmd) return NULL;

    cmd->type = type_from_str(type2);
    cmd->argc = argc;
    cmd->nb_cmds = 0;
    cmd->cmds = NULL;
    cmd->argv = malloc((size_t)argc * sizeof(char *));
    if (!cmd->argv) {
        free(cmd);
        return NULL;
    }

    for (uint32_t i = 0; i < argc; ++i) {
        if (!argv[i]) {
            /* on libère la mémoire */
            for (uint32_t j = 0; j < i; ++j) free(cmd->argv[j]);
            free(cmd->argv);
            free(cmd);
            errno = EINVAL;
            return NULL;
        }
        cmd->argv[i] = strdup(argv[i]);
        if (!cmd->argv[i]) {
            /* on libère la mémoire */
            for (uint32_t j = 0; j < i; ++j) free(cmd->argv[j]);
            free(cmd->argv);
            free(cmd);
            return NULL;
        }
    }
    return cmd;
}

command_t *create_sequence_command(const char *type2, uint32_t nb_cmds, command_t **cmds) {

    if (!validate_type2(type2) || nb_cmds == 0 || cmds == NULL) {
        errno = EINVAL;
        return NULL;
    }

    command_t *cmd = malloc(sizeof(command_t));
    if (!cmd) return NULL;

    cmd->type = type_from_str(type2);
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->nb_cmds = nb_cmds;
    cmd->cmds = cmds; 
    return cmd;
}

// Libération récursive
void free_command(command_t *cmd) {
    if (!cmd) return;

    /* commande simple */
    if (cmd->argc > 0 && cmd->argv != NULL) {
        for (uint32_t i = 0; i < cmd->argc; ++i) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
        cmd->argv = NULL;
    }

    /* séquence */
    if (cmd->nb_cmds > 0 && cmd->cmds != NULL) {
        for (uint32_t i = 0; i < cmd->nb_cmds; ++i) {
            free_command(cmd->cmds[i]);
        }
        free(cmd->cmds);
        cmd->cmds = NULL;
    }

    free(cmd);
}

static int validate_command_layout(const command_t *cmd) {
    if (!cmd) {
        errno = EINVAL;
        return 0;
    }

    int is_simple = (cmd->argc > 0);
    int is_sequence = (cmd->nb_cmds > 0);

    if (is_simple && is_sequence) {
        errno = EINVAL;
        return 0;
    }

    if (!is_simple && !is_sequence) {
        errno = EINVAL;
        return 0;
    }

    if (is_simple) {
        if (!cmd->argv) {
            errno = EINVAL;
            return 0;
        }
    } else {
        if (!cmd->cmds) {
            errno = EINVAL;
            return 0;
        }
    }
    return 1;
}

int write_command(int fd, const command_t *cmd) {
    if (!validate_command_layout(cmd)) {
        return -1;
    }

    if (write_uint16(fd, cmd->type) < 0) {
        return -1;
    }

    uint8_t kind = (cmd->nb_cmds > 0) ? CMD_SERIAL_KIND_SEQUENCE : CMD_SERIAL_KIND_SIMPLE;
    if (write_uint8(fd, kind) < 0) {
        return -1;
    }

    if (kind == CMD_SERIAL_KIND_SIMPLE) {
        if (write_arguments(fd, cmd->argc, cmd->argv) < 0) {
            return -1;
        }
        return 0;
    }

    if (write_uint32(fd, cmd->nb_cmds) < 0) {
        return -1;
    }

    for (uint32_t i = 0; i < cmd->nb_cmds; ++i) {
        if (!cmd->cmds[i]) {
            errno = EINVAL;
            return -1;
        }
        if (write_command(fd, cmd->cmds[i]) < 0) {
            return -1;
        }
    }
    return 0;
}

static void clean_partial_children(command_t **children, uint32_t count) {
    if (!children) return;
    for (uint32_t i = 0; i < count; ++i) {
        if (children[i]) {
            free_command(children[i]);
        }
    }
    free(children);
}

int read_command(int fd, command_t **cmd_out) {
    if (!cmd_out) {
        errno = EINVAL;
        return -1;
    }

    command_t *cmd = calloc(1, sizeof(command_t));
    if (!cmd) return -1;

    if (read_uint16(fd, &cmd->type) < 0) {
        free(cmd);
        return -1;
    }

    uint8_t kind;
    if (read_uint8(fd, &kind) < 0) {
        free(cmd);
        return -1;
    }

    if (kind == CMD_SERIAL_KIND_SIMPLE) {
        uint32_t argc = 0;
        char **argv = NULL;
        if (read_arguments(fd, &argc, &argv) < 0) {
            free(cmd);
            return -1;
        }
        cmd->argc = argc;
        cmd->argv = argv;
        cmd->nb_cmds = 0;
        cmd->cmds = NULL;
        *cmd_out = cmd;
        return 0;
    }

    if (kind != CMD_SERIAL_KIND_SEQUENCE) {
        free(cmd);
        errno = EBADMSG;
        return -1;
    }

    uint32_t nb_cmds = 0;
    if (read_uint32(fd, &nb_cmds) < 0) {
        free(cmd);
        return -1;
    }
    if (nb_cmds == 0) {
        free(cmd);
        errno = EPROTO;
        return -1;
    }

    command_t **children = calloc(nb_cmds, sizeof(command_t *));
    if (!children) {
        free(cmd);
        return -1;
    }

    for (uint32_t i = 0; i < nb_cmds; ++i) {
        if (read_command(fd, &children[i]) < 0) {
            clean_partial_children(children, i);
            free(cmd);
            return -1;
        }
    }

    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->nb_cmds = nb_cmds;
    cmd->cmds = children;
    *cmd_out = cmd;
    return 0;
}