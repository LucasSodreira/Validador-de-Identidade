# Instruções para agentes de IA no projeto "Validador de Identidades Únicas"

Este repositório é um benchmark de geração/distribuição de IDs via TCP com três variantes de servidor (fila, pilha, lista) e um cliente. Use estas diretrizes para navegar, editar e estender o código com segurança.

## Visão geral e arquitetura
- Código-fonte em `src/`; binários em `bin/`; utilitários/estruturas em `src/common/`; headers em `src/*.h`.
- Servidores: `src/server/server_queue.c`, `server_stack.c`, `server_list.c` (mesmo protocolo; muda a estrutura de dados interna).
- Cliente: `src/client/client.c` (conecta via TCP, envia comandos e mede tempo).
- Protocolo textual (definido em `src/protocol.h`): cliente envia `GET` (1 ID) ou `GETB <n>` (lote). Servidor responde com `EMPTY\n` (acabou), `<id>\n` (unitário) ou `<count> <id1> ... <idN>\n` (lote). Todas as respostas terminam com `\n`.
- Geração: servidor cria `num_ids` sequenciais, chama `shuffle(...)` e carrega na estrutura com mutex para acesso concorrente.

## Build e execução
- Makefile: `make` (release), `make debug`, `make release`, ou alvos específicos (ex.: `make bin/server_queue`). Saída em `bin/`.
- Execução típica (cmd.exe):
  - Servidor (fila): `bin\server_queue.exe 8080 1000000`
  - Cliente: `bin\client.exe 127.0.0.1 8080 1000000 1000`
- Logs thread-safe em `request.txt` e `responses.txt` no diretório onde o processo inicia (veja `utils.c`).

## Convenções específicas do código
- Cross-platform: `#ifdef _WIN32` para rede/threads; em Windows use Winsock2 e mapeie `close` -> `closesocket`.
- Sincronização: proteja operações na estrutura com mutex (`CreateMutex`/`pthread_mutex_t`).
- Batches: limite unificado via `MAX_BATCH_SIZE` em `src/protocol.h` (atual: 1024). Ajuste cliente/servidores em conjunto se mudar.
- Embaralhamento: `shuffle(...)` (Fisher–Yates) não chama `srand`; inicialize a semente no `main` do servidor antes de `shuffle`.
- Headers: prefira `src/protocol.h`. Atenção: `include/protocol.h` existe mas está vazio; evite inclui-lo por engano.

## Padrões por arquivo (exemplos úteis)
- `server_queue.c`: normaliza `\r\n` após `recv`, limita `GETB`, monta resposta em único `send`.
- `server_list.c`: usa `MAX_BATCH_SIZE` compartilhado, otimiza buffers de socket, garante `\n` nas respostas.
- `server_stack.c`: agora padronizado para responder com `\n` e usar `MAX_BATCH_SIZE`.
- `client.c`: loga comandos/respostas, parseia `<count>` e descarta IDs (benchmark puro), batch opcional por 4º arg.

## Pontos de extensão
- Novo servidor: copie um `server_*.c`, mova a estrutura para `src/common/` com header em `src/`, proteja com mutex e mantenha o mesmo contrato do protocolo.
- Makefile: adicione `SRCS_*`/`OBJS_*` e regra de link como as existentes; inclua nos alvos `debug/release`.

## Gotchas e troubleshooting
- Diferenças de `\n`: padronizar respostas pode exigir tocar cliente e servidores; valide ambos.
- Memória: 100M/1B IDs consomem muita RAM (veja README); use lotes grandes para melhor throughput.
- Windows: se arquivos `.exe` ficarem travados, finalize com `taskkill /f /im server_*.exe` antes de `make clean`.

Arquivos-chave: `src/protocol.h`, `src/common/utils.c` (tempo, shuffle, logs), `src/common/{queue,stack,linked_list}.c`, `src/server/*.c`, `src/client/client.c`, `Makefile`.
