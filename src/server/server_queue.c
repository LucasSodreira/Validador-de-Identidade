#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "queue.h"
#include "../protocol.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#define close closesocket
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#define MAX_CLIENTS 10

Queue* id_queue = NULL;

#ifdef _WIN32
HANDLE queue_mutex;
#else
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef _WIN32
DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
#else
void* handle_client(void* client_socket_ptr) {
#endif
    SOCKET client_socket = *(SOCKET*)client_socket_ptr;
    free(client_socket_ptr);
    char buffer[2048];
    size_t pending = 0;
    id_t id;

    while (1) {
        int read_size = recv(client_socket, buffer + pending, sizeof(buffer) - 1 - (int)pending, 0);
        if (read_size <= 0) {
            printf("[SERVIDOR] Cliente desconectado.\n");
            break;
        }
        int total = (int)pending + read_size;
        buffer[total] = '\0';

        char *start = buffer;
        char *nl;
        while ((nl = strchr(start, '\n')) != NULL) {
            *nl = '\0';
            char *line = start;
            // Remover \r
            char *cr = strchr(line, '\r'); if (cr) *cr = '\0';

            // Log do comando recebido
            log_request("SERVER_QUEUE RECV: %s", line);

            if (strncmp(line, CMD_GETB, strlen(CMD_GETB)) == 0) {
            // Formato: GETB <n>
            long req = 0;
            char *p = line + strlen(CMD_GETB);
            while (*p == ' ') p++;
            if (*p) req = strtoll(p, NULL, 10);
            if (req <= 0) req = 1;
            if (req > MAX_BATCH_SIZE) req = MAX_BATCH_SIZE; // limite saneador
            id_t ids_local[MAX_BATCH_SIZE];
            int count = 0;
#ifdef _WIN32
            WaitForSingleObject(queue_mutex, INFINITE);
#else
            pthread_mutex_lock(&queue_mutex);
#endif
            while (count < req) {
                id_t tmp;
                if (!queue_dequeue(id_queue, &tmp)) break;
                ids_local[count++] = tmp;
            }
#ifdef _WIN32
            ReleaseMutex(queue_mutex);
#else
            pthread_mutex_unlock(&queue_mutex);
#endif
            if (count == 0) {
                log_response("SERVER_QUEUE SEND: %s", RESP_EMPTY);
                send(client_socket, RESP_EMPTY, (int)strlen(RESP_EMPTY), 0);
                // não encerra o loop de leitura imediatamente; cliente deverá fechar
            } else {
                // Montar resposta
                char outbuf[8192];
                int offset = snprintf(outbuf, sizeof(outbuf), "%d", count);
                for (int i = 0; i < count && offset < (int)sizeof(outbuf); ++i) {
                    offset += snprintf(outbuf + offset, sizeof(outbuf) - offset, " %lld", (long long)ids_local[i]);
                }
                if (offset < (int)sizeof(outbuf) - 2) {
                    outbuf[offset++]='\n';
                }
                log_response("SERVER_QUEUE SEND: %.*s", offset, outbuf);
                send(client_socket, outbuf, offset, 0);
            }
        } else if (strcmp(line, CMD_GET) == 0) {
#ifdef _WIN32
            WaitForSingleObject(queue_mutex, INFINITE);
#else
            pthread_mutex_lock(&queue_mutex);
#endif
            int success = queue_dequeue(id_queue, &id);
#ifdef _WIN32
            ReleaseMutex(queue_mutex);
#else
            pthread_mutex_unlock(&queue_mutex);
#endif

            if (success) {
                char response[64];
                int len = snprintf(response, sizeof(response), "%lld\n", id);
                send(client_socket, response, len, 0);
                log_response(response);
            } else {
                send(client_socket, RESP_EMPTY, (int)strlen(RESP_EMPTY), 0);
                log_response(RESP_EMPTY);
                // idem: não encerra imediatamente
            }
        }
        // próxima linha
        start = nl + 1;
        }
        // guarda sobras sem '\n' no início do buffer
        pending = (size_t)(buffer + total - start);
        if (pending > 0 && start != buffer) memmove(buffer, start, pending);
    }

    close(client_socket);
    return 0;
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Falhou. Código de erro: %d", WSAGetLastError());
        return 1;
    }
#endif

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <porta> <num_ids>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Inicializa sistema de log (arquivos dedicados ao servidor)
    init_logging("server_request.txt", "server_responses.txt");

    int port = atoi(argv[1]);
    long long num_ids = atoll(argv[2]);

    srand((unsigned)time(NULL));
    printf("[SERVIDOR] Gerando %lld IDs...\n", num_ids);
    id_t* ids = malloc(sizeof(id_t) * num_ids);
    if (!ids) {
        perror("Falha ao alocar memória para IDs");
        exit(EXIT_FAILURE);
    }
    for (long long i = 0; i < num_ids; ++i) {
        ids[i] = i + 1;
        // Mostra progresso a cada 1% ou a cada 1 milhão, o que for menor
        long long step = num_ids / 100;
        if (step < 1000000) step = 1000000;
        if ((i+1) % step == 0 || i == num_ids - 1) {
            double percent = 100.0 * (i+1) / num_ids;
            if (percent > 100.0) percent = 100.0;
            printf("[SERVER] Criando IDs: %.1f%% (%lld/%lld)\r", percent, i+1, num_ids);
            fflush(stdout);
        }
    }
    printf("\n");

    printf("[SERVIDOR] Embaralhando IDs...\n");
    shuffle(ids, num_ids);

    printf("[SERVIDOR] Armazenando IDs na Fila...\n");
    id_queue = queue_create();
    for (long long i = 0; i < num_ids; ++i) {
        queue_enqueue(id_queue, ids[i]);
    }
    free(ids);
    printf("[SERVIDOR] Todos os IDs armazenados. Servidor pronto.\n");

#ifdef _WIN32
    queue_mutex = CreateMutex(NULL, FALSE, NULL);
#endif

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    listen(server_socket, MAX_CLIENTS);
    printf("[SERVIDOR] Aguardando conexões na porta %d...\n", port);

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len))) {
        printf("[SERVIDOR] Conexão aceita.\n");
        SOCKET* new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;

#ifdef _WIN32
        CreateThread(NULL, 0, handle_client, new_sock, 0, NULL);
#else
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
        }
#endif
    }

    if (client_socket == INVALID_SOCKET) {
        perror("Accept failed");
    }

    queue_destroy(id_queue);
    close(server_socket);
#ifdef _WIN32
    CloseHandle(queue_mutex);
    WSACleanup();
#endif
    
    // Fecha sistema de log
    close_logging();
    return 0;
}
