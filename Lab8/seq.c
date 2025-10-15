#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

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

// Função para contar primos sequencialmente
int contarPrimosSequencial(int N) {
    int count = 0;
    long long int i;

    for (i = 1; i <= N; i++) {
        if (ehPrimo(i)) {
            count++;
        }
    }

    return count;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <N>\n", argv[0]);
        printf("  N: quantidade total de números para verificar\n");
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        printf("Erro: N deve ser positivo\n");
        return 1;
    }

    printf("Calculando quantidade esperada de primos até %d...\n", N);
    int primos_esperados = contarPrimosSequencial(N);

    printf("\n=== RESULTADO ESPERADO ===\n");
    printf("Total de números primos de 1 até %d: %d\n", N, primos_esperados);
    printf("\nCompare este resultado com a saída do lab8 para verificar corretude.\n");

    // Salva resultado em arquivo para comparação
    FILE *arquivo = fopen("resultado_esperado.txt", "a");
    if (arquivo != NULL) {
        fprintf(arquivo, "N=%d\n", N);
        fprintf(arquivo, "Primos esperados: %d\n", primos_esperados);
        fclose(arquivo);
        printf("\nResultado salvo em: resultado_esperado.txt\n");
    }

    return 0;
}
