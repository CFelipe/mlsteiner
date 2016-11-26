#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

void construct(int size, int graph[size][size], int colors, int col[colors], int iteration) {
    /* Etapa de construção do GRASP */

    printf("it: %i\n", iteration);

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

void local(int size, int graph[size][size], int colors, int col[colors]) {
    /* A busca local consiste simplesmente em tentar apagar cores do conjunto e
     * verificar se após a remoção ainda tem-se um grafo conexo, isto é,
     * comp(c) = 1 */

    printf("%i / ", card(colors, col));
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
    printf("%i\n", card(colors, col));
    //printf("---\n");
}

void shaking(int size, int graph[size][size], int colors, int col[colors], int k) {
    /* Etapa de shaking do VNS */

    int c;
    int used[colors];
    int used_count = 0;
    int unused_count = 0;
    for(int i = 0; i < colors; ++i) {
        if(col[i] == 1) {
            used[used_count] = i;
            ++used_count;
        } else {
            used[colors - unused_count - 1] = i;
            ++unused_count;
        }
    }

    for(int i = 1; i < k; ++i) {
        if(i <= card(colors, col)) {
            // Remove cor utilizada
            col[rand() % used_count] = 0;
        } else {
            // Adiciona cor não utilizada
            col[used_count + (rand() % unused_count)] = 1;
        }
    }

    int col2[colors];
    int rcl[colors];
    int alpha = 0;
    for(int i = 0; i < colors; ++i) { rcl[i] = 0; }

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

void vns(int size, int graph[size][size], int colors, int col[colors]) {
    /* VNS */

    int k;
    int col2[colors];
    int no_improv = 0;
    for(int i = 0; i < colors; ++i) { col2[i] = col[i]; }
    // De Consoli et al: according to our experiments, (|C| + |C|/3) is the best choice
    int card_col = card(colors, col);
    int kmax = card_col * (4 / 3);
    while(no_improv < 5) {
        k = 1;
        while(k <= kmax) {
            //printf("shaking (k: %i / kmax: %i | %i)\n", k, kmax, no_improv);
            shaking(size, graph, colors, col2, k);
            local(size, graph, colors, col2);
            if(comp(size, graph, colors, col2) == 1 && card(colors, col2) < card_col) {
                for(int i = 0; i < colors; ++i) { col[i] = col2[i]; }
                card_col = card(colors, col);
                printf("VNS improvement!\n");
                no_improv = 0;
                break;
            } else {
                ++k;
            }

            if(k == kmax) { ++no_improv; }
        }

    }
}

void grasp(int size, int graph[size][size], int colors, int col[colors], int col_star[colors]) {
    /* GRASP */

    int iteration = 1;
    int no_improv = 0;

    while(no_improv < 15) {
        printf("---\n");
        printf("no_improv: %i (%i)\n", no_improv, card(colors, col_star));
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
        printf("it sol card: %i\n", card(colors, col));

        ++iteration;
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
    int col_star[colors];

    // Lê grafo do arquivo
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

    // Seed com o tempo
    srand(time(NULL));

    // Clock inicial para mensurar diversos tempos de execução
    clock_t begin = clock();

    grasp(size, graph, colors, col, col_star);

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

    // Clock final para tempo total do algoritmo
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
