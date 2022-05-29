#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




int main(int argc, char* argv[]) {

    if(argc == 1) {
        write(1, "./sdstore status\n", strlen("./sdstore status\n"));
        write(1, "./sdstore proc-file input-filename output-filename transformation-id-1 transformation-id-2 ...\n", 
        strlen("./sdstore proc-file input-filename output-filename transformation-id-1 transformation-id-2 ...\n"));

        return 1;
    }

    int fifo_c2s = open("tmp/fifo_c2s", O_WRONLY);
    if(fifo_c2s == -1) {
        perror("fifo_c2s");
    }

    if(strcmp(argv[1], "status") != 0 && strcmp(argv[1], "proc-file") != 0) {
        perror("só podem ser executados os comandos 'status' e 'proc-file'");
    }

    if(strcmp("status", argv[1]) == 0 && argc != 2) {
        perror("erro no número de argumentos");

        return -1;
    }
    if(strcmp("proc-file", argv[1]) == 0 && argc < 5) {
        perror("erro no número de argumentos");

        return -1;
    }

    int pid = getpid();
    char w[256];
    sprintf(w, "%d ", pid);

    strcat(w, argv[1]);
    strcat(w, " ");
    
    for(int i=2; i<argc-1; i++) {
        strcat(w, argv[i]);
        strcat(w, " ");
    }
    strcat(w, argv[argc-1]);

    strcat(w, "\n");

    if(strcmp(argv[1], "proc-file") == 0) {
        int in_fd = open(argv[2], O_RDONLY);
        int out_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if(in_fd < 0 || out_fd < 0) {
            perror("abertura de um ou dos ficheiros a processar");

            return -1;
        }
    }

    // write(1, w, strlen(w));
    write(fifo_c2s, w, strlen(w));

    char pipe[16];
    sprintf(pipe, "tmp/pipe_%d", pid);

    return 0;
}