#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <stdint.h>
#include <unistd.h>

// Types

// Timing
typedef struct {
    uint64_t minutes;
    uint32_t hours;
    uint8_t daysofweek;
} timing_t;

// Command
typedef struct command {
    uint16_t type;         
    /* commande simple */
    uint32_t argc;         
    char **argv;           

    /* combinaison de commandes */
    uint32_t nb_cmds;      
    struct command **cmds; 
} command_t;

// Fonctions pour écrire des entiers non signés (big-endian)
int write_uint8(int fd, uint8_t value);
int write_uint16(int fd, uint16_t value);
int write_uint32(int fd, uint32_t value);
int write_uint64(int fd, uint64_t value);

// Fonctions pour lire des entiers non signés (big-endian)
int read_uint8(int fd, uint8_t *value);
int read_uint16(int fd, uint16_t *value);
int read_uint32(int fd, uint32_t *value);
int read_uint64(int fd, uint64_t *value);

// Fonctions pour écrire/lire des entiers signés 64 bits
int write_int64(int fd, int64_t value);
int read_int64(int fd, int64_t *value);

// Lire et écrire
int write_string(int fd, const char *str);
int read_string(int fd, char **out);

// Timing MINUTES <uint64>, HOURS <uint32>, DAYSOFWEEK <uint8>
int write_timing(int fd, const timing_t *t);
int read_timing(int fd, timing_t *t);

// Arguments
int write_arguments(int fd, uint32_t argc, char *argv[]);
int read_arguments(int fd, uint32_t *argc_out, char ***argv_out);

// Créer commandes et libérer espace
command_t *create_simple_command(const char *type2, uint32_t argc, char *argv[]);
command_t *create_sequence_command(const char *type2, uint32_t nb_cmds, command_t **cmds);
void free_command(command_t *cmd);

/* write/read (récursif) */
int write_command(int fd, const command_t *cmd);
int read_command(int fd, command_t **cmd_out);

/* fonction helper 2-char type string en uint16_t (sans malloc) */
static inline uint16_t type_from_str(const char *s) {
    return (uint16_t)(((unsigned char)s[0] << 8) | (unsigned char)s[1]);
}

#endif // SERIALIZATION_H
