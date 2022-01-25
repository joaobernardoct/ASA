/* Projeto 1 - ASA 2017-18
   João Bernardo - 86443
   Pedro Antunes - 86493  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static int visited;
static int numberSSC;
int N, M;
static int i, j; /*para serem usados em ciclos*/

int *locator;

#define INFINITY 99999999
#define MIN(x,y) (((x)<(y)) ? (x) : (y))

int comp(const void *pa, const void *pb);

 typedef struct{
    int x;
    int y;
  } pair;

  pair *pairList;

  typedef struct{
    int value;
    bool onStack;
    int disc;
    int low;
  } vertex;

  vertex *vertexes;

  int *id;

  typedef struct node{
    int value;
    struct node *next;
  }*link;

  static link top;

/*--------------------------------------------------------------------
| Function: stack
| Description: stack para o tarjan
---------------------------------------------------------------------*/
  void init() /* inicializa a pilha */ {top = NULL;}

  void push(int value) /* introduz novo elemento no topo */{
    struct node *new;
    new = (struct node *) malloc(sizeof(struct node));
    new->value = value;
    new->next = top;
    top = new;
  }

  int is_empty() /* pergunta se está vazia */{
    return top == NULL;
  }

  int pop() /* apaga o topo e retorna o valor apagado */{
    int value;
    struct node *old;

    if (!is_empty()) {
      value = top->value;
      old = top;
      top = top->next;
      free(old);
      return value;
    }
    else
      return -1;
  }

/*--------------------------------------------------------------------
| Function: tarjan
| Description: Descobre SSCs
---------------------------------------------------------------------*/
  void tarjan_visit(int value){
    /*set the depth index for v to the smallest unused index*/
    vertex *u = &vertexes[value-1];
    u->disc = u->low = visited;
    visited++;
    push(u->value);
    u->onStack = true;
    int start = locator[u->value-1];
    int end = locator[u->value];

    /*consider successors of v -> w*/
    int h;
    for(h = start; h < end; h++){
      if(vertexes[pairList[h].y-1].disc == INFINITY || vertexes[pairList[h].y-1].onStack){
        /*successor w has not yet been visited; recurse on it*/
        if(vertexes[pairList[h].y-1].disc == INFINITY){
          tarjan_visit(vertexes[pairList[h].y-1].value);
        }
        u->low = MIN(u->low, vertexes[pairList[h].y-1].low);
      }
    }

    /*if v is a root node, pop the stack and generate an SCC*/
    if (u->disc == u->low){
      numberSSC++;
      link t = top;
      int min = u->value;/*int min = u->value;*/
      while(u->value != t->value ){
        if (t->value < min)
          min = t->value;
        t = t->next;
      }
      while(u->value != top->value ){
        int x = pop();
        vertexes[x-1].onStack = false;
        id[x-1] = min;
      }

      id[u->value-1] = min;
      pop();
      u->onStack = false;
    }
  }

 void tarjan(){
  visited = 0;
  init();
  for(i=0;i<N;i++){
    if(vertexes[i].disc==INFINITY){
      tarjan_visit(vertexes[i].value);
    }
  }
 }

/*--------------------------------------------------------------------
| Function: compare
| Description: Compara valores
---------------------------------------------------------------------*/
  int comp(const void *pa, const void *pb){
    const pair *a = (pair *)pa;
    const pair *b = (pair *)pb;
    if (a->x == b->x)
      return (a->y - b->y);
    return (a->x - b->x);
  }

/*--------------------------------------------------------------------
| Function: create_loc
| Description: Cria o array locator (que indica em que índice da
|              pairlist é que um dado vértice começa)
---------------------------------------------------------------------*/
  void create_loc(){
    int h = 0;
    locator[h] = 0;
    h++;
    j = 0;
    while(j < M){
      while(h+1 <= pairList[j].x){
        locator[h] = j;
        h++;
      }
      j++;
    }
    while(h <= N){
      locator[h] = j;
      h++;
    }
  }

/*--------------------------------------------------------------------
| Function: pair_scc
| Description: Altera os valores de cada indice da pairList para o
|              valor minimo do SSC correspondente
---------------------------------------------------------------------*/
void pair_scc(){
  int l;
  for(l=0; l<M; l++){
    int h1 = pairList[l].x;
    pairList[l].x=id[h1-1];
    int h2 = pairList[l].y;
    pairList[l].y=id[h2-1];
  }
}

/*--------------------------------------------------------------------
| Function: print_nr_lig
| Description: Imprime o nr de ligações necessária (sem repetições
|              e ligações entre o mesmo SSC)
---------------------------------------------------------------------*/
void print_nr_lig(){
  int nr_lig_sccs = 0;
  int w=0;
  while(pairList[w].x == pairList[w].y)
    w++;
  if(numberSSC>1){
    nr_lig_sccs++;
  }
  if(numberSSC>1){
    for(i=w+1; i<M; i++){
      if(pairList[i].x != pairList[i].y && (pairList[i].y != pairList[i-1].y || pairList[i].x != pairList[i-1].x)){
        nr_lig_sccs++;
      }
    }
  }
  printf("%d\n", nr_lig_sccs);
}

/*--------------------------------------------------------------------
| Function: print_ligs
| Description: Imprime asligações necessária (sem repetições e ligações
|              entre o mesmo SSC)
---------------------------------------------------------------------*/
void print_ligs(){
  int w=0;
  while(pairList[w].x == pairList[w].y)
    w++;
  if(numberSSC>1){
    printf("%d %d\n", pairList[w].x, pairList[w].y);
  }
  if(numberSSC>1){
    for(i=w+1; i<M; i++){
      if(pairList[i].x != pairList[i].y && (pairList[i].y != pairList[i-1].y || pairList[i].x != pairList[i-1].x)){
        printf("%d %d\n", pairList[i].x, pairList[i].y);
      }
    }
  }
}

/*--------------------------------------------------------------------
| Function: main
| Description: Entrada do programa
---------------------------------------------------------------------*/
int main (int argc, char** argv) {

  /*Ler Input*/
  scanf("%d", &N);
  scanf("%d", &M);

  pairList = (pair *) malloc(sizeof(pair)*M);
  int x,y;

  for(i=1; i<=M; i++){
    scanf("%d", &x);
    scanf("%d", &y);
    pairList[i-1].x = x;
    pairList[i-1].y = y;
  }

  qsort(pairList, M, sizeof(pair), comp); /*Ordenação da pairList*/

  locator = (int *) malloc(sizeof(int)*(N+1));
  create_loc();

  /*Inicialização do vetor de vértices*/
  vertexes = (vertex *) malloc(sizeof(vertex)*N);
  for(j=0; j<N; j++){
    vertexes[j].value=j+1;
    vertexes[j].disc = INFINITY;
    vertexes[j].onStack = false;
  }

  id = (int*) malloc(sizeof(int)*N);
  for(j = 0 ; j < N ; j++)
    id[j]=j+1;

  tarjan(); /*Aplicação do Tarjan*/

  pair_scc(); /*Atribuir valor do SCC aos vértices da pairList*/

  qsort(pairList, M, sizeof(pair), comp); /*Ordenação da nova pairList*/

  printf("%d\n", numberSSC);   /*Imprimir o nr de SCCS*/

  print_nr_lig();

  print_ligs();

  free(pairList);
  free(locator);
  free(vertexes);
  free(id);

  return 0;
}
