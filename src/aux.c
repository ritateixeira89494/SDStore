#include "../include/aux.h"
#include "../include/sdstored.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char* handshake(){
    char fifoClienteParaServidor[1024];

    Request handshake = { handshake->client_pid = getpid(), handshake->msg_type = HANDSHAKE};

    sprintf(fifoClienteParaServidor, "pipe_%d", handshake->client_pid);
    mkfifo(fifoClienteParaServidor, 0666);

    int clienteParaServidor = open("Cliente a Servidor", O_WRONLY);
    write(clienteParaServidor, &handshake, sizeof(struct request));

    int servidorParaCliente = open(fifoClienteParaServidor, O_RDONLY);
    char* transformacoes = malloc(sizeof (char) * 1024);
    read(servidorParaCliente, transformacoes, 1024);

    return transformacoes;
}