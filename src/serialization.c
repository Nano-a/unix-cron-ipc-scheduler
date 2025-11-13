// TODO: À compléter par la personne responsable de la sérialisation
// Référence : serialisation.md, ARCHITECTURE_T1.1.md

#define _DEFAULT_SOURCE
#include "serialization.h"
#include <endian.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>      // printf, perror
#include <fcntl.h>      // open, O_CREAT, O_WRONLY, O_TRUNC, O_RDONLY


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
    if (rd != sizeof(uint16_t)) return -1;
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
    if (rd != sizeof(int64_t)) return -1;
    *value = (int64_t)be64toh((uint64_t)tmp);
    return 0;
}
