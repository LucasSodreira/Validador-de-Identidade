# Projeto Validador de Identidades Únicas

Este projeto implementa um sistema de geração de identidades únicas usando três estruturas de dados diferentes: Fila, Pilha e Lista Encadeada. O objetivo é comparar a performance de cada estrutura para gerar e fornecer um grande volume de IDs.

## Estrutura do Projeto

```
├── src/
│   ├── server/                 # Implementações dos servidores
│   │   ├── server_queue.c     # Servidor usando fila
│   │   ├── server_stack.c     # Servidor usando pilha
│   │   └── server_list.c      # Servidor usando lista encadeada
│   ├── client/                 # Cliente
│   │   └── client.c           # Cliente que requisita IDs
│   └── common/                 # Código compartilhado
│       ├── utils.c            # Funções auxiliares
│       ├── queue.c            # Implementação da fila
│       ├── stack.c            # Implementação da pilha
│       └── linked_list.c      # Implementação da lista encadeada
├── include/                    # Headers do protocolo
│   └── protocol.h             # Definições do protocolo de comunicação
├── bin/                        # Executáveis compilados
│   ├── server_queue.exe       # Servidor com fila
│   ├── server_stack.exe       # Servidor com pilha
│   ├── server_list.exe        # Servidor com lista encadeada
│   └── client.exe             # Cliente
├── *.h                        # Headers das estruturas de dados
├── Makefile                   # Arquivo de compilação
└── README.md                  # Este arquivo
```

## Como Compilar

Para compilar todos os executáveis, use o comando:

```bash
make
```

Modos:

- Debug (sem otimizações, com símbolos):
```bash
make debug
```
- Release (O3, LTO, march=native):
```bash
make release
```

Para compilar um alvo específico (por exemplo, `server_queue`):

```bash
make bin/server_queue
```

## Como Executar os Testes

1.  **Abra dois terminais.**

2.  **No primeiro terminal, inicie um dos servidores.**
    Você precisa fornecer a porta e o número de IDs a serem gerados.

    **Exemplo (Servidor de Fila, 1 milhão de IDs, porta 8080):**
    ```bash
    bin/server_queue.exe 8080 1000000
    ```
    O servidor irá gerar e embaralhar os IDs, e então aguardará o cliente.

3.  **No segundo terminal, inicie o cliente.**
    Você precisa fornecer o endereço (localhost), a porta e o número de IDs a serem requisitados (deve ser o mesmo número do servidor).
    
    **Exemplo (Cliente solicitando 1 milhão de IDs):**
    ```bash
    bin/client.exe 127.0.0.1 8080 1000000
    ```
    
    **Para requisições em lote (mais eficiente):**
    ```bash
    bin/client.exe 127.0.0.1 8080 1000000 1000
    ```
    O último parâmetro é o tamanho do lote (opcional).

4.  **Analise o resultado.**
    O cliente irá requisitar todos os IDs e, ao final, imprimirá o tempo total da operação.

    Exemplo de saída:
    ```
    [CLIENT] Connected to server. Requesting 1000000 IDs (batch=1)...
    [CLIENT] Received 1000000/1000000 IDs.
    [CLIENT] Total time taken: 5.821 seconds.
    ```

### Testes de Grande Volume (100M e 1G)

-   **100 Milhões (100M):** `100000000`
    -   Este teste requer aproximadamente 800 MB de RAM.
    -   Servidor: `bin/server_queue.exe 8080 100000000`
    -   Cliente: `bin/client.exe 127.0.0.1 8080 100000000`

-   **1 Bilhão (1G):** `1000000000`
    -   **AVISO:** Este teste requer pelo menos 8 GB de RAM livres e pode ser muito lento ou falhar em sistemas com memória insuficiente.
    -   Servidor: `bin/server_queue.exe 8080 1000000000`
    -   Cliente: `bin/client.exe 127.0.0.1 8080 1000000000`

Repita os testes para `bin/server_stack.exe` e `bin/server_list.exe` para comparar a performance.

## Protocolo de Comunicação

O projeto implementa um protocolo simples baseado em texto:

### Comandos do Cliente
- `GET`: Solicita um único ID
- `GETB <n>`: Solicita até `n` IDs em lote (mais eficiente)

### Respostas do Servidor
- `<id>\n`: ID único (long long, termina com newline)
- `<count> <id1> <id2> ... <idN>\n`: Resposta em lote (termina com newline)
- `EMPTY\n`: Não há mais IDs disponíveis

### Exemplo de Comunicação
```
Cliente -> Servidor: GET
Servidor -> Cliente: 12345

Cliente -> Servidor: GETB 3
Servidor -> Cliente: 3 12346 12347 12348
```

## Arquitetura das Estruturas de Dados

### Fila (Queue)
- **FIFO** (First In, First Out)
- IDs são adicionados no final e removidos do início
- Melhor para distribuição sequencial

### Pilha (Stack)
- **LIFO** (Last In, First Out)
- IDs são adicionados e removidos do topo
- Distribuição em ordem reversa

### Lista Encadeada (Linked List)
- Inserção no início, remoção do início
- Comportamento similar à pilha nesta implementação
- Flexibilidade para futuras modificações

## Otimizações Implementadas e Recomendadas

### Já Implementadas
1. **Protocolo em lote**: Cliente pode solicitar múltiplos IDs de uma vez com `GETB <n>`
2. **Embaralhamento Fisher-Yates**: Distribuição aleatória eficiente dos IDs
3. **Multithreading**: Suporte a múltiplos clientes simultâneos
4. **Otimizações de compilação**: LTO e `-march=native` no modo release
5. **Multiplataforma**: Suporte para Windows e Linux/Unix


### Sistema Operacional
- **Windows**: MinGW-w64 ou Visual Studio
- **Linux/Unix**: GCC com suporte a pthreads

### Bibliotecas
- **Windows**: ws2_32 (Winsock2)
- **Linux/Unix**: pthread, libm

### Memória
- **Teste básico (1M IDs)**: ~8 MB RAM
- **Teste médio (100M IDs)**: ~800 MB RAM  
- **Teste intensivo (1B IDs)**: ~8 GB RAM

## Resolução de Problemas

### Erro de Compilação
```bash
# Limpar arquivos objeto e recompilar
make clean
make
```

### Erro "Permission denied" 
```bash
# Parar processos em execução
taskkill /f /im server_queue.exe
taskkill /f /im server_stack.exe
taskkill /f /im server_list.exe
```

### Servidor não aceita conexões
- Verificar se a porta está disponível
- Executar como administrador se necessário
- Verificar firewall do Windows

## Métricas de Performance

### Fatores que Afetam Performance
1. **Tamanho do lote**: Lotes maiores reduzem overhead de rede
2. **Estrutura de dados**: Fila geralmente mais eficiente que lista encadeada
3. **Número de IDs**: Overhead de setup se torna insignificante com volumes grandes
4. **Hardware**: CPU, RAM e velocidade de rede

### Benchmark Típico (1M IDs, lote=1000)
- **Fila**: ~2-3 segundos
- **Pilha**: ~2-3 segundos  
- **Lista**: ~3-4 segundos

*Resultados podem variar conforme hardware e configuração de rede.*

## Limites e compatibilidade

- Lotes: o servidor impõe limite de `MAX_BATCH_SIZE` por resposta (atualmente 1024, ver `src/protocol.h`). Ajuste cliente/servidor em conjunto se alterar.
- Newlines: todas as variantes de servidor agora terminam respostas com `\n` para consistência.
