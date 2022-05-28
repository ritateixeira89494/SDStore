#ifndef SDSTORE_REQUEST_H
#define SDSTORE_REQUEST_H

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define MAX_FILTER_NUMBER 32
#define MAX_FILENAME_LEN 128
#define MAX_FILTERNAME_LEN 128

typedef enum request_type {
    HANDSHAKE   = -2,
    TERMINATION = -1,
    TRANSFORM,
    STATUS
} RequestType;

//TODO: Verificar quais as caraterísticas necessárias
typedef struct request {
    pid_t       client_pid;
    RequestType request_type;
    size_t      number_filters;
    int         requested_filters[MAX_FILTER_NUMBER];
    char        input_file[MAX_FILENAME_LEN];
    char        output_file[MAX_FILENAME_LEN];
} Request;

#endif //SDSTORE_REQUEST_H
