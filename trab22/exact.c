#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "common.c"

void test(int size, int graph[size][size], int colors, int col[colors], int col_star[colors]) {
    /*
    for(int i = 0; i < colors; ++i) {
        printf("%i | ", col[i]);
    }
    printf("\n");
    */

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
    int size, colors;

    FILE* fp;
    if((fp = fopen(argv[1], "r")) == 0){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    fscanf(fp, "%i %i", &size, &colors);

    int graph[size][size];
    int col[colors];
    int col_star[colors];

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            fscanf(fp, "%i", &graph[i][j]);
        }
    }

    printf("size: %i\n", size);
    printf("colors: %i\n", colors);

    for(int i = 0; i < colors; ++i) {
        col[i] = 0;
        col_star[i] = 1;
    }

    int color;

    srand(time(NULL));

    clock_t begin = clock();

    test(size, graph, colors, col, col_star);

    printf("---\n");
    printf("Solução (%i):\n", card(colors, col_star));
    for(int i = 0; i < colors; ++i) {
        printf("%i | ", col_star[i]);
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
    dfs2(size, graph, colors, col_star, visited, span, 0);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time: %fms\n", time_spent * 1000);

    out(argv[1], size, graph, colors, col_star);

    printf("Plot? (y/N): ");
    char ans;
    scanf("%c", &ans);

    if(ans == 'y') {
        plot_initial(size, graph);
        plot_solution(size, graph, colors, col_star, span);
    }

    return 1;
}
