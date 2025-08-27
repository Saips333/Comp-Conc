CC = gcc
CFLAGS = -Wall -O2 -pthread
LDFLAGS = -lm -pthread

# Alvos principais
all: gerador produto

gerador: gerador.c
	$(CC) $(CFLAGS) -o gerador gerador.c $(LDFLAGS)

produto: produto.c timer.h
	$(CC) $(CFLAGS) -o produto produto.c $(LDFLAGS)

# Limpeza
clean:
	rm -f gerador produto teste_*.bin dados_laboratorio.txt

# Testa a compilação
test: all
	@echo "✅ Compilação concluída com sucesso!"
	@echo "📋 Programas disponíveis:"
	@echo "   - gerador: cria arquivos de teste"
	@echo "   - produto: calcula produto interno"
	@echo ""
	@echo "Para executar a coleta de dados:"
	@echo "   chmod +x coleta_dados.sh"
	@echo "   ./coleta_dados.sh"

.PHONY: all clean test