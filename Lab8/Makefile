CC=gcc
CFLAGS=-Wall -pthread -lm
TARGET=lab8

all: $(TARGET)

$(TARGET): lab8.c
	$(CC) -o $(TARGET) lab8.c $(CFLAGS)

clean:
	rm -f $(TARGET) resultados.txt

run: $(TARGET)
	./$(TARGET) 100 10 4

test: $(TARGET)
	@echo "=== Iniciando testes ==="
	@rm -f resultados.txt
	@echo "\n--- Testes com diferentes escalas ---"
	@echo "\nTeste 1: N=100, M=5, 2 consumidores"
	@./$(TARGET) 100 5 2
	@echo "\nTeste 2: N=1000, M=10, 4 consumidores"
	@./$(TARGET) 1000 10 4
	@echo "\nTeste 3: N=10000, M=50, 8 consumidores"
	@./$(TARGET) 10000 50 8
	@echo "\nTeste 4: N=100000, M=100, 16 consumidores"
	@./$(TARGET) 100000 100 16
	@echo "\n--- Testes comparativos (N=10000, M=50) ---"
	@echo "\nTeste 5: 2 consumidores"
	@./$(TARGET) 10000 50 2
	@echo "\nTeste 6: 4 consumidores"
	@./$(TARGET) 10000 50 4
	@echo "\nTeste 7: 8 consumidores"
	@./$(TARGET) 10000 50 8
	@echo "\nTeste 8: 16 consumidores"
	@./$(TARGET) 10000 50 16
	@echo "\nTeste 9: 32 consumidores"
	@./$(TARGET) 10000 50 32
	@echo "\n=== Todos os testes salvos em resultados.txt ==="

.PHONY: all clean run test
