//
// Created by rita on 29/05/22.
//

#ifndef SDSTORE_SDSTORED_H
#define SDSTORE_SDSTORED_H

#define MAX_FILENAME_LEN 128

typedef struct transform {
    char *tipo;
    int capacidade;
    int disp;
} *Transform;

typedef enum request_type {
    HANDSHAKE   = -2,
    TERMINATION = -1,
    TRANSFORM,
    STATUS
} RequestType;

typedef struct request {
    int client_pid;
    RequestType msg_type;
    int no_trnsf;
    char        input_file[MAX_FILENAME_LEN];
    char        output_file[MAX_FILENAME_LEN];
} *Request;

typedef struct queue {
    Request* request;
    struct queue* prox;
}Queue;


int readln(int fd, char *line, size_t size);
void init_transform(char* config_file, Transform trnsf[]);
void show_transform(Transform t);
void show_transforms(Transform* t);
void free_transforms(Transform *t);
void init_queue (Request* request);
void adicionarPedidoQueue(Queue** pedidos, Request* request);
void exec_req (char* trnsf_dir, char* tipo, char* in_pathfile, char* out_pathfile);

#endif //SDSTORE_SDSTORED_H
