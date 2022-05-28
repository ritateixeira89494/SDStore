#include "../include/transformacoes.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "../include/request.h"

char *config_path, *filter_path;

Transformacao* init_filtro(
        char* identificador, char* ficheiro_executavel, size_t max_instancias) {

    size_t      len_filter_path = strlen(filter_path);
    size_t      used = len_filter_path + strlen(ficheiro_executavel) + 1;
    struct stat buffer;
    char        path_executavel[used];
    // sprintf(filter_path, "%s", ficheiro_executavel);
    sprintf(path_executavel, "%s/%s", filter_path, ficheiro_executavel);
    // fprintf(stderr, "%s\n", path_executavel);
    int valid = 0;
    valid     = stat(path_executavel, &buffer);

    Transformacao* transformacao = (Transformacao*) malloc(sizeof(struct transformacao));
    *transformacao        = (struct transformacao){
            .identificador       = strdup(identificador),
            .ficheiro_executavel = strdup(path_executavel),
            .max_instancias      = max_instancias,
            .em_processamento    = 0};

    // verificar se é valido
    if (valid != 0) {
        fprintf(stderr, "\nError read filtro:\n");
        show_filtro(transformacao);
        fprintf(stderr, "File path not found: %s\n", path_executavel);
        free_filtro(transformacao);
        transformacao = NULL;
    }
    return transformacao;
}

void free_filtro(Transformacao* transformacao) {
    if (!transformacao) return;
    free(transformacao->identificador);
    free(transformacao->ficheiro_executavel);
    free(transformacao);
}

static Transformacao* parse_filtro(char* string) {
    if (!string) return NULL;
    char* identificador       = strsep(&string, " ");
    char* ficheiro_executavel = strsep(&string, " ");
    char* max_instancias      = strsep(&string, " ");

    if (!max_instancias) return NULL;
    int max_instancias_valor = atoi(max_instancias);

    // if (1 != scanf(max_instancias, "%zu", &max_instancias_valor)) return
    // NULL;
    return init_filtro(identificador, ficheiro_executavel, max_instancias_valor);
}

void show_filtro(Transformacao* filtro) {
    if (!filtro) return;
    printf("FILTRO\n");
    printf("\tidentificador: %s\n", filtro->identificador);
    printf("\tficheiro executavel: %s\n", filtro->ficheiro_executavel);
    printf("\tmáximo de instancias: %zu\n", filtro->max_instancias);
    printf("\tem processamento: %zu\n", filtro->em_processamento);
}

/* Catalogo de filtros completo */

static void add_filtro_catalogo(CatalogoTransformacoes* catalogo, Transformacao* filtro) {
    if (!filtro) return;

    catalogo->filtros[catalogo->used] = filtro;
    catalogo->used++;
}

CatalogoTransformacoes* init_catalogo_fitros(
        char* all_filters_string, size_t size, size_t size_used) {
    int fd;
    if ((fd = open(config_path, O_RDONLY)) < 0 && !all_filters_string)
        return NULL;

    char*            line          = malloc(BUF_SIZE * (sizeof(char)));
    size_t           size_catalogo = 0;
    CatalogoTransformacoes* catalogo      = NULL;

    size_t total_read;
    while (size_catalogo < MAX_FILTER_NUMBER &&
           (total_read = readln(fd, line, BUF_SIZE)) > 0) {
        Transformacao* transformacao = parse_filtro(line);

        if (!transformacao) continue;
        if (catalogo) {
            add_filtro_catalogo(catalogo, transformacao);
        }
        else {
            catalogo = (CatalogoTransformacoes*) malloc(sizeof(struct catalogo_transformacoes));
            catalogo->filtros[0] = transformacao;
            catalogo->used       = 1;
        }

        // add string
        if (transformacao) {
            char*  name_filtro     = transformacao->identificador;
            size_t size_need       = strlen(name_filtro) + strlen(";");
            char*  add_string_name = malloc(size_need * sizeof(char));
            strcat(add_string_name, name_filtro);
            strcat(add_string_name, ";");

            if (size < (size_used + size_need)) {
                size               = 2 * size + size_need;
                all_filters_string = realloc(all_filters_string, size);
            }
            size_used += size_need;
            strcat(all_filters_string, strdup(add_string_name));
            free(add_string_name);

            size_catalogo++;
        }
    }
    if (size_used > 0) all_filters_string[size_used - 1] = '\n';
    close(fd);
    free(line);
    return catalogo;
}

