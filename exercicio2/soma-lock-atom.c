/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond_multiplo_1000; //variavel de condicao para sincronizar multiplos de 1000
int multiplo_1000_impresso = 1; //flag para indicar se o multiplo de 1000 foi impresso
int threads_terminadas = 0; //contador de threads que terminaram
int num_threads; //numero total de threads trabalhadoras

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--SC (seção critica)
     //se soma é múltiplo de 1000, espera até que seja impresso
     if (soma % 1000 == 0 && soma != 0) {
        multiplo_1000_impresso = 0; //marca que há um múltiplo de 1000 para imprimir
        pthread_cond_signal(&cond_multiplo_1000); //sinaliza para a thread extra
        while (!multiplo_1000_impresso) {
           pthread_cond_wait(&cond_multiplo_1000, &mutex); //espera até ser impresso
        }
     }
     soma++; //incrementa a variavel compartilhada 
     //--saida da SC
     pthread_mutex_unlock(&mutex);
  }
  
  //thread terminou, incrementa contador e sinaliza
  pthread_mutex_lock(&mutex);
  threads_terminadas++;
  pthread_cond_signal(&cond_multiplo_1000); //sinaliza para acordar a thread extra
  pthread_mutex_unlock(&mutex);
  
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");
  
  pthread_mutex_lock(&mutex);
  while (threads_terminadas < num_threads) {
     //espera até haver um múltiplo de 1000 para imprimir ou todas as threads terminarem
     while (multiplo_1000_impresso && threads_terminadas < num_threads) {
        pthread_cond_wait(&cond_multiplo_1000, &mutex);
     }
     
     //se há um múltiplo de 1000 para imprimir
     if (!multiplo_1000_impresso) {
        //imprime o múltiplo de 1000
        printf("soma = %ld \n", soma);
        
        //marca que o múltiplo foi impresso e sinaliza para as threads continuarem
        multiplo_1000_impresso = 1;
        pthread_cond_broadcast(&cond_multiplo_1000);
     }
  }
  printf("soma = %ld \n", soma);
  pthread_mutex_unlock(&mutex);
  
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);
   num_threads = nthreads; //armazena o numero de threads trabalhadoras

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua) e variavel de condicao
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond_multiplo_1000, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex e variavel de condicao
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond_multiplo_1000);
   

   return 0;
}
