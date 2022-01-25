/* Projeto 2 - ASA 2017-18
   João Bernardo - 86443
   Pedro Antunes - 86493  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int m, n;
static int i, j; /*para serem usados em ciclos*/

#define INFINITY 999999999
#define MIN(x,y) (((x)<(y)) ? (x) : (y))

typedef struct{
  int cp;
  int lp;
} vertex;

vertex *vertexes;

typedef struct edge{
  int orig;
  int dest;
  int flow;
  int capacity;
} edge;

edge *edges;

typedef struct{
  int color;
  int d;
  int pi;
} bfsVertex;

bfsVertex *bfsVertexes;

typedef struct QUEUEnode* link;

struct QUEUEnode{
  int value;
  link next;
};

static link head, tail;

int *locator;

int *pred;
int *pred_index;

int finalFlow = 0;


/*--------------------------------------------------------------------
| Function: stack FIFO
| Description: stack para a BFS
---------------------------------------------------------------------*/

  void init() /* inicializa a pilha */ {
    head = NULL;
    tail = NULL;
  }

  link newNode(int value, link next){
    link x = (link) malloc(sizeof(struct QUEUEnode));
    x->value = value;
    x->next = next;
    return x;
  }

  void push(int value) /* introduz novo elemento no topo */{
    if (head == NULL){
      head = (tail = newNode(value, head));
      return;
    }
    tail->next = newNode(value, tail->next);
    tail = tail->next;
  }

  int is_empty() /* pergunta se está vazia */{
    return head == NULL;
  }

  int pop() /* apaga o topo e retorna o valor apagado */{
    int value;
    value = head->value;
    link t = head->next;
    free(head);
    head = t;
    return value;
  }


/*--------------------------------------------------------------------
| Function: inputReader
| Description: lê o input
---------------------------------------------------------------------*/
void inputReader(){
  int value;
  int edge_index = 0;
  int reverse_edge_index = 0;

  /*--- Ler os Pesos lp ---*/
  for(i=0; i<m; i++){
    for(j=0; j<n; j++){
    scanf("%d", &value);
    vertexes[n*i+j+1].lp = value;
    /*edge: Source-->Vertex*/
    edges[edge_index].orig = 0;
    edges[edge_index].dest = n*i+j+1;
    edges[edge_index].capacity = value;

    edge_index++;
    }
  }

  /*--- Ler os Pesos cp ---*/
  for(i=0; i<m; i++){
    for(j=0; j<n; j++){
    scanf("%d", &value);
    vertexes[n*i+j+1].cp = value;
    /*edge: Vertex-->Target*/
    edges[edge_index].orig = n*i+j+1;
    edges[edge_index].dest = m*n+1;
    edges[edge_index].capacity = value;
    /*Atribuir min(cp,lp) ao fluxo da edge e da sua reverse*/
    edges[reverse_edge_index].flow = MIN(edges[reverse_edge_index].capacity, value);
    edges[edge_index].flow = MIN(edges[reverse_edge_index].capacity, value);
    finalFlow = finalFlow + edges[edge_index].flow;

    reverse_edge_index++;
    edge_index++;
    }
  }

  /*--- Ler os Pesos Associados às Vizinhanças Horizontais ---*/
  int orig = 1;
  int counter = n-1;
  for(i=0; i<m*(n-1); i++){
    counter--;
    scanf("%d", &value);
    /*Edge*/
    edges[edge_index].orig = orig;
    edges[edge_index].dest = orig+1;
    edges[edge_index].flow = 0;
    edges[edge_index].capacity = value;
    /*Reverse edge*/
    edges[edge_index+1].orig = orig+1;
    edges[edge_index+1].dest = orig++;
    edges[edge_index+1].flow = 0;
    edges[edge_index+1].capacity = value;
    if(counter == 0){
      orig++;
      counter = n-1;
    }
    edge_index = edge_index+2;
  }

  /*--- Ler os Pesos Associados às Vizinhanças Verticais ---*/
  orig = 1;
  counter = n-1;
  for(i=0; i<n*(m-1); i++){
    scanf("%d", &value);
    /*Edge*/
    edges[edge_index].orig = orig;
    edges[edge_index].dest = orig+n;
    edges[edge_index].flow = 0;
    edges[edge_index].capacity = value;
    /*Reverse edge*/
    edges[edge_index+1].orig = orig+n;
    edges[edge_index+1].dest = orig++;
    edges[edge_index+1].flow = 0;
    edges[edge_index+1].capacity = value;

    if(counter == 0){
      orig++;
      counter = n-1;
    }
    edge_index = edge_index+2;
  }
}


/*--------------------------------------------------------------------
| Function: compare
| Description: Compara valores de inteiros
---------------------------------------------------------------------*/
  int comp(const void *pa, const void *pb){
    const edge*a = (edge*)pa;
    const edge*b = (edge*)pb;
    if (a->orig == b->orig)
      return (a->dest - b->dest);
    return (a->orig - b->orig);
  }


