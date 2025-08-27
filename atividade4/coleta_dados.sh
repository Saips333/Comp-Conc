#!/bin/bash

# Script CORRIGIDO para coletar dados do laboratório
# Coleta: dimensões, threads, tempo médio (5 exec), variação relativa

echo "🚀 COLETANDO DADOS DO LABORATÓRIO"
echo "=================================="

ARQUIVO="dados_laboratorio.txt"

# Verifica se programas foram compilados
if [ ! -f "./gerador" ] || [ ! -f "./produto" ]; then
    echo "❌ ERRO: Execute 'make all' primeiro!"
    exit 1
fi

# Informações do sistema
echo "📊 Informações do Sistema:"
NUM_PROC=$(nproc 2>/dev/null || echo "N/A")
CPU=$(cat /proc/cpuinfo 2>/dev/null | grep "model name" | head -1 | cut -d: -f2 | xargs || echo "N/A")
echo "  Processadores: $NUM_PROC"
echo "  CPU: $CPU"

# Cabeçalho do arquivo
cat > $ARQUIVO << EOF
DADOS DO LABORATÓRIO - PRODUTO INTERNO CONCORRENTE
==================================================
Data: $(date)
Processadores: $NUM_PROC  
CPU: $CPU

CONFIGURAÇÕES TESTADAS:
- Dimensões: 100, 1000, 10.000 elementos
- Threads: 1, 2, 4, 8
- Execuções por configuração: 5 (para média)

EOF

# Configurações
DIMENSOES=(100 1000 10000)
THREADS=(1 2 4 8)
EXECUCOES=5

echo ""
echo "🔧 Gerando casos de teste..."
for dim in "${DIMENSOES[@]}"; do
    arquivo="teste_${dim}.bin"
    if [ ! -f "$arquivo" ]; then
        echo "  Criando $arquivo..."
        ./gerador $dim $arquivo > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "❌ ERRO ao gerar $arquivo"
            exit 1
        fi
    fi
done

echo ""
echo "⏱️  Coletando tempos (5 execuções por configuração)..."

# Tabela de resultados
printf "\n%-10s| %-3s| %-12s| %-12s| %-12s\n" "Dimensão" "Thr" "Tempo_Médio" "Desvio" "Var_Relativa" >> $ARQUIVO
echo "----------|----|--------------|--------------|--------------" >> $ARQUIVO

for dim in "${DIMENSOES[@]}"; do
    arquivo="teste_${dim}.bin"
    echo ""
    echo "🔍 Testando $dim elementos..."
    
    for t in "${THREADS[@]}"; do
        echo "  📈 $t thread(s): executando 5 vezes..."
        
        # Arrays para guardar resultados
        tempos=()
        variacoes=()
        
        # 5 execuções
        for i in $(seq 1 $EXECUCOES); do
            echo -n "    Run $i... "
            
            # Executa o programa e captura a saída
            resultado=$(./produto $arquivo $t 2>/dev/null)
            
            if [ $? -eq 0 ]; then
                # Extrai tempo de execução da linha "TEMPO_EXECUCAO: X.XXXXXX"
                tempo=$(echo "$resultado" | grep "TEMPO_EXECUCAO:" | cut -d: -f2 | xargs)
                
                # Extrai variação relativa da linha "Variacao relativa: X.XXeXX"  
                variacao=$(echo "$resultado" | grep "Variacao relativa:" | cut -d: -f2 | xargs)
                
                # Verifica se conseguiu extrair valores válidos
                if [ ! -z "$tempo" ] && [ ! -z "$variacao" ]; then
                    tempos+=($tempo)
                    variacoes+=($variacao)
                    printf "%.6fs ✓\n" "$tempo"
                else
                    echo "erro (parsing) ❌"
                fi
            else
                echo "erro (execução) ❌"
            fi
        done
        
        # Calcula estatísticas se tem dados suficientes
        if [ ${#tempos[@]} -gt 2 ]; then
            # Calcula média dos tempos usando awk
            media=$(printf '%s\n' "${tempos[@]}" | awk '{sum+=$1} END {printf "%.6f", sum/NR}')
            
            # Calcula desvio padrão usando awk
            desvio=$(printf '%s\n' "${tempos[@]}" | awk -v media="$media" '{sum+=($1-media)^2} END {printf "%.6f", sqrt(sum/NR)}')
            
            # Média das variações usando awk  
            media_var=$(printf '%s\n' "${variacoes[@]}" | awk '{sum+=$1} END {printf "%.2e", sum/NR}')
            
            # Salva no arquivo
            printf "%-10s| %-3s| %-12s| %-12s| %-12s\n" "$dim" "$t" "$media" "$desvio" "$media_var" >> $ARQUIVO
            
            echo "    📊 Média: ${media}s (±${desvio}s), Variação: ${media_var}"
        else
            printf "%-10s| %-3s| %-12s| %-12s| %-12s\n" "$dim" "$t" "ERRO" "ERRO" "ERRO" >> $ARQUIVO
            echo "    ❌ Dados insuficientes (${#tempos[@]} execuções válidas)"
        fi
    done
done

# Adiciona análise
cat >> $ARQUIVO << EOF

echo ""
echo "✅ COLETA CONCLUÍDA!"
echo "===================="
echo "📄 Arquivo: $ARQUIVO"
echo "📊 Dados coletados:"
echo "   - Sistema: $NUM_PROC processadores"
echo "   - Dimensões: ${DIMENSOES[@]}"
echo "   - Threads: ${THREADS[@]}"
echo "   - Execuções: $EXECUCOES por configuração"
echo ""
echo "Para ver os resultados:"
echo "  cat $ARQUIVO"
echo ""
echo "===================="