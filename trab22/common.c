#include <string.h>

char* get_color(int label) {
    /* Retorna cores para as arestas do grafo, caso queira gerar uma
     * representação com o graphviz */

    label = label % 10;
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
    /* Cria um arquivo graph1.pdf com a configuração inicial do problema usando
     * graphviz */

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
    /* Cria um arquivo graph2.pdf com a configuração final do problema usando
     * graphviz */

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

    // Simplesmente percorre o vetor e aumenta o contador para cada elemento
    // diferente de 0
    int count = 0;
    for(int i = 0; i < size; ++i) {
        if(arr[i] != 0) {
            ++count;
        }
    }

    return count;
}

int deg(int size, int graph[size][size], int colors, int col_star[colors], int node) {
    /* Retorna o grau de um vértice especificado pelo parâmetro node */

    // Percorre a linha e coluna da matriz de adjacência referente ao vértice
    // desejado, aumentando o contador se há um vértice e as suas arestas têm a
    // rotulação inclusa em col_star
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
                visited[i] = 1;
                dfs(size, graph, visited, i, colors, col);
            }
        }
    }
}

void dfs2(int size, int graph[size][size], int colors, int col_star[colors], int visited[], int span[size], int node) {
    /* Realiza a busca em profundidade para cálculo do grafo final, isto é, a
     * árvore de Steiner com rotulação mínima. O resultado é colocado no vetor
     * span, que indica a ordem em que são visitadasdiquuwdqwd */

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
    /* Calcula a quantidade de componentes conexos que têm pelo menos um
     * vértice básico pra o conjunto de cores em colors. Se um vértice ainda
     * não foi visitado e é básico, é possível fazer uma busca e marcar como
     * visitados todos os vértices conectados a ele */

    int connected = 0;
    int visited[size];
    for(int i = 0; i < size; ++i) {
        visited[i] = 0;
    }

    for(int i = 0; i < size; ++i) {
        // Se o vértice não foi visitado e é básico
        if(visited[i] == 0 && graph[i][i] == 1) {
            visited[i] = 1;
            ++connected;
            dfs(size, graph, visited, i, colors, col);
        }
    }

    return connected;
}

void local(int size, int graph[size][size], int colors, int col[colors]) {
    /* A busca local consiste simplesmente em tentar apagar cores do conjunto e
     * verificar se após a remoção ainda tem-se um grafo conexo, isto é,
     * comp(c) = 1 */

    for(int i = 0; i < colors; ++i) {
        if(col[i] == 1) {
            col[i] = 0;
            if(comp(size, graph, colors, col) != 1) {
                // Apagar a cor não gera uma solução, então adicionamo-a de
                // volta
                col[i] = 1;
            } else {
                printf("> ");
            }
        }
    }

    //printf("%i\n", card(colors, col));
    //printf("---\n");
}

void out(char *name, int size, int graph[size][size], int colors, int col[colors]) {
    char *new_str;
    char *extension = "-exact.out";
    if((new_str = malloc(strlen(name)+strlen(extension)+1)) != NULL) {
        new_str[0] = '\0';
        strcat(new_str, name);
        strcat(new_str, extension);
    } else {
        exit(1);
    }

    FILE* fp;
    if((fp = fopen(new_str, "w")) == 0){
        printf("Erro ao abrir arquivo");
        exit(1);
    }

    fprintf(fp, "%i %i\n", size, colors);
    for(int i = 0; i < colors; ++i) {
        fprintf(fp, "%i ", col[i]);
    }
}
