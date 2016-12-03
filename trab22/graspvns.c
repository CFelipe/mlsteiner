#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "common.c"

bool debug = 0;
bool track = 0;
clock_t count_zero = 0;
double construct_time = 0;
double shaking_time = 0;
double local_time = 0;
double total_time = 0;

void shaking(int size, int graph[size][size], int colors, int col[colors], int k) {
    /* Etapa de shaking do VNS */

    int used[colors];
    int used_count = 0;
    int unused_count = 0;
    int card_col = card(colors, col);
    int i, j, z;

    // Coloca índices das cores usadas no começo da matriz used e não usadas no
    // final. Para diferenciar os usados de não usados, no final do laço,
    // used_count contém o valor do índice da última cor usada
    for(i = 0; i < colors; ++i) {
        if(col[i] == 1) {
            used[used_count] = i;
            ++used_count;
        } else {
            used[colors - unused_count - 1] = i;
            ++unused_count;
        }
    }

    // Remove e adiciona cores
    for(i = 1; i < k; ++i) {
        if(i <= card_col) {
            // Remove cor utilizada
            col[used[rand() % used_count]] = 0;
        } else {
            // Adiciona cor não utilizada
            col[used[used_count + (rand() % unused_count)]] = 1;
        }
    }

    int col2[colors];
    int rcl[colors];
    int alpha = 0;
    for(i = 0; i < colors; ++i) { rcl[i] = 0; }

    int comp_val;
    int comp_min = size;
    count_zero = clock();
    // Adiciona melhores candidatos até obter grafo conexo
    while(comp(size, graph, colors, col) > 1) {
        // Adiciona candidatos que minimizam comp(c)
        for(i = used_count + 1; i < colors; ++i) {
            // Se a cor não é utilizada
            if(col[used[i]] == 0) {
                col[used[i]] = 1;
                comp_val = comp(size, graph, colors, col);
                if(comp_val < comp_min) {
                    comp_min = comp_val;
                    // Zera a lista de candidatos pois um novo mínimo foi descoberto
                    for(z = 0; z < alpha; ++z) { rcl[z] = 0; }
                    rcl[0] = used[i];
                    alpha = 1;
                } else if(comp_val == comp_min) {
                    rcl[alpha] = used[i];
                    ++alpha;
                } else {
                    col[used[i]] = 0;
                }
            }
        }

        col[rcl[rand() % alpha]] = 1;
    }

    total_time += (double) (clock() - count_zero) / CLOCKS_PER_SEC;
}

void vns(int size, int graph[size][size], int colors, int col[colors]) {
    /* VNS */

    int k;
    int no_improv = 0;
    int col2[colors];

    if(debug) {
    printf("vns:\n");
        for(int i = 0; i < colors; ++i) { printf("%i | ", col[i]); }
        printf("\n");
    }
    // De Consoli et al: according to our experiments, (|C| + |C|/3) is the best choice
    int new_card;
    int card_col = card(colors, col);
    int kmax = card_col * (4.0f / 3.0f);
    while(no_improv < 1) {
        k = 1;
        while(k <= kmax) {
            if(debug) printf("shaking (k: %i / kmax: %i | %i)\n", k, kmax, no_improv);
            count_zero = clock();
            shaking(size, graph, colors, col2, k);
            shaking_time += (double) (clock() - count_zero) / CLOCKS_PER_SEC;
            count_zero = clock();
            local(size, graph, colors, col2);
            local_time += (double) (clock() - count_zero) / CLOCKS_PER_SEC;
            new_card = card(colors, col2);
            if(new_card < card_col) {
                for(int i = 0; i < colors; ++i) { col[i] = col2[i]; }
                if(debug) printf("improvement: %i vs %i\n", new_card, card_col);
                card_col = card(colors, col2);
                kmax = card_col * (4.0f / 3.0f);
                k = 1;
                no_improv = 0;
            } else {
                ++k;
            }

            if(k == kmax) { ++no_improv; }
        }

    }
}

void construct(int size, int graph[size][size], int colors, int col[colors], int iteration) {
    /* Etapa de construção do GRASP */

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
    int i, j;

    while(no_improv < 1) {
        //printf("---\n");
        if(debug) printf("no_improv: %i (%i)\n", no_improv, card(colors, col_star));
        for(i = 0; i < colors; ++i) { col[i] = 0; }
        count_zero = clock();
        construct(size, graph, colors, col, iteration);
        construct_time += (double) (clock() - count_zero) / CLOCKS_PER_SEC;
        vns(size, graph, colors, col);
        if(card(colors, col) < card(colors, col_star)) {
            for(j = 0; j < colors; ++j) { col_star[j] = col[j]; }
            no_improv = 0;
        } else {
            ++no_improv;
        }

        ++iteration;
    }
}

int main(int argc, char **argv) {
    // Tamanho e cores do problema a serem lidos do arquivo de entrada
    int size, colors, c;
    bool plot = 0;
    char* input_file = NULL;

    while((c = getopt(argc, argv, "f:dpt")) != -1) {
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
            case 't':
                track = 1;
                break;
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

    // Clock final para tempo total do algoritmo
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;


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

    if(track) {
        printf("total construct time: %fms\n", construct_time * 1000);
        printf("total shaking time: %fms\n", shaking_time * 1000);
        printf("total local search time: %fms\n", local_time * 1000);
        printf("total fixing time: %fms\n", total_time * 1000);
    }

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
