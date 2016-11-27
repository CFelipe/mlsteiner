#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

char* get_color(int label) {
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

void plot_initial(int size, int graph[size][size]) {
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

    for(int i = 0; i < size; ++i) {
        fprintf(fp, "%i [label=\"%i\"", i, i);
        if(graph[i][i]) {
            fprintf(fp, ", style=filled, color=gray");
        }
        fprintf(fp, "]\n");

        for(int j = 0; j < size; ++j) {
            if(j > i) {
                if(graph[i][j] != -1) {
                    fprintf(fp, "%i -- %i [color=\"%s\", penwidth=1.5]\n", i, j, get_color(graph[i][j]));
                }
            }
        }
    }

    fprintf(fp, "}");

    fclose(fp);
    system("sfdp steiner1.dot -Tpdf -o graph1.pdf");
}

void plot_solution(int size, int graph[size][size], int colors, int col_star[colors], int span[size]) {
    /* Salva a solução final */
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

    for(int i = 0; i < size; ++i) {
        fprintf(fp, "%i [label=\"%i\"", i, i);
        if(graph[i][i]) {
            fprintf(fp, ", style=filled, color=gray");
        }

        fprintf(fp, "]\n");

        for(int j = 0; j < size; ++j) {
            if(j > i) {
                if(graph[i][j] != -1) {
                    if(col_star[graph[i][j]] &&
                       (span[i] == j || span[j] == i)) {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=1.5]\n", i, j, get_color(graph[i][j]));
                    } else {
                        fprintf(fp, "%i -- %i [color=\"%s\", penwidth=0.0]\n", i, j, get_color(graph[i][j]));
                    }
                }
            }
        }
    }

    fprintf(fp, "}");
    fclose(fp);
    system("sfdp steiner2.dot -Tpdf -o graph2.pdf");
}

int card(int size, int arr[size]) {
    /* Cardinalidade de um conjunto */

    int count = 0;
    for(int i = 0; i < size; ++i) {
        if(arr[i] != 0) {
            ++count;
        }
    }

    return count;
}

int deg(int size, int graph[size][size], int colors, int col_star[colors], int node) {
    int count = 0;
    for(int i = 0; i < size; ++i) {
        if((i < node && graph[i][node] != -1 && col_star[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && col_star[graph[node][i]] == 1)) {
            ++count;
        }
    }

    return count;
}

void dfs(int size, int graph[size][size], int visited[size], int node, int colors, int col[colors]) {
    /* Realiza a busca em profundidade para auxílio no cálculo dos componentes
     * conexos. Apesar de não retornar valores, modifica visited, que por sua
     * vez é lido por comp */

    for(int i = 1; i < size; ++i) {
        if((i < node && graph[i][node] != -1 && col[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && col[graph[node][i]] == 1)) {
            if(visited[i] == 0) {
                //printf("u %i | v %i\n", node, i);
                //printf("%i\n", i);
                visited[i] = 1;
                dfs(size, graph, visited, i, colors, col);
            }
        }
    }
}

void dfs2(int size, int graph[size][size], int colors, int col_star[colors], int visited[], int span[size], int node) {
    if(graph[node][node] == 0 && deg(size, graph, colors, col_star, node) <= 1) {
        span[node] = -1;
    }

    for(int i = 0; i < size; ++i) {
        if((i < node && graph[i][node] != -1 && col_star[graph[i][node]] == 1) ||
           (i > node && graph[node][i] != -1 && col_star[graph[node][i]] == 1)) {
            if(visited[i] == 0) {
                span[i] = node;
                visited[i] = 1;
                dfs2(size, graph, colors, col_star, visited, span, i);
            }
        }
    }
}

int comp(int size, int graph[size][size], int colors, int col[colors]) {
    /* Calcula a quantidade de componentes conexos pra o conjunto de cores em
     * colors. Se um vértice ainda não foi visitado e é básico, é possível
     * fazer uma busca e marcar como visitados todos os vértices conectados a
     * ele */

    int connected = 0;
    int visited[size];
    for(int i = 0; i < size; ++i) {
        visited[i] = 0;
    }

    for(int i = 0; i < size; ++i) {
        if(visited[i] == 0 && graph[i][i] == 1) {
            visited[i] = 1;
            ++connected;
            dfs(size, graph, visited, i, colors, col);
        }
    }

    return connected;
}

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

    printf("Plot? (y/N): ");
    char ans;
    scanf("%c", &ans);

    if(ans == 'y') {
        plot_initial(size, graph);
        plot_solution(size, graph, colors, col_star, span);
    }

    return 1;
}
