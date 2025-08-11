#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utils.h"
#include "../queue.h"
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
    char buffer[1024];
    id_t id;

    while (1) {
        int read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (read_size <= 0) {
            printf("[SERVER] Client disconnected.\n");
            break;
        }
        buffer[read_size] = '\0';

        // Remover \r\n
        char *cr = strchr(buffer, '\r'); if (cr) *cr = '\0';
        char *lf = strchr(buffer, '\n'); if (lf) *lf = '\0';

        if (strncmp(buffer, CMD_GETB, strlen(CMD_GETB)) == 0) {
            // Formato: GETB <n>
            long req = 0;
            char *p = buffer + strlen(CMD_GETB);
            while (*p == ' ') p++;
            if (*p) req = strtoll(p, NULL, 10);
            if (req <= 0) req = 1;
            if (req > 1024) req = 1024; // limite saneador

            id_t ids_local[1024];
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
                send(client_socket, RESP_EMPTY, (int)strlen(RESP_EMPTY), 0);
                break; // encerra
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
                send(client_socket, outbuf, offset, 0);
            }
        } else if (strcmp(buffer, CMD_GET) == 0) {
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
            } else {
                send(client_socket, RESP_EMPTY, (int)strlen(RESP_EMPTY), 0);
                break;
            }
        }
    }

    close(client_socket);
    return 0;
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
#endif

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <num_ids>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    long long num_ids = atoll(argv[2]);

    srand((unsigned)time(NULL));
    printf("[SERVER] Generating %lld IDs...\n", num_ids);
    id_t* ids = malloc(sizeof(id_t) * num_ids);
    if (!ids) {
        perror("Failed to allocate memory for IDs");
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

    printf("[SERVER] Shuffling IDs...\n");
    shuffle(ids, num_ids);

    printf("[SERVER] Storing IDs in Queue...\n");
    id_queue = queue_create();
    for (long long i = 0; i < num_ids; ++i) {
        queue_enqueue(id_queue, ids[i]);
    }
    free(ids);
    printf("[SERVER] All IDs stored. Server is ready.\n");

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
    printf("[SERVER] Waiting for connections on port %d...\n", port);

    while ((client_socket = accept(server_socket, (struct sockaddr*)&server_addr, &client_len))) {
        printf("[SERVER] Connection accepted.\n");
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
    return 0;
}
