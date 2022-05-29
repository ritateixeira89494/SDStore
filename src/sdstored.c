#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


#define NO_TRNSF 7
#define BUFSIZE 1024


typedef struct transform {
    char *tipo;
    int capacidade;
    int disp;
} *Transform;

typedef struct request {
    int client_pid;
    int msg_type;
    int no_trnsf;
} *Request;



int readln(int fd, char *line, size_t size) {
	int bytes_read;
	int i = 0;

	while (i < size) {
		if(((bytes_read = read(fd, &line[i], 1)) < 1) || line[i] == '\n' || line[i] == '\0')
			break;
		i++;

	}
	return i;
}



void init_transform(char* config_file, Transform trnsf[]) {

    int bytes_read;
    char buf[BUFSIZE];

    int cfg = open(config_file, O_RDONLY);
    if(cfg < 0) {
        perror("abertura do ficheiro de configuração");
    }

    for(int i=0; (bytes_read = readln(cfg, buf, BUFSIZE)) > 0; i++) {
        char *aux[2];

        char *token;
        token = strtok(buf, " ");
        aux[0] = strdup(token);
        token = strtok(NULL, " ");
        aux[1] = strdup(token);


        trnsf[i] = malloc(sizeof(struct transform));

        trnsf[i]->tipo = aux[0];
        trnsf[i]->capacidade = atoi(aux[1]);
        trnsf[i]->disp = trnsf[i]->capacidade;
    }
    close(cfg);
}

void show_transform(Transform t) {
    if(!t) {
        write(1, "transform não existe\n", strlen("transform não existe\n"));
        return ;
    }
    char w[128];
    sprintf(w, "  %s, capacidade %d\n", t->tipo, t->capacidade);
    write(1, w, strlen(w));
}

void show_transforms(Transform* t) {
    write(1, "transformações: \n", strlen("transformações: \n"));
    for(int i=0; i<NO_TRNSF; i++) {
        show_transform(t[i]);
    }
}

void free_transforms(Transform *t) {
    for(int i=0; i<NO_TRNSF; i++) {
        free(t[i]->tipo);
        free(t[i]);
    }
}




void exec_req (char* trnsf_dir, char* tipo, char* in_pathfile, char* out_pathfile) {

    int in_fd = open(in_pathfile, O_RDONLY);
    int out_fd = open(out_pathfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (in_fd < 0 || out_fd < 0) {
        perror("erro na pathfile do ficheiro a processar ou processado");
        return ;
    }

    char* cmd = strdup(trnsf_dir);
    strcat(cmd, tipo);
    // write(1, search, strlen(search));

    int fd[2];
    int p = pipe(fd);
     if(p == -1){
        perror("pipe");
    }

    if(!fork()) {

        dup2(in_fd, 0);
        dup2(out_fd, 1);

        execlp(cmd, cmd, NULL);

        perror("execlp");
        _exit(1);
    }
    else {
        wait(NULL);
    }
}





int main(int argc, char* argv[]) {

    char* config_file, *transform_path;
    Transform trnsf[NO_TRNSF];

    if(argc == 3) {
        config_file = argv[1];
        transform_path = argv[2];
    }
    else {
        char *msg = "Wrong number of arguments\n";
        write(1, msg, strlen(msg));
        return -1;
    }

    init_transform(config_file, trnsf);
    
    show_transforms(trnsf);
    
    // exec_req(transform_path, "nop", "samples/sample-1-so.m4a", "outputs/sample-2-so.m4a");


    
    mkfifo("tmp/fifo_c2s", 0644);
    int fifo_c2s = open("tmp/fifo_c2s", O_RDONLY);

    int bytes_read;
    char buf[BUFSIZE];
    while(1) {
        if((bytes_read = readln(fifo_c2s, buf, BUFSIZE)) > 0) {
            write(1, buf, strlen(buf));
        }
    }
    

    free_transforms(trnsf);
    unlink("tmp/fifo_c2s");

    return 0;
}