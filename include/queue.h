#ifndef SDSTORE_QUEUE_H
#define SDSTORE_QUEUE_H

#include <stdlib.h>

#include "filtros.h"
#include "request.h"

#define BUF_SIZE 1024

typedef struct queue {
    Request*      request;
    struct queue* prox;
} Queue;

ssize_t readch(int fd, char* buf);

ssize_t readln(int fd, char* line, size_t size);

ssize_t read_file(char* filename);

Queue* init_queue(Request* request);

void add_request_to_queue(Request* request, Queue** last_request);

Request* remove_request(
        Queue* prev_queue, Queue* cur_queue, Queue** last_request);

bool valid_request_to_execute(Request* request, CatalogoFiltros* catalogo);

Request* can_execute_request(
        Queue* queue, CatalogoFiltros* catalogo, Queue** last_queue);

Request* remove_request_by_pid(
        Queue* queue, Queue** last_queue, pid_t client_pid);

#endif //SDSTORE_QUEUE_H
