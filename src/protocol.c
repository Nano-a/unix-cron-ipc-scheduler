#include "protocol.h"
#include "serialization.h"
#include <stdlib.h>
#include <errno.h>

//Envoie requêtes
int send_request(int fd, const request_t *req) {
    if (!req) { errno = EINVAL; return -1; }

    if (write_uint16(fd, req->opcode) < 0)
        return -1;

    switch (req->kind) {

    //List
    case REQ_LIST:
    case REQ_TERMINATE:
    
        return 0;

    //Create
    case REQ_CREATE: {
        if (write_timing(fd, &req->create.timing) < 0)
            return -1;
        if (write_arguments(fd, req->create.argc, req->create.argv) < 0)
            return -1;
        return 0;
    }

    //Combine
    case REQ_COMBINE: {
        const request_combine_t *c = &req->combine;
        if (write_timing(fd, &c->timing) < 0)
            return -1;
        if (write_uint16(fd, c->type_comb) < 0)
            return -1;
        if (write_uint32(fd, c->nbtasks) < 0)
            return -1;

        for (uint32_t i = 0; i < c->nbtasks; i++) {
            if (write_uint64(fd, c->taskids[i]) < 0)
                return -1;
        }
        return 0;
    }

    case REQ_REMOVE:
    case REQ_TIMES_EXITCODES:
    case REQ_STDOUT:
    case REQ_STDERR:
        if (write_uint64(fd, req->taskid_req.taskid) < 0)
            return -1;
        return 0;

    default:
        errno = EPROTO;
        return -1;
    }
}

//réception des requêtes
int receive_request(int fd, request_t **out) {
    if (!out) { errno = EINVAL; return -1; }
    *out = NULL;

    request_t *req = calloc(1, sizeof(request_t));
    if (!req) return -1;

    uint16_t opcode;
    if (read_uint16(fd, &opcode) < 0) {
        free(req);
        return -1;
    }
    req->opcode = opcode;

    switch (opcode) {
    case OPCODE_LIST:        req->kind = REQ_LIST; break;
    case OPCODE_CREATE:      req->kind = REQ_CREATE; break;
    case OPCODE_COMBINE:     req->kind = REQ_COMBINE; break;
    case OPCODE_REMOVE:      req->kind = REQ_REMOVE; break;
    case OPCODE_TIMES:       req->kind = REQ_TIMES_EXITCODES; break;
    case OPCODE_STDOUT:      req->kind = REQ_STDOUT; break;
    case OPCODE_STDERR:      req->kind = REQ_STDERR; break;
    case OPCODE_TERMINATE:   req->kind = REQ_TERMINATE; break;
    default:
        free(req);
        errno = EPROTO;
        return -1;
    }

    switch (req->kind) {

    //List
    case REQ_LIST:
    case REQ_TERMINATE:
        *out = req;    /* pas de payload */
        return 0;

    //create
    case REQ_CREATE:
        if (read_timing(fd, &req->create.timing) < 0) {
            free(req);
            return -1;
        }
        if (read_arguments(fd, &req->create.argc, &req->create.argv) < 0) {
            free(req);
            return -1;
        }
        *out = req;
        return 0;

    //combine
    case REQ_COMBINE: {
        request_combine_t *c = &req->combine;

        if (read_timing(fd, &c->timing) < 0) {
            free(req);
            return -1;
        }
        if (read_uint16(fd, &c->type_comb) < 0) {
            free(req);
            return -1;
        }
        if (read_uint32(fd, &c->nbtasks) < 0) {
            free(req);
            return -1;
        }
        if (c->nbtasks > 0) {
            c->taskids = malloc(c->nbtasks * sizeof(uint64_t));
            if (!c->taskids) { free(req); return -1; }
            for (uint32_t i = 0; i < c->nbtasks; i++) {
                if (read_uint64(fd, &c->taskids[i]) < 0) {
                    free(c->taskids); free(req);
                    return -1;
                }
            }
        }
        *out = req;
        return 0;
    }

    
    case REQ_REMOVE:
    case REQ_TIMES_EXITCODES:
    case REQ_STDOUT:
    case REQ_STDERR:
        if (read_uint64(fd, &req->taskid_req.taskid) < 0) {
            free(req);
            return -1;
        }
        *out = req;
        return 0;

    default:
        free(req);
        errno = EPROTO;
        return -1;
    }
}


