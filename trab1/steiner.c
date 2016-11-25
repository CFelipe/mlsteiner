#define SIZE 15
#define COLORS 3
#define BASIC 7
#define DENSITY 70

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

void plot_initial() {
    /* Salva um arquivo .dot com a configuração inicial do problema */

    FILE* fp;
    if((fp = fopen("steiner1.dot", "w")) == 0){
        printf("Erro ao abrir arquivo");
        exit(1);
    }

    fprintf(fp, "graph {\n");
    fprintf(fp, "node [shape=circle, height=.15, label=\"\", margin=0.02];\n");
    fprintf(fp, "splines=true;\n \
                 sep=\"+25,25\";\n \
                 overlap=scalexy;\n \
                 nodesep=0.6;\n \
                 node [fontsize=11];\n");

    for(int i = 0; i < SIZE; ++i) {
        //printf("| ");
        fprintf(fp, "%i [label=\"%i\"", i, i);
        if(basic[i]) {
            fprintf(fp, ", style=filled, color=gray");
        }
        fprintf(fp, "]\n");

        for(int j = 0; j < SIZE; ++j) {
            if(j > i) {
                if(graph[i][j] != -1) {
                    //fprintf(fp, "%i -- %i [label=\"%i\"]\n", i, j, graph[i][j]);
                    fprintf(fp, "%i -- %i [color=\"%s\", penwidth=1.5]\n", i, j, colors(graph[i][j]));
                }
                //printf("%2i | ", graph[i][j]);
            } else {
                //printf(" X | ");
            }
        }
        //printf("\n");
    }

    fprintf(fp, "}");
}

void plot_h_star() {
    /* Salva um arquivo .dot com a configuração intermediária H* de um
     * problema, isto é, um grafo apenas com arestas de rótulo em C*
     * */

    FILE* fp;
    if((fp = fopen("steiner2.dot", "w")) == 0){
        printf("Erro ao abrir arquivo");
        exit(1);
    }

    fprintf(fp, "graph {\n");
    fprintf(fp, "node [shape=circle, height=.15, label=\"\", margin=0.02];\n");
    fprintf(fp, "splines=true;\n \
                 sep=\"+25,25\";\n \
                 overlap=scalexy;\n \
                 nodesep=0.6;\n \
                 node [fontsize=11];\n");

    for(int i = 0; i < SIZE; ++i) {
        fprintf(fp, "%i [label=\"%i\"", i, i);
        if(basic[i]) {
            fprintf(fp, ", style=filled, color=gray");
        }
        fprintf(fp, "]\n");

        for(int j = 0; j < SIZE; ++j) {
            if(j > i) {
                if(graph[i][j] != -1) {
                    if(col_star[graph[i][j]]) {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=1.5]\n", i, j, colors(graph[i][j]));
                    } else {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=0.0]\n", i, j, colors(graph[i][j]));
                    }
                }
            }
        }
    }

    fprintf(fp, "}");
}

void plot_solution() {
    /* Salva a solução final */
    FILE* fp;
    if((fp = fopen("steiner3.dot", "w")) == 0){
        printf("Erro ao abrir arquivo");
        exit(1);
    }

    fprintf(fp, "graph {\n");
    fprintf(fp, "node [shape=circle, height=.15, label=\"\", margin=0.02];\n");
    fprintf(fp, "splines=true;\n \
                 sep=\"+25,25\";\n \
                 overlap=scalexy;\n \
                 nodesep=0.6;\n \
                 node [fontsize=11];\n");

    for(int i = 0; i < SIZE; ++i) {
        fprintf(fp, "%i [label=\"%i\"", i, i);
        if(basic[i]) {
            fprintf(fp, ", style=filled, color=gray");
        }

        fprintf(fp, "]\n");

        for(int j = 0; j < SIZE; ++j) {
            if(j > i) {
                if(graph[i][j] != -1) {
                    if(col_star[graph[i][j]] &&
                       (span[i] == j || span[j] == i)) {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=1.5]\n", i, j, colors(graph[i][j]));
                    } else {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=0.0]\n", i, j, colors(graph[i][j]));
                    }
                }
            }
        }
    }

    fprintf(fp, "}");
}

int card(int* arr) {
    /* Cardinalidade de um conjunto */

    int count = 0;
    for(int i = 0; i < COLORS; ++i) {
        //printf("%i | ", arr[i]);
        if(arr[i] != 0) {
            ++count;
        }
    }
    //printf("\n");

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
    /* Procedimento que executa o branch and bound como descrito no relatório
     * */

    for(int i = 0; i < COLORS; ++i) {
        printf("%i | ", col[i]);
    }
    printf("\n");

    if(card(col) < card(col_star)) {
        int comp_val = comp(col);
        if(comp_val == 1) {
            for(int i = 0; i < COLORS; ++i) {
                col_star[i] = col[i];
            }
        } else if(card(col) < card(col_star) - 1) {
            for(int i = 0; i < COLORS; ++i) {
                if(col[i] == 0) {
                    col[i] = 1;
                    test(col);
                    col[i] = 0;
                }
            }
        }
    }

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
    test(col);
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

    plot_initial();
    plot_h_star();
    plot_solution();

    return 1;
}
