/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
#define LOG 

int main(int argc, char*argv[]) {
   float *vetor_a; //vetor a ser gerado
   float *vetor_b; //vetor b a ser gerado
   long int n; //qtde de elementos do vetor
   float elem1; //valor gerado para incluir no vetor
   float elem2; //valor gerado para incluir no vetor
   double produto = 0; //produto dos elementos dos vetores
   int fator=1; //fator multiplicador para gerar números negativos
   FILE * descritorArquivo; //descritor do arquivo de saida
   size_t ret; //retorno da funcao de escrita no arquivo de saida

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <nome arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para o vetor
   vetor_a = (float*) malloc(sizeof(float) * n);
   vetor_b = (float*) malloc(sizeof(float) * n);
   if(!vetor_a || !vetor_b) {
      fprintf(stderr, "Erro de alocao da memoria do vetor\n");
      return 2;
   }

   //preenche o vetor com valores float aleatorios
   srand(time(NULL));
   for(long int i=0; i<n; i++) {
        elem1 = (rand() % MAX)/3.0 * fator;
        elem2 = (rand() % MAX)/3.0 * fator;
        vetor_a[i] = elem1;
        vetor_b[i] = elem2; 
        produto += elem1 * elem2;
        fator*=-1;
   }

   //imprimir na saida padrao o vetor gerado
   #ifdef LOG
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor_a[i]);
   }
   fprintf(stdout, "\n");
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor_b[i]);
   }
   fprintf(stdout, "\n");
   fprintf(stdout, "%lf\n", produto);
   #endif

   //escreve o vetor no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }
   //escreve a dimensao
   ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   //escreve os elementos do vetor
   ret = fwrite(vetor_a, sizeof(float), n, descritorArquivo);
   if(ret<n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   ret = fwrite(vetor_b, sizeof(float), n, descritorArquivo);
   if(ret<n) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }
   ret = fwrite(&produto, sizeof(double), 1, descritorArquivo);
   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor_a);
   free(vetor_b);
   return 0;
} 
