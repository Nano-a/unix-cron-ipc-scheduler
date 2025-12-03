#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include "serialization.h"

//OPCODES
#define OPCODE_LIST        0x4C53  /* 'LS' */
#define OPCODE_CREATE      0x4352  /* 'CR' */
#define OPCODE_COMBINE     0x4342  /* 'CB' */
#define OPCODE_REMOVE      0x524D  /* 'RM' */
#define OPCODE_TIMES       0x5458  /* 'TX' */
#define OPCODE_STDOUT      0x534F  /* 'SO' */
#define OPCODE_STDERR      0x5345  /* 'SE' */
#define OPCODE_TERMINATE   0x4B49  /* 'TM' */

//ANSTYPE + ERROR
#define ANSTYPE_OK         0x4F4B  /* 'OK' */
#define ANSTYPE_ERROR      0x4552  /* 'ER' */

#define ERR_NOTFOUND       0x4E46  /* 'NF' */
#define ERR_NOTRUN         0x4E52  /* 'NR' */

//REQUETES
typedef enum {
    REQ_LIST,
    REQ_CREATE,
    REQ_COMBINE,
    REQ_REMOVE,
    REQ_TIMES_EXITCODES,
    REQ_STDOUT,
    REQ_STDERR,
    REQ_TERMINATE
} request_kind_t;

/* Requête COMBINE */
typedef struct {
    timing_t timing;
    uint16_t type_comb;    /* TYPE <uint16> */
    uint32_t nbtasks;
    uint64_t *taskids;     /* nbtasks éléments */
} request_combine_t;

/* Requête CREATE */
typedef struct {
    timing_t timing;
    uint32_t argc;
    char **argv;
} request_create_t;

/* Requêtes avec uniquement un taskid : REMOVE, TX, SO, SE */
typedef struct {
    uint64_t taskid;
} request_taskid_t;

/* Structure principale request_t */
typedef struct {
    request_kind_t kind;
    uint16_t opcode;

    union {
        request_create_t  create;
        request_combine_t combine;
        request_taskid_t  taskid_req;
        /* LIST & TERMINATE n'ont pas de payload */
    };
} request_t;

//Réponses
/* Utilisé pour LIST */
typedef struct {
    uint64_t taskid;
    timing_t timing;
    char *commandline; /* string */
} list_taskinfo_t;

/* Utilisé pour TX */
typedef struct {
    int64_t time;
    uint16_t exitcode;
} exec_info_t;

/* Réponse principale */
typedef struct {
    uint16_t anstype;

    union {
        /* -------- OK responses -------- */
        struct {
            uint32_t nbtasks;
            list_taskinfo_t *tasks;
        } list_ok;

        struct {
            uint64_t taskid;
        } create_ok;

        struct {
            uint64_t taskid;
        } combine_ok;

        struct {
            uint32_t nbruns;
            exec_info_t *runs;
        } times_ok;

        struct {
            char *output;
        } output_ok; /* stdout + stderr */

        /* -------- ERROR response -------- */
        struct {
            uint16_t errcode;
        } error;
    };
} response_t;

//Fonctions protocol
int send_request(int fd, const request_t *req);
int receive_request(int fd, request_t **out);

int send_response(int fd, const response_t *res);
int receive_response(int fd, response_t **out);

void free_request(request_t *req);
void free_response(response_t *res);

#endif // PROTOCOL_H