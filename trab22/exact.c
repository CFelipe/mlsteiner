#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "common.c"

int debug = 0;
clock_t init_clock = 0;
clock_t shaking_time = 0;
clock_t local_time = 0;

void test(int size, int graph[size][size], int colors, int col[colors], int col_star[colors]) {
    double time_spent = (double) (clock() - init_clock) / CLOCKS_PER_SEC;

    if(time_spent > 3.0f) {
        if(debug) printf("Tempo limite ultrapassado\n");
        printf("-1 -1\n");
        exit(0);
    } else {
        //if(debug) printf("ts: %f\n", time_spent);
    }

    if(card(colors, col) < card(colors, col_star)) {
        int comp_val = comp(size, graph, colors, col);
        if(comp_val == 1) {
            for(int i = 0; i < colors; ++i) {
                col_star[i] = col[i];
            }
        } else if(card(colors, col) < card(colors, col_star) - 1) {
            for(int i = 0; i < colors; ++i) {
                if(col[i] == 0) {
                    col[i] = 1;
                    test(size, graph, colors, col, col_star);
                    col[i] = 0;
                }
            }
        }
    }

}

int main(int argc, char **argv) {
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

    int color;

    // Seed com o tempo
    srand(mix(clock(), time(NULL), getpid()));
    if(debug) printf("rand_test: %i\n", rand());

    init_clock = clock();

    test(size, graph, colors, col, col_star);

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

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    if(debug) {
        printf("time: %fms\n", time_spent * 1000);
    } else {
        printf("%f\n", time_spent * 1000);
    }

    out(input_file, size, graph, colors, col_star);

    if(plot) {
        plot_initial(size, graph);
        plot_solution(size, graph, colors, col, span);
    }

    return 0;
}