/*--------------------------------------------------------------------
| Function: create_loc
| Description: Cria o array locator (que indica em que índice de
|              edges é que as adjacências de um dado vértice começam)
---------------------------------------------------------------------*/
  void create_loc(int nr_edges, int nr_vertex){
    int h = 0;
    locator[h] = 0;
    h++;
    for(i=1; i<2*(nr_edges+m*n); i++){
      if(h == edges[i].orig){
        locator[h] = i;
        h++;
      }
    }
    locator[m*n+1] = 2*(nr_edges+nr_vertex);
  }


/*--------------------------------------------------------------------
| Function: E-K and BFS
| Description: Algorithm to find the maximum flow
---------------------------------------------------------------------*/
void bfs(){
  for(i=0; i< m*n+2; i++){
    bfsVertexes[i].color = 0; /*univisited: 0   visited: 1   finished visiting: 2*/
    bfsVertexes[i].d = INFINITY;
    bfsVertexes[i].pi = -1;
    pred[i] = -1; /*-1 is NULL*/
    pred_index[i]=-1; /*-1 is NULL*/
  }

  bfsVertexes[0].color = 1;
  bfsVertexes[0].d = 0;
  push(0);
  int currentNode, v, to;

  while (!is_empty()){
    currentNode = pop();
    if(currentNode != m*n+1){    /*o vértice não pode ser o target*/
      for(v = locator[currentNode]; v < locator[currentNode+1]; v++){    /*ver todas as adjacências do vértice*/
        to = edges[v].dest;    /*vértice adjacente ao currentNode*/

        if(bfsVertexes[to].color == 0 && (edges[v].capacity - edges[v].flow > 0)){
          bfsVertexes[to].color = 1;
          bfsVertexes[to].d = bfsVertexes[currentNode].d + 1;
          bfsVertexes[to].pi = currentNode;
          push(to);
        }

      }
    }
    bfsVertexes[currentNode].color = 2;
  }

  /*Store the edge taken to get to a certain vertex*/
  int target = m*n+1;
  int target_dad = bfsVertexes[target].pi;
  while(target_dad != -1){ /*percorrer tabela de pi p chegar de source a target
                            e meter todas as arestas necessarias em pred_index*/
    for(i = locator[target_dad]; i < locator[target_dad+1]; i++){
      if (edges[i].dest == target){
        pred_index[target_dad] = i;
        pred[edges[i].dest] = target_dad;
        break;
      }
    }
    target = target_dad;
    target_dad = bfsVertexes[target].pi;
  }
}


int edmondsKarp(){
    while(true){
      bfs();
      if(pred[m*n+1] == -1)
        break;
      int e;
      int df = INFINITY;
      /*We found an augmenting path. See how much flow we can send...*/
      e = pred[m*n+1];
      while(e != -1){
        df = MIN(df, edges[pred_index[e]].capacity - edges[pred_index[e]].flow);
        e = pred[e];
      }
      /*... and update edges by that amount (df)*/
      e = pred[m*n+1];
      while(e != -1){
        edges[pred_index[e]].flow = edges[pred_index[e]].flow + df;
        if(edges[pred_index[e]].orig != 0 && edges[pred_index[e]].dest != m*n+1){
          for(i = locator[e]; i < locator[e+1]; i++){
            if (edges[i].dest == edges[pred_index[e]].orig){
              edges[i].flow = edges[i].flow -df;
            }
          }
        }
        e = pred[e];
      }
      finalFlow = finalFlow + df;
  }
  return 0;
}


/*--------------------------------------------------------------------
| Function: minimumCutPrint
| Description: encontra o corte mínimo e imprime-o
---------------------------------------------------------------------*/

void minimumCutPrint(){
  init();
  bfs();
  int tab=0;
  for(i=1; i< m*n+1; i++){
    if(bfsVertexes[i].d == INFINITY)
      printf("P ");
    else
      printf("C ");
    tab++;
    if (tab == n){
      puts("");
      tab = 0;
    }
  }
}


/*--------------------------------------------------------------------
| Function: main
| Description: Entrada do programa
---------------------------------------------------------------------*/
int main (int argc, char** argv) {

  /*--- Leitura e Processamento do Input ---*/
  scanf("%d", &m);
  scanf("%d", &n);

  int nr_vertex = m*n;
  int nr_edges  = ((m-1)*n + (n-1)*m);

  vertexes = (vertex *) malloc(sizeof(vertex)* (nr_vertex+2) );
  edges    = (edge*) malloc(sizeof(edge)* (2*(nr_edges+nr_vertex)) );

  inputReader();

  qsort(edges, 2*(nr_edges+m*n), sizeof(edge), comp); /*Ordenação das edges*/

  locator = (int *) malloc(sizeof(int)*(nr_vertex+2));  /*Índice da 1ª edge adjacente a cada vértice*/
  create_loc(nr_edges, nr_vertex);

  /*--- Execução do algoritmo ---*/
  pred        = (int *) malloc(sizeof (int)* (m*n+2));
  pred_index  = (int *) malloc(sizeof (int)* (m*n+2));
  bfsVertexes = (bfsVertex *) malloc(sizeof(bfsVertex)*(m*n+2));

  init();
  edmondsKarp();

  /*--- Output: ---*/
  printf("%d\n\n",finalFlow);
  minimumCutPrint();

  /*--- Frees: ---*/
  free(bfsVertexes);
  free(pred_index);
  free(pred);
  free(locator);
  free(edges);
  free(vertexes);

  return 0;
}
