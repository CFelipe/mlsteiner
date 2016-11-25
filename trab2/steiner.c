#define SIZE 70
#define COLORS 7
#define DENSITY 70
#define BASIC 60

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int graph[SIZE][SIZE] = {0};
int basic[SIZE] = {0};
int col[COLORS] = {0};
int col_star[COLORS] = {0};
int span[SIZE] = {0};
int trim[SIZE] = {0};

char* colors(int label) {
    switch(label) {
        case 0:  return "#61a0d0"; break;
        case 1:  return "#d15033"; break;
        case 2:  return "#56b874"; break;
        case 3:  return "#bf61bb"; break;
        case 4:  return "#b96c53"; break;
        case 5:  return "#726bc9"; break;
        case 6:  return "#cb9540"; break;
        case 7:  return "#ca547c"; break;
        case 8:  return "#607a3a"; break;
        case 9:  return "#99b241"; break;
        default: return "#000000";
    }
}

int card(int* arr) {
    /* Cardinalidade de um conjunto */

    int count = 0;
    for(int i = 0; i < COLORS; ++i) {
        if(arr[i] != 0) {
            ++count;
        }
    }

    return count;
}

int deg(int node) {
    int count = 0;
    for(int i = 0; i < SIZE; ++i) {
        if((i < node && graph[i][node] != -1 && col_star[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && col_star[graph[node][i]] == 1)) {
            ++count;
        }
    }

    return count;
}

void dfs(int* visited, int node, int* colors) {
    /* Realiza a busca em profundidade para auxílio no cálculo dos componentes
     * conexos. Apesar de não retornar valores, modifica visited, que por sua
     * vez é lido por comp */

    /*
    printf("visited: ");
    for(int i = 0; i < SIZE; ++i) {
        printf("%i | ", visited[i]);
    }
    printf("\n");
    */

    for(int i = 1; i < SIZE; ++i) {
        if((i < node && graph[i][node] != -1 && colors[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && colors[graph[node][i]] == 1)) {
            if(visited[i] == 0) {
                //printf("u %i | v %i\n", node, i);
                //printf("%i\n", i);
                visited[i] = 1;
                dfs(visited, i, colors);
            }
        }
    }
}

void dfs2(int visited[], int node) {
    if(basic[node] == 0 && deg(node) <= 1) {
        //printf("trim %i (deg %i) | ", node, deg(node));
        span[node] = -1;
    }

    for(int i = 0; i < SIZE; ++i) {
        if((i < node && graph[i][node] != -1 && col_star[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && col_star[graph[node][i]] == 1)) {
            if(visited[i] == 0) {
                span[i] = node;
                visited[i] = 1;
                dfs2(visited, i);
            }
        }
    }
}

int comp(int* colors) {
    /* Calcula a quantidade de componentes conexos pra o conjunto de cores em
     * colors. Se um vértice ainda não foi visitado e é básico, é possível
     * fazer uma busca e marcar como visitados todos os vértices conectados a
     * ele */

    int connected = 0;
    int visited[SIZE] = {0};
    for(int i = 0; i < SIZE; ++i) {
        if(visited[i] == 0 && basic[i] == 1) {
            visited[i] = 1;
            ++connected;
            dfs(visited, i, colors);
        }
    }

    return connected;
}

void test(int* col) {
    int count = 0;
    do {
        ++count;
        for(int i = 0; i < COLORS; ++i) {
            col[i] = 0;
        }

        //construction-phase
        int rcl[10];
        if(count > 2) {
            for(int i = 0; i < COLORS; ++i) {
                rcl[i] = 1;
            }
            // select random color c
            col[c] = 1;
            // update h and comp
        }
        while(comp(col) > 1) {
            // rcl = minimizar comp
            // selecionar cor c em rcl
            col[c] = 1;
            // update h and comp
        }

        //local-search

        if(card(col) < card(col_star)) {
            for(int i = 0; i < COLORS; ++i) {
                col_star[i] = col;
            }
        }

        //take spanning tree
    } while(true);
}

int main() {
    int color;

    srand(time(NULL));

    // Inicializa grafo
    for(int i = 0; i < SIZE; ++i) {
        if(i < BASIC) {
            basic[i] = 1;
        }
        for(int j = 0; j < SIZE; ++j) {
            if(j > i) {
                // Gera aresta ou não com probabilidade DENSITY / 100
                if(rand() % 100 < DENSITY) {
                    // Escolhe uma cor aleatória pra aresta
                    color = rand() % (COLORS);
                    graph[i][j] = color;
                    col_star[color] = 1;
                } else {
                    // A parte redundante da representação matricial é ignorada
                    graph[i][j] = -1;
                }
            }
        }
    }

    int visited[SIZE] = {0};
    grasp(col);
    printf("\nCores finais:\n");
    for(int i = 0; i < COLORS; ++i) {
        printf("%i | ", col_star[i]);
    }
    printf("\n");

    for(int i = 0; i < SIZE; ++i) {
        span[i] = -1;
        visited[i] = 0;
    }

    visited[0] = 1;
    span[0] = 0;
    dfs2(visited, 0);

    return 1;
}
