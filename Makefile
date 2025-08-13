# Makefile para o Projeto Validador de Identidades Únicas
# Compila servidores (fila, pilha, lista) e cliente para teste de performance

# Configurações do compilador
CC = gcc
WARN = -Wall -Wextra -Wpedantic
OPT_DEBUG = -O0 -g
OPT_RELEASE = -O3 -march=native -flto
THREAD = -pthread
DEFS =
COMMON_CFLAGS = $(WARN) $(THREAD) $(DEFS)
LDFLAGS = -lm -lws2_32

# Diretórios
SRC_DIR = src
INC_DIR = include . src
INCLUDES = $(addprefix -I, $(INC_DIR))
BIN_DIR = bin

# Arquivos fonte organizados por categoria
SRCS_COMMON = src/common/utils.c
SRCS_QUEUE = src/server/server_queue.c src/common/queue.c
SRCS_STACK = src/server/server_stack.c src/common/stack.c
SRCS_LIST  = src/server/server_list.c src/common/linked_list.c
SRCS_CLIENT = src/client/client.c

# Objetos correspondentes
OBJS_QUEUE = $(SRCS_QUEUE:.c=.o) src/common/utils.o
OBJS_STACK = $(SRCS_STACK:.c=.o) src/common/utils.o
OBJS_LIST  = $(SRCS_LIST:.c=.o) src/common/utils.o
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o) src/common/utils.o

# Targets principais
.PHONY: all debug release clean format stop logs-clean rebuild

# Target padrão - para, limpa e compila (release)
all: rebuild

# Modo debug - sem otimizações, com símbolos de debug
debug: CFLAGS = $(COMMON_CFLAGS) $(OPT_DEBUG) $(INCLUDES)
debug: $(BIN_DIR)/server_queue $(BIN_DIR)/server_stack $(BIN_DIR)/server_list $(BIN_DIR)/client

# Modo release - otimizações máximas
release: CFLAGS = $(COMMON_CFLAGS) $(OPT_RELEASE) $(INCLUDES)
release: $(BIN_DIR)/server_queue $(BIN_DIR)/server_stack $(BIN_DIR)/server_list $(BIN_DIR)/client

# Regras de linking para cada executável
$(BIN_DIR)/server_queue: $(OBJS_QUEUE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/server_stack: $(OBJS_STACK) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/server_list: $(OBJS_LIST) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/client: $(OBJS_CLIENT) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Cria diretório bin se não existir
$(BIN_DIR):
	mkdir $(BIN_DIR)

# Regra genérica para compilação de arquivos .c para .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza de arquivos objeto e executáveis
clean:
	del /q src\common\*.o src\server\*.o src\client\*.o 2>nul
	del /q bin\server_queue.exe bin\server_stack.exe bin\server_list.exe bin\client.exe 2>nul

# Encerra binários em execução (ignora erro se não estiverem rodando)
stop:
	-@taskkill /f /im server_queue.exe 2>nul
	-@taskkill /f /im server_stack.exe 2>nul
	-@taskkill /f /im server_list.exe 2>nul
	-@taskkill /f /im client.exe 2>nul

# Limpa os arquivos de log mais comuns (raiz e bin/)
logs-clean:
	-@del /q responses.txt request.txt client_responses.txt client_request.txt server_responses.txt server_request.txt 2>nul
	-@del /q bin\*.txt 2>nul

# Para tudo, limpa objetos/execs e recompila em release
rebuild: stop clean release