//envoie réponses
int send_response(int fd, const response_t *res) {
    if (!res) { errno = EINVAL; return -1; }

    if (write_uint16(fd, res->anstype) < 0)
        return -1;

    //erreur
    if (res->anstype == ANSTYPE_ERROR) {
        if (write_uint16(fd, res->error.errcode) < 0)
            return -1;
        return 0;
    }

    //OK
    switch (res->anstype) {

    case ANSTYPE_OK:
        break;

    default:
        errno = EPROTO;
        return -1;
    }

    // type OK dépend de la réponse d’origine 

    // LIST OK 
    if (res->list_ok.tasks) {
        if (write_uint32(fd, res->list_ok.nbtasks) < 0)
            return -1;

        for (uint32_t i = 0; i < res->list_ok.nbtasks; i++) {
            const list_taskinfo_t *t = &res->list_ok.tasks[i];
            if (write_uint64(fd, t->taskid) < 0) return -1;
            if (write_timing(fd, &t->timing) < 0) return -1;
            if (write_string(fd, t->commandline) < 0) return -1;
        }
        return 0;
    }

    // CREATE OK 
    if (res->create_ok.taskid) {
        return write_uint64(fd, res->create_ok.taskid);
    }

    // COMBINE OK 
    if (res->combine_ok.taskid) {
        return write_uint64(fd, res->combine_ok.taskid);
    }

    // TIMES OK 
    if (res->times_ok.runs) {
        if (write_uint32(fd, res->times_ok.nbruns) < 0)
            return -1;

        for (uint32_t i = 0; i < res->times_ok.nbruns; i++) {
            if (write_int64(fd, res->times_ok.runs[i].time) < 0)
                return -1;
            if (write_uint16(fd, res->times_ok.runs[i].exitcode) < 0)
                return -1;
        }
        return 0;
    }

    if (res->output_ok.output) {
        return write_string(fd, res->output_ok.output);
    }

    //rien d'autre à écrire
    return 0;
}


//réception réponses
int receive_response(int fd, response_t **out) {
    if (!out) { errno = EINVAL; return -1; }
    *out = NULL;

    response_t *res = calloc(1, sizeof(response_t));
    if (!res) return -1;

    if (read_uint16(fd, &res->anstype) < 0) {
        free(res);
        return -1;
    }

    //erreur?
    if (res->anstype == ANSTYPE_ERROR) {
        if (read_uint16(fd, &res->error.errcode) < 0) {
            free(res);
            return -1;
        }
        *out = res;
        return 0;
    }

    uint32_t n;
    if (read_uint32(fd, &n) == 0) {
        res->list_ok.nbtasks = n;
        res->list_ok.tasks = calloc(n, sizeof(list_taskinfo_t));
        if (!res->list_ok.tasks) { free(res); return -1; }

        for (uint32_t i = 0; i < n; i++) {
            if (read_uint64(fd, &res->list_ok.tasks[i].taskid) < 0) return -1;
            if (read_timing(fd, &res->list_ok.tasks[i].timing) < 0) return -1;
            if (read_string(fd, &res->list_ok.tasks[i].commandline) < 0) return -1;
        }
        *out = res;
        return 0;
    }

    if (read_uint64(fd, &res->create_ok.taskid) == 0) {
        *out = res;
        return 0;
    }

    free(res);
    errno = EPROTO;
    return -1;
}


//libérer mémoire
void free_request(request_t *req) {
    if (!req) return;

    switch (req->kind) {
        case REQ_CREATE:
            if (req->create.argv) {
                for (uint32_t i = 0; i < req->create.argc; ++i) {
                    free(req->create.argv[i]);
                }
                free(req->create.argv);
                req->create.argv = NULL;
            }
            req->create.argc = 0;
            break;

        case REQ_COMBINE:
            if (req->combine.taskids) {
                free(req->combine.taskids);
                req->combine.taskids = NULL;
            }
            req->combine.nbtasks = 0;
            break;

        case REQ_REMOVE:
        case REQ_TIMES_EXITCODES:
        case REQ_STDOUT:
        case REQ_STDERR:
           
            break;

        case REQ_LIST:
        case REQ_TERMINATE:
        
            break;
    }

    free(req);
}

void free_response(response_t *res) {
    if (!res) return;

    if (res->anstype == ANSTYPE_OK) {
        switch (res->anstype) {
            case ANSTYPE_OK:
                // LIST
                if (res->list_ok.tasks) {
                    for (uint32_t i = 0; i < res->list_ok.nbtasks; ++i) {
                        free(res->list_ok.tasks[i].commandline);
                    }
                    free(res->list_ok.tasks);
                    res->list_ok.tasks = NULL;
                }
                res->list_ok.nbtasks = 0;
                break;
        }

    
        if (res->times_ok.runs) {
            free(res->times_ok.runs);
            res->times_ok.runs = NULL;
            res->times_ok.nbruns = 0;
        }

        if (res->output_ok.output) {
            free(res->output_ok.output);
            res->output_ok.output = NULL;
        }

    } else if (res->anstype == ANSTYPE_ERROR) {
    }

    free(res);
}