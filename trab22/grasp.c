#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "common.c"

int debug = 0;

void construct(int size, int graph[size][size], int colors, int col[colors], int iteration) {
    /* Etapa de construção do GRASP */

    //printf("it: %i\n", iteration);

    int col2[colors];
    int rcl[colors];
    int alpha = 0;
    for(int i = 0; i < colors; ++i) { rcl[i] = 0; }

    if(iteration > 2) {
        for(int i = 0; i < colors; ++i) { rcl[i] = 1; ++alpha; }
        col[rand() % colors] = 1;
    }

    while(comp(size, graph, colors, col) > 1) {
        // Adicionar candidatos que minimizam comp(c)
        int comp_val;
        int comp_min = size;
        for(int i = 0; i < colors; ++i) {
            for(int j = 0; j < colors; ++j) { col2[j] = col[j]; }
            col2[i] = 1;
            comp_val = comp(size, graph, colors, col2);
            if(comp_val < comp_min) {
                comp_min = comp_val;
                for(int j = 0; j < colors; ++j) { rcl[j] = 0; }
                rcl[0] = i;
                alpha = 1;
            } else if(comp_val == comp_min) {
                rcl[alpha] = i;
                ++alpha;
            }
        }

        col[rcl[rand() % alpha]] = 1;
    }
}

void grasp(int size, int graph[size][size], int colors, int col[colors], int col_star[colors]) {
    /* GRASP */

    int iteration = 1;
    int no_improv = 0;

    while(no_improv < 1) {
        //printf("---\n");
        //printf("no_improv: %i (%i)\n", no_improv, card(colors, col_star));
        for(int i = 0; i < colors; ++i) { col[i] = 0; }
        construct(size, graph, colors, col, iteration);
        //vns(size, graph, colors, col);
        local(size, graph, colors, col);
        if(card(colors, col) < card(colors, col_star)) {
            for(int j = 0; j < colors; ++j) { col_star[j] = col[j]; }
            no_improv = 0;
        } else {
            ++no_improv;
        }
        /*
        for(int i = 0; i < colors; ++i) { printf("%i | ", col[i]); }
        printf("\n");
        */
        //printf("it sol card: %i\n", card(colors, col));

        ++iteration;
    }
}

int main(int argc, char **argv) {
    // Tamanho e cores do problema a serem lidos do arquivo de entrada
    int size, colors, c;
    bool plot = 0;
    char* input_file = NULL;

    while((c = getopt(argc, argv, "f:dp")) != -1) {
        switch (c) {
            case 'f':
                input_file = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            case 'p':
                plot = 1;
                break;
            return 1;
            case '?':
                if (optopt == 'f') {
                    fprintf (stderr, "A opção -%c requer um nome de arquivo.\n", optopt);
                    exit(1);
                } else if (isprint(optopt)) {
                    fprintf (stderr, "Opção `-%c' desconhecida.\n", optopt);
                } else {
                    fprintf (stderr, "Erro de argumentos.\n");
                }
            default:
                abort();
        }
    }

    FILE* fp;
    if((fp = fopen(input_file, "r")) == 0){
        printf("Arquivo não informado ou erro na leitura\n");
        printf("Formato esperado: -f [nome do arquivo] -dp\n");
        exit(1);
    }

    // Lê do arquivo o tamanho do grafo e a quantidade de cores
    fscanf(fp, "%i %i", &size, &colors);

    // Separa espaço na memória para o grafo e soluções temporárias
    int graph[size][size];
    int col[colors];
    int col_star[colors];

    int count_edges = 0;
    // Lê grafo do arquivo
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            fscanf(fp, "%i", &graph[i][j]);
            if((j > i && graph[i][j] != -1)) {
                ++count_edges;
            }
        }
    }

    if(debug) {
        printf("size: %i\n", size);
        printf("colors: %i\n", colors);
        printf("edges: %i\n", count_edges);
    }

    for(int i = 0; i < colors; ++i) {
        col[i] = 0;
        col_star[i] = 1;
    }

    // Seed com o tempo
    //srand(time(NULL));
    srand(mix(clock(), time(NULL), getpid()));
    if(debug) printf("rand_test: %i\n", rand());

    // Clock inicial para mensurar diversos tempos de execução
    clock_t begin = clock();

    grasp(size, graph, colors, col, col_star);

    if(debug) {
        printf("---\n");
        printf("Solução (%i):\n", card(colors, col_star));

        for(int i = 0; i < colors; ++i) {
            printf("%i | ", col_star[i]);
        }
        printf("\n");
    } else {
        printf("%i ", card(colors, col_star));
    }

    int span[size];
    int visited[size];
    for(int i = 0; i < size; ++i) {
        span[i] = -1;
        visited[i] = 0;
    }

    visited[0] = 1;
    span[0] = 0;
    dfs2(size, graph, colors, col_star, visited, span, 0);

    // Clock final para tempo total do algoritmo
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    if(debug) {
        printf("time: %fms\n", time_spent * 1000);
    } else {
        printf("%f\n", time_spent * 1000);
    }

    if(plot) {
        plot_initial(size, graph);
        plot_solution(size, graph, colors, col, span);
    }

    return 0;
}
