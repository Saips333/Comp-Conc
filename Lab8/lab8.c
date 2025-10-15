#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

// Estrutura para o buffer compartilhado
typedef struct {
    long long int *buffer;
    int tamanho;
    int N; // Total de números a processar
    int numeros_inseridos; // Contador de números inseridos
} Buffer;

// Estrutura para dados das threads consumidoras
typedef struct {
    int id;
    int primos_encontrados;
} ThreadConsumidora;

// Variáveis globais
Buffer buffer_compartilhado;
sem_t mutex;           // Exclusão mútua para acessar o buffer
sem_t vazio;           // Semáforo para indicar buffer vazio
sem_t cheio;           // Semáforo para indicar buffer cheio
int indice_consumo = 0; // Índice para consumo

// Função para verificar se um número é primo
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Thread produtora
void* produtor(void* arg) {
    int i;
    long long int numero = 1;

    while (numero <= buffer_compartilhado.N) {
        // Espera o buffer ficar vazio
        sem_wait(&vazio);

        // Preenche o buffer completamente
        for (i = 0; i < buffer_compartilhado.tamanho && numero <= buffer_compartilhado.N; i++) {
            buffer_compartilhado.buffer[i] = numero;
            numero++;
        }
        buffer_compartilhado.numeros_inseridos = i;
        indice_consumo = 0; // Reset índice de consumo

        // Sinaliza que o buffer está cheio
        sem_post(&cheio);
    }

    pthread_exit(NULL);
}

// Thread consumidora
void* consumidor(void* arg) {
    ThreadConsumidora* dados = (ThreadConsumidora*)arg;
    long long int numero;
    int continuar = 1;

    while (continuar) {
        // Espera o buffer estar cheio
        sem_wait(&cheio);

        // Consome um número
        sem_wait(&mutex);
        if (indice_consumo < buffer_compartilhado.numeros_inseridos) {
            numero = buffer_compartilhado.buffer[indice_consumo];
            indice_consumo++;

            // Verifica se é o último número do buffer
            int ultimo_do_buffer = (indice_consumo == buffer_compartilhado.numeros_inseridos);
            // Verifica se é o último número total
            int ultimo_total = (numero >= buffer_compartilhado.N);

            if (ultimo_do_buffer) {
                // Sinaliza que buffer está vazio
                sem_post(&vazio);
                // Se não é o último número total, outros consumidores precisarão acordar depois
            } else {
                // Ainda há números no buffer, permite outros consumidores
                sem_post(&cheio);
            }
            sem_post(&mutex);

            // Verifica se é primo (fora da seção crítica)
            if (ehPrimo(numero)) {
                dados->primos_encontrados++;
            }

            // Se processou o último número, libera outros consumidores para terminarem
            if (ultimo_total) {
                continuar = 0;
                // Acorda outros consumidores que possam estar esperando
                sem_post(&cheio);
            }
        } else {
            // Buffer já foi esvaziado, libera próximo e termina
            sem_post(&cheio);
            sem_post(&mutex);
            continuar = 0;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Uso: %s <N> <M> <num_consumidores>\n", argv[0]);
        printf("  N: quantidade total de números\n");
        printf("  M: tamanho do buffer\n");
        printf("  num_consumidores: número de threads consumidoras\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int num_consumidores = atoi(argv[3]);

    if (N <= 0 || M <= 0 || num_consumidores <= 0) {
        printf("Erro: N, M e num_consumidores devem ser positivos\n");
        return 1;
    }

    if (M >= N) {
        printf("Erro: M deve ser muito menor que N (M << N)\n");
        return 1;
    }

    // Inicializa o buffer
    buffer_compartilhado.buffer = (long long int*)malloc(M * sizeof(long long int));
    buffer_compartilhado.tamanho = M;
    buffer_compartilhado.N = N;
    buffer_compartilhado.numeros_inseridos = 0;

    // Inicializa os semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&vazio, 0, 1);  // Começa vazio (produtor pode inserir)
    sem_init(&cheio, 0, 0);  // Começa não-cheio (consumidores esperam)

    // Cria a thread produtora
    pthread_t thread_prod;
    pthread_create(&thread_prod, NULL, produtor, NULL);

    // Cria as threads consumidoras
    pthread_t* threads_cons = (pthread_t*)malloc(num_consumidores * sizeof(pthread_t));
    ThreadConsumidora* dados_cons = (ThreadConsumidora*)malloc(num_consumidores * sizeof(ThreadConsumidora));

    int i;
    for (i = 0; i < num_consumidores; i++) {
        dados_cons[i].id = i + 1;
        dados_cons[i].primos_encontrados = 0;
        pthread_create(&threads_cons[i], NULL, consumidor, &dados_cons[i]);
    }

    // Espera a thread produtora terminar
    pthread_join(thread_prod, NULL);

    // Espera as threads consumidoras terminarem
    for (i = 0; i < num_consumidores; i++) {
        pthread_join(threads_cons[i], NULL);
    }

    // Calcula o total de primos
    int primos_totais = 0;
    for (i = 0; i < num_consumidores; i++) {
        primos_totais += dados_cons[i].primos_encontrados;
    }

    // Encontra o vencedor
    int vencedor_id = 1;
    int max_primos = dados_cons[0].primos_encontrados;
    for (i = 1; i < num_consumidores; i++) {
        if (dados_cons[i].primos_encontrados > max_primos) {
            max_primos = dados_cons[i].primos_encontrados;
            vencedor_id = dados_cons[i].id;
        }
    }

    // Salva resultados em arquivo
    FILE *arquivo = fopen("resultados.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo de resultados\n");
        return 1;
    }

    fprintf(arquivo, "=== RESULTADOS ===\n");
    fprintf(arquivo, "N=%d, M=%d, Consumidores=%d\n", N, M, num_consumidores);
    fprintf(arquivo, "Total de números primos encontrados: %d\n", primos_totais);
    fprintf(arquivo, "\nDesempenho das threads consumidoras:\n");
    for (i = 0; i < num_consumidores; i++) {
        fprintf(arquivo, "  Thread %d: %d primos\n", dados_cons[i].id, dados_cons[i].primos_encontrados);
    }
    fprintf(arquivo, "\nThread VENCEDORA: Thread %d (%d primos)\n", vencedor_id, max_primos);
    fprintf(arquivo, "\n");

    fclose(arquivo);

    // Também imprime na tela
    printf("\n=== RESULTADOS ===\n");
    printf("Total de números processados: %d\n", N);
    printf("Total de números primos encontrados: %d\n", primos_totais);
    printf("\nDesempenho das threads consumidoras:\n");
    for (i = 0; i < num_consumidores; i++) {
        printf("  Thread %d: %d primos\n", dados_cons[i].id, dados_cons[i].primos_encontrados);
    }
    printf("\nThread VENCEDORA: Thread %d (%d primos)\n", vencedor_id, max_primos);
    printf("\nResultados salvos em: resultados.txt\n");

    // Libera recursos
    sem_destroy(&mutex);
    sem_destroy(&vazio);
    sem_destroy(&cheio);
    free(buffer_compartilhado.buffer);
    free(threads_cons);
    free(dados_cons);

    return 0;
}
