#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


#define NO_TRNSF 7
#define BUFSIZE 1024


typedef struct request {
    int client_pid;
    char* tipo;
    char* input_file;
    char* output_file;
    // int no_trnsf;
    char* trnsfs[32];
} *Request;


typedef struct queue {
    Request* request;
    struct queue* prox;
} Queue;



typedef struct transform {
    char *tipo;
    int capacidade;
    int disp;
} *Transform;







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




int read_file(char* filename) {
  int  fd = open(filename, O_RDONLY);
  char line[BUFSIZE];
  int  total_read = 0, len = 0;
  while ((len = readln(fd, line, BUFSIZE)) > 0) {
    total_read += len;
    //write(1, line, len);
  }
  return total_read;
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


Request proc_request(char* line) {
        Request r = malloc(sizeof(struct request));

        
        char *aux, *aux1, *aux2, *aux3, *aux4, *token;
        
        token = strtok(line, " ");
        aux4 = strdup(token);
        r->client_pid = atoi(aux4);
        // r->no_trnsf = 0;

        
        
        token = strtok(NULL, " ");
        aux1 = strdup(token);
        r->tipo = aux1;

        
        if(strcmp(r->tipo, "status") == 0) {
            return r;
        }

        if(strcmp(r->tipo, "proc-file") == 0) {
           
            token = strtok(NULL, " ");
            aux2 = strdup(token);
            r->input_file = aux2;

        
        
            token = strtok(NULL, " ");
            aux3 = strdup(token);
            r->output_file = aux3;

            token = strtok(NULL, " ");

            
            int i=0;
            while(token != NULL) {
                aux = strdup(token);
                // printf("%s %d ", aux, i);
                r->trnsfs[i] = aux;
                // printf("%s", r->trnsfs[i]);

                token = strtok(NULL, " ");
            
                i++;
                // r->no_trnsf++;
            }
            // r->trnsfs[i+1] = NULL;
            // write(1, "a ", strlen("a "));

            return r;
        }
        return r;
}


void print_req(Request r) {

    char w[256];
    sprintf(w, "%d %s ", r->client_pid, r->tipo);
    
    if(strcmp(r->tipo, "status") == 0) {
        strcat(w, "\n");
        write(1, w, strlen(w));
        return ;
    }
    if(strcmp(r->tipo, "proc-file") == 0) {

        strcat(w, r->input_file);
        strcat(w, " ");

        strcat(w, r->output_file);
        strcat(w, " ");

        int i;
        for(i=0; r->trnsfs[i]; i++) {
            strcat(w, r->trnsfs[i]);
            strcat(w, " ");
        }

        strcat(w, "\n");
        write(1, w, strlen(w));
        return ;
    }
}


void init_queue (Request* request){
    Queue* queue = malloc(sizeof(struct queue));
    queue->request = request;
    queue->prox = NULL;
}



void adicionarPedidoQueue(Queue** pedidos, Request* request){
    if (pedidos == NULL) return;

    (*pedidos)->prox = malloc(sizeof (struct queue));
    *pedidos = (*pedidos)->prox;
    (*pedidos)->request = request;
}




void exec_req_noreq (char* trnsf_dir, char* trnsfs[], int trnsf_no, char* in_pathfile, char* out_pathfile) {

    int in_fd = open(in_pathfile, O_RDONLY);
    int fd[2];
    int p = pipe(fd);
     if(p == -1){
        perror("pipe");
    }

       

    for(int i=0; i<trnsf_no; i++) {
        if(i < trnsf_no-1) pipe(fd);
        if(!fork()) {
            dup2(in_fd, 0);
            close(in_fd);

            if(i < trnsf_no - 1) {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
            }
            else {
                int out_fd = open(out_pathfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                dup2(out_fd, 1);
                close(out_fd);
            }

            char* cmd = strdup(trnsf_dir); 
            strcat(cmd, trnsfs[i]);

            execlp(cmd, cmd, NULL);
            perror("execlp");
            _exit(1);
        }
        if (i < trnsf_no-1) close(fd[1]);
        close(in_fd);
        if (i < trnsf_no-1) in_fd = fd[0];

    }

    for (int i = 0; i < trnsf_no; i++) {
        wait(NULL);
    }
}

void exec_req_noreq_simple (char* trnsf_dir, char* tipo, char* in_pathfile, char* out_pathfile) {

    int in_fd = open(in_pathfile, O_RDONLY);
    int out_fd = open(out_pathfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(in_fd < 0 || out_fd < 0) {
        perror("abertura de um ou dos ficheiros a processar");
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

void exec_req (char* trnsf_dir, Request r) {

    int in_fd = open(r->input_file, O_RDONLY);
    int out_fd = open(r->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(in_fd < 0 || out_fd < 0) {
        perror("abertura de um ou dos ficheiros a processar");
    }

    char* cmd = strdup(trnsf_dir);
    strcat(cmd, r->trnsfs[0]);
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
    
    


    
    mkfifo("tmp/fifo_c2s", 0644);
    int fifo_c2s = open("tmp/fifo_c2s", O_RDONLY);

    int bytes_read;
    char buf[BUFSIZE];
    while(1) {
        if((bytes_read = readln(fifo_c2s, buf, BUFSIZE)) > 0) {
            // write(1, buf, strlen(buf));

            Request req = malloc(sizeof(struct request));
            char* to_proc = strdup(buf);

            req = proc_request(to_proc);

            // print_req(req);

            // exec_req(transform_path, req);

            // exec_req2(transform_path, "bcompress", "samples/sample-1-so.m4a", "outputs/sample-1-so-bcomp.m4a");
            // exec_req2(transform_path, "bdecompress", "outputs/sample-1-so-bcomp.m4a", "outputs/sample-1-so-bdecomp.m4a");


            char pid_pipe[16];
            sprintf(pipe, "tmp/pipe_%d", req->client_pid);

            /*
            mkfifo(pid_pipe, 0644);
            int fifo_s2c = open(pid_pipe, O_WRONLY);
            
            close(fifo_s2c);
            unlink(pid_pipe);
            */
        }
    }
    

    free_transforms(trnsf);
    close(fifo_c2s);
    unlink("tmp/fifo_c2s");

    return 0;
}