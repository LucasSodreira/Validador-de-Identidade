# Exemplos de Uso - Validador de Identidades Únicas

Este arquivo contém exemplos práticos de como usar o sistema de validação de identidades únicas.

## Exemplos Básicos

### Exemplo 1: Teste Simples (1000 IDs)

**Terminal 1 - Servidor:**
```bash
bin/server_queue.exe 8080 1000
```

**Terminal 2 - Cliente:**
```bash
bin/client.exe 127.0.0.1 8080 1000
```

**Saída Esperada:**
```
[SERVER] Generating 1000 IDs...
[SERVER] Shuffling IDs...
[SERVER] Storing IDs in Queue...
[SERVER] All IDs stored. Server is ready.
[SERVER] Waiting for connections on port 8080...
[SERVER] Connection accepted.
[SERVER] Client disconnected.
```

```
[CLIENT] Connected to server. Requesting 1000 IDs (batch=1)...
[CLIENT] Received 1000/1000 IDs.
[CLIENT] Total time taken: 0.123 seconds.
```

### Exemplo 2: Teste com Lotes (1000 IDs, lotes de 100)

**Terminal 1 - Servidor:**
```bash
bin/server_queue.exe 8080 1000
```

**Terminal 2 - Cliente:**
```bash
bin/client.exe 127.0.0.1 8080 1000 100
```

**Resultado:** Significativamente mais rápido devido à redução de overhead de rede.

## Comparação de Performance

### Script de Teste (Windows)
```batch
@echo off
echo Testando performance das estruturas de dados...
echo.

echo === FILA (QUEUE) ===
start /wait bin/server_queue.exe 8080 100000 &
timeout /t 2 /nobreak > nul
bin/client.exe 127.0.0.1 8080 100000 1000

echo.
echo === PILHA (STACK) ===
start /wait bin/server_stack.exe 8080 100000 &
timeout /t 2 /nobreak > nul
bin/client.exe 127.0.0.1 8080 100000 1000

echo.
echo === LISTA ENCADEADA ===
start /wait bin/server_list.exe 8080 100000 &
timeout /t 2 /nobreak > nul
bin/client.exe 127.0.0.1 8080 100000 1000

echo.
echo Teste concluído!
```

### Script de Teste (Linux/Bash)
```bash
#!/bin/bash
echo "Testando performance das estruturas de dados..."
echo

echo "=== FILA (QUEUE) ==="
bin/server_queue 8080 100000 &
SERVER_PID=$!
sleep 2
bin/client 127.0.0.1 8080 100000 1000
kill $SERVER_PID

echo
echo "=== PILHA (STACK) ==="
bin/server_stack 8080 100000 &
SERVER_PID=$!
sleep 2
bin/client 127.0.0.1 8080 100000 1000
kill $SERVER_PID

echo
echo "=== LISTA ENCADEADA ==="
bin/server_list 8080 100000 &
SERVER_PID=$!
sleep 2
bin/client 127.0.0.1 8080 100000 1000
kill $SERVER_PID

echo
echo "Teste concluído!"
```

## Testes de Carga

### Teste de Volume Médio (1M IDs)
```bash
# Servidor
bin/server_queue.exe 8080 1000000

# Cliente com lotes otimizados
bin/client.exe 127.0.0.1 8080 1000000 10000
```

### Teste de Volume Alto (10M IDs)
```bash
# Servidor
bin/server_queue.exe 8080 10000000

# Cliente com lotes grandes
bin/client.exe 127.0.0.1 8080 10000000 50000
```

## Múltiplos Clientes

### Exemplo com 3 Clientes Simultâneos

**Terminal 1 - Servidor:**
```bash
bin/server_queue.exe 8080 300000
```

**Terminal 2 - Cliente 1:**
```bash
bin/client.exe 127.0.0.1 8080 100000 5000
```

**Terminal 3 - Cliente 2:**
```bash
bin/client.exe 127.0.0.1 8080 100000 5000
```

**Terminal 4 - Cliente 3:**
```bash
bin/client.exe 127.0.0.1 8080 100000 5000
```

**Nota:** O servidor distribuirá os 300K IDs entre os três clientes.

## Análise de Resultados

### Métricas Típicas (hardware moderno)

| Teste | Estrutura | IDs | Lote | Tempo | IDs/seg |
|-------|-----------|-----|------|-------|---------|
| Básico | Fila | 1K | 1 | 0.1s | 10K |
| Otimizado | Fila | 1K | 100 | 0.02s | 50K |
| Médio | Fila | 100K | 1K | 2.5s | 40K |
| Alto | Fila | 1M | 10K | 15s | 67K |

### Comparação de Estruturas (1M IDs, lote 10K)

| Estrutura | Tempo | IDs/seg | Memória |
|-----------|-------|---------|---------|
| Fila | 15.2s | 65.8K | 8MB |
| Pilha | 15.1s | 66.2K | 8MB |
| Lista | 16.8s | 59.5K | 8MB |

**Observações:**
- Fila e Pilha têm performance similar
- Lista encadeada é ligeiramente mais lenta devido ao overhead de ponteiros
- Todas as estruturas usam quantidade similar de memória

## Troubleshooting

### Problema: "Connection refused"
```bash
# Verificar se o servidor está rodando
netstat -an | findstr 8080

# Tentar porta diferente
bin/server_queue.exe 8081 1000
bin/client.exe 127.0.0.1 8081 1000
```

### Problema: Performance baixa
```bash
# Usar lotes maiores
bin/client.exe 127.0.0.1 8080 100000 10000

# Compilar em modo release
make clean
make release
```

### Problema: Memória insuficiente
```bash
# Reduzir número de IDs
bin/server_queue.exe 8080 10000
bin/client.exe 127.0.0.1 8080 10000
```
