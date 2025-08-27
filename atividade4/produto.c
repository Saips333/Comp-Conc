/* Disciplina: Programação Concorrente */
/* Profa.: Silvana Rossetto */
/* Lab3: Produto interno com medição de tempo aprimorada */
/* Codigo: Calcula o produto interno de dois vetores de floats */
/* Lucas Tsai Tong Shin 118025502 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 
#include "timer.h"

//variaveis globais
//vetor de elementos
float *vetor_a;
float *vetor_b;

//estrutura de dados para passar argumentos para a thread
typedef struct {
   //tamanho do vetor
   long int n;
   //numero de threads 
   short int nthreads;
   //identificador da thread
   short int id;
} t_args;  


//funcao executada pelas threads
//estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *ProdutoInterno (void *args) {
  t_args *arg = (t_args*) args; //argumentos da thread
  int ini, fim, fatia; //auxiliares para divisao do vetor em blocos
  float produto_local=0, *ret; //produto interno local
  
  fatia = arg->n / arg->nthreads; //tamanho do bloco de dados de cada thread
  ini = arg->id * fatia; //posicao inicial do vetor
  fim = ini + fatia; //posicao final do vetor
  if (arg->id == (arg->nthreads-1)) fim = arg->n; //a ultima thread trata os elementos restantes no caso de divisao nao exata

  //calcula o produto interno
  for(int i=ini; i<fim; i++) {
     produto_local += vetor_a[i] * vetor_b[i];
  }

  //retorna o resultado do produto interno
  ret = (float*) malloc(sizeof(float));
  if (ret!=NULL) *ret = produto_local;
  else printf("--ERRO: malloc() thread\n");
  pthread_exit((void*) ret);
}

//funcao principal do programa
int main(int argc, char *argv[]) {
  long int n; //tamanho do vetor
  short int nthreads; //numero de threads 
  FILE *arq; //arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
  double produto_ori; //produto interno registrado no arquivo
  float produto_par_global; //resultado do produto interno concorrente
  float *produto_retorno_threads; //auxiliar para retorno das threads
  double inicio, fim, tempo_execucao; //variaveis para medicao de tempo

  pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

  //valida e recebe os valores de entrada
  if(argc < 3) { printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); exit(-1); }

  //abre o arquivo de entrada com os valores para serem somados
  arq = fopen(argv[1], "rb");
  if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

  //le o tamanho do vetor (primeira linha do arquivo)
  ret = fread(&n, sizeof(long int), 1, arq);
  if(!ret) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 3;
  }

  //aloca espaco de memoria e carrega o vetor de entrada
  vetor_a = (float*) malloc (sizeof(float) * n);
  vetor_b = (float*) malloc (sizeof(float) * n);
  if(vetor_a==NULL || vetor_b==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }
  ret = fread(vetor_a, sizeof(float), n, arq);
  ret = fread(vetor_b, sizeof(float), n, arq);
  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
     return 4;
  }

  //le o numero de threads da entrada do usuario 
  nthreads = atoi(argv[2]);
  //limita o numero de threads ao tamanho do vetor
  if(nthreads>n) nthreads = n;

  //aloca espaco para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

  //INICIA MEDICAO DE TEMPO
  GET_TIME(inicio);

  //cria as threads
  for(long int i=0; i<nthreads; i++) {
    t_args *args;
    args = (t_args*) malloc(sizeof(t_args));
    if(args==NULL) {    
       printf("--ERRO: malloc argumentos\n"); exit(-1);
    }
    args->n = n;
    args->nthreads = nthreads;
    args->id = i;
    if (pthread_create(&tid_sistema[i], NULL, ProdutoInterno, (void*) args)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //espera todas as threads terminarem e calcula a soma total das threads
  produto_par_global=0;
  for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &produto_retorno_threads)) {
        printf("--ERRO: pthread_join()\n"); exit(-1);
     }
     produto_par_global += *produto_retorno_threads;
     free(produto_retorno_threads);
  }

  //FINALIZA MEDICAO DE TEMPO
  GET_TIME(fim);
  tempo_execucao = fim - inicio;

  //le o produto registrado no arquivo
  ret = fread(&produto_ori, sizeof(double), 1, arq); 

  //calcula variacao relativa
  double variacao = 0.0;
  if (produto_ori != 0.0) {
      variacao = ((double)produto_par_global - produto_ori) / produto_ori;
      if (variacao < 0) variacao = -variacao;
  }

  //imprime os resultados de forma organizada
  printf("Dimensao: %ld\n", n);
  printf("Threads: %d\n", nthreads);
  printf("Produto concorrente: %.10f\n", produto_par_global);
  printf("Produto original: %.10lf\n", produto_ori);
  printf("Variacao relativa: %.2e\n", variacao);
  printf("TEMPO_EXECUCAO: %.6f\n", tempo_execucao);

  //verifica corretude
  if (variacao < 1e-6) {
      printf("STATUS: CORRETO\n");
  } else {
      printf("STATUS: ERRO_NUMERICO\n");
  }

  //desaloca os espacos de memoria
  free(vetor_a);
  free(vetor_b);
  free(tid_sistema);
  //fecha o arquivo
  fclose(arq);

  return 0;
}