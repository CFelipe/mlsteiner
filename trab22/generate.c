#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if(argc < 6) {
        printf("Passar argumentos no formato "
                "'arquivo tamanho cores basicos densidade'\n");
        exit(1);
    }

    FILE* fp;
    if((fp = fopen(argv[1], "w")) == 0){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    srand(mix(clock(), time(NULL), getpid()));

    int size = atoi(argv[2]);
    int colors = atoi(argv[3]);
    int basic = atoi(argv[4]);
    int density = atoi(argv[5]);
    int color;

    fprintf(fp, "%i %i\n", size, colors);

    int graph[size][size];

    // Inicializa grafo
    for(int i = 0; i < size; ++i) {

        for(int j = 0; j < size; ++j) {
            if(j > i) {
                // Gera aresta ou não com probabilidade DENSITY / 100
                if(rand() % 100 < density) {
                    // Escolhe uma cor aleatória pra aresta
                    color = rand() % (colors);
                    graph[i][j] = color;
                } else {
                    // A parte redundante da representação matricial é ignorada
                    graph[i][j] = -1;
                }
            } else if(j == i) {
                graph[i][i] = i < basic;
            } else {
                graph[i][j] = 0;
            }
            fprintf(fp, "%4i ", graph[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}
