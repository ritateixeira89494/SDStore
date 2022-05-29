#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct command {
    char *tipo;
    int n_args;
    char* args;
} *Command;

typedef struct cmdlist {
    Command* cmds;
    int no_cmd;
} *CmdList;


void print_commands (CmdList cl) {
    for(int i=0; i<cl->no_cmd; i++) {
        char w[128];
        Command c = cl->cmds[i];

        

        write(1, "w", strlen(w));
    }
}







int main(int argc, const char* argv[]) {

    int fifo_c2s = open("tmp/fifo_c2s", O_WRONLY);
    if(fifo_c2s == -1) {
        perror("fifo_c2s");
    }

    if(argc == 1) {

    }

    char line[128] = "";
    int i;
    for(i=1; i < argc-1; i++) {
        strcat(line, argv[i]);
        strcat(line, " ");
    }
    strcat(line, argv[i]);


    write(1, line, strlen(line));
    write(fifo_c2s, line, strlen(line));
    

    return 0;
}