#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "protocol.h"

int main() {
    const char *request_fifo = "/tmp/run/request";
    const char *reply_fifo   = "/tmp/run/reply";

    // 1️⃣ Préparer la requête
    request_t req;
    memset(&req, 0, sizeof(req));
    req.opcode = 0xFFFF; // Exemple : OPCODE_TERMINATE ou autre code valide

    // 2️⃣ Envoyer la requête
    int fd_req = open(request_fifo, O_WRONLY);
    if (fd_req < 0) {
        perror("open request fifo");
        return 1;
    }

    ssize_t n = write(fd_req, &req, sizeof(req));
    if (n != sizeof(req)) {
        perror("write request");
        close(fd_req);
        return 1;
    }
    close(fd_req);
    printf("Requête envoyée\n");

    // 3️⃣ Lire la réponse
    // Ouvrir le FIFO reply **en lecture seule** (ne pas faire O_RDWR)
    int fd_reply = open(reply_fifo, O_RDWR);
    if (fd_reply < 0) {
        perror("open reply fifo");
        return 1;
    }

    response_t resp;
    n = read(fd_reply, &resp, sizeof(resp));
    if (n <= 0) {
        perror("read reply fifo");
        close(fd_reply);
        return 1;
    }
    close(fd_reply);

    printf("Réponse reçue: anstype = %d\n", resp.anstype);

    return 0;
}