Transformacao* search_filtro(CatalogoTransformacoes* catalogo, char* name) {
    Transformacao* transformacao = NULL;
    bool    find   = false;
    size_t  size   = catalogo->used;
    if (catalogo && name) {
        for (int i = 0; i < size && !find; i++) {
            if (catalogo->filtros[i]) {
                find   = !strcmp(catalogo->filtros[i]->identificador, name);
                transformacao = catalogo->filtros[i];
            }
        }
    }
    return transformacao;
}

bool valid_filtro(
        CatalogoTransformacoes* catalogo, char* name_filtro) {  // verifica se é valido
    Transformacao* transformacao = search_filtro(catalogo, name_filtro);
    return transformacao != NULL && !strcmp(transformacao->identificador, name_filtro);
}

// so aumenta se os em processamento não forem igual ao valor do max_instancias
void increase_number_filtro(CatalogoTransformacoes* catalogo, size_t indice) {
    if (catalogo && indice < catalogo->used) {
        Transformacao* transformacao = catalogo->filtros[indice];
        transformacao->em_processamento++;
        if (transformacao->em_processamento > transformacao->max_instancias) {
            transformacao->em_processamento = transformacao->max_instancias;
        }
    }
    // return !result;
}

// so diminui se os em processamento não forem igual ao valor do max_instancias
void decrease_number_filtro(CatalogoTransformacoes* catalogo, size_t indice) {
    if (catalogo && indice < catalogo->used) {
        Transformacao* transformacao = catalogo->filtros[indice];
        if (transformacao->em_processamento == 1 || transformacao->em_processamento == 0)
            transformacao->em_processamento = 0;
        else
            transformacao->em_processamento--;
    }
}

static void free_array_filtros(Transformacao* filtros[], size_t size) {
    for (int i = 0; i < size; i++) {
        free_filtro(filtros[i]);
    }
}

void free_catalogo_filtros(CatalogoTransformacoes* catalogo) {
    if (!catalogo) return;
    free_array_filtros(catalogo->filtros, catalogo->used);
    free(catalogo);
}

void show_catalogo(CatalogoTransformacoes* catalogo) {
    if (!catalogo) return;
    size_t size = catalogo->used;
    for (int i = 0; i < size; i++) {
        printf("\n%d: ", i);
        show_filtro(catalogo->filtros[i]);
    }
}

void show_one_filtro(CatalogoTransformacoes* catalogo, char* name) {
    show_filtro(search_filtro(catalogo, name));
}

void update_catalogo_done_request(CatalogoTransformacoes* catalogo, Request request) {
    if (request.request_type != TRANSFORM) return;
    size_t size = request.number_filters;
    for (size_t i = 0; i < size; i++) {
        decrease_number_filtro(catalogo, request.requested_filters[i]);
    }
}
void update_catalogo_execute_request(
        CatalogoTransformacoes* catalogo, Request request) {
    if (request.request_type != TRANSFORM) return;
    size_t size = request.number_filters;
    for (size_t i = 0; i < size; i++) {
        increase_number_filtro(catalogo, request.requested_filters[i]);
    }
}

void update_fake_request(CatalogoTransformacoes* catalogo, Request* fake_request) {
    if (!catalogo) return;

    size_t size = catalogo->used;
    for (int i = 0; i < size; i++)
        fake_request->requested_filters[i] = catalogo->filtros[i]->em_processamento;
}