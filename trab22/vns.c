#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "common.c"

void shaking(int size, int graph[size][size], int colors, int col[colors], int k) {
    /* Etapa de shaking do VNS */

    int used[colors];
    int used_count = 0;
    int unused_count = 0;
    int card_col = card(colors, col);

    // Coloca índices das cores usadas no começo da matriz used e não usadas no
    // final. Para diferenciar os usados de não usados, no final do laço,
    // used_count contém o valor do índice da última cor usada
    for(int i = 0; i < colors; ++i) {
        if(col[i] == 1) {
            used[used_count] = i;
            ++used_count;
        } else {
            used[colors - unused_count - 1] = i;
            ++unused_count;
        }
    }

    // Remove e adiciona cores
    for(int i = 1; i < k; ++i) {
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
    for(int i = 0; i < colors; ++i) { rcl[i] = 0; }

    while(comp(size, graph, colors, col) > 1) {
        // Adiciona candidatos que minimizam comp(c)
        int comp_val;
        int comp_min = size;
        for(int i = 0; i < colors; ++i) {
            for(int j = 0; j < colors; ++j) { col2[j] = col[j]; }
            // Tenta adicionar a cor para verificar se minimiza comp
            col2[i] = 1;
            comp_val = comp(size, graph, colors, col2);
            if(comp_val < comp_min) {
                comp_min = comp_val;
                // Zera a lista de candidatos pois um novo mínimo foi descoberto
                for(int j = 0; j < colors; ++j) { rcl[j] = 0; }
                rcl[0] = i;
                alpha = 1;
            } else if(comp_val == comp_min) {
                rcl[alpha] = i;
                ++alpha;
            }
        }

        // Adiciona melhores candidatos até obter grafo conexo
        col[rcl[rand() % alpha]] = 1;
    }

    printf("shaking final comp: %i\n", comp(size, graph, colors, col));

    //for(int i = 0; i < colors; ++i) { printf("%i | ", col[i]); }
    //printf("\n");
}

void vns(int size, int graph[size][size], int colors, int col[colors]) {
    /* VNS */

    int k;
    int no_improv = 0;
    int col2[colors];

    for(int i = 0; i < colors; ++i) {
        col[i] = rand() % 2;
        col2[i] = col[i];
    }

    // De Consoli et al: according to our experiments, (|C| + |C|/3) is the best choice
    int new_card;
    int card_col = card(colors, col);
    int kmax = card_col * (4.0f / 3.0f);
    while(no_improv < 3) {
        k = 1;
        while(k <= kmax) {
            printf("shaking (k: %i / kmax: %i | %i)\n", k, kmax, no_improv);
            shaking(size, graph, colors, col2, k);
            local(size, graph, colors, col);
            new_card = card(colors, col2);
            if(new_card < card_col) {
                for(int i = 0; i < colors; ++i) { col[i] = col2[i]; }
                card_col = card(colors, col2);
                kmax = card_col * (4.0f / 3.0f);
                k = 1;
                no_improv = 0;
                printf("VNS improvement!\n");
            } else {
                ++k;
            }

            if(k == kmax) { ++no_improv; }
        }

    }
}

int main(int argc, char **argv) {
    // Tamanho e cores do problema a serem lidos do arquivo de entrada
    int size, colors;

    FILE* fp;
    if((fp = fopen(argv[1], "r")) == 0){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    // Lê do arquivo o tamanho do grafo e a quantidade de cores
    fscanf(fp, "%i %i", &size, &colors);

    // Separa espaço na memória para o grafo e soluções temporárias
    int graph[size][size];
    int col[colors];

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

    printf("size: %i\n", size);
    printf("colors: %i\n", colors);
    printf("edges: %i\n", count_edges);

    // Seed com o tempo
    srand(time(NULL));

    for(int i = 0; i < colors; ++i) { col[i] = 0; }
    printf("\n");

    // Clock inicial para mensurar diversos tempos de execução
    clock_t begin = clock();

    vns(size, graph, colors, col);

    printf("---\n");
    printf("Solução (%i):\n", card(colors, col));
    for(int i = 0; i < colors; ++i) {
        printf("%i | ", col[i]);
    }
    printf("\n");

    int span[size];
    int visited[size];
    for(int i = 0; i < size; ++i) {
        span[i] = -1;
        visited[i] = 0;
    }

    visited[0] = 1;
    span[0] = 0;
    dfs2(size, graph, colors, col, visited, span, 0);

    // Clock final para tempo total do algoritmo
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time: %fms\n", time_spent * 1000);

    //printf("Plot? (y/N): ");
    //char ans;
    //scanf("%c", &ans);

    //if(ans == 'y') {
    //    plot_initial(size, graph);
    //    plot_solution(size, graph, colors, col, span);
    //}

    return 1;
}
