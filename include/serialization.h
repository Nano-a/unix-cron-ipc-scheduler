#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <stdint.h>
#include <unistd.h>

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

#endif // SERIALIZATION_H
