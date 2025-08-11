#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utils.h"
#include "../stack.h"
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

Stack* id_stack = NULL;

#ifdef _WIN32
HANDLE stack_mutex;
#else
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;
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

        if (strcmp(buffer, "GET") == 0) {
#ifdef _WIN32
            WaitForSingleObject(stack_mutex, INFINITE);
#else
            pthread_mutex_lock(&stack_mutex);
#endif
            int success = stack_pop(id_stack, &id);
#ifdef _WIN32
            ReleaseMutex(stack_mutex);
#else
            pthread_mutex_unlock(&stack_mutex);
#endif

            if (success) {
                char response[64];
                snprintf(response, sizeof(response), "%lld", id);
                send(client_socket, response, strlen(response), 0);
            } else {
                send(client_socket, "EMPTY", 5, 0);
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
    }

    printf("[SERVER] Shuffling IDs...\n");
    shuffle(ids, num_ids);

    printf("[SERVER] Storing IDs in Stack...\n");
    id_stack = stack_create();
    for (long long i = 0; i < num_ids; ++i) {
        stack_push(id_stack, ids[i]);
    }
    free(ids);
    printf("[SERVER] All IDs stored. Server is ready.\n");

#ifdef _WIN32
    stack_mutex = CreateMutex(NULL, FALSE, NULL);
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

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len))) {
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

    stack_destroy(id_stack);
    close(server_socket);
#ifdef _WIN32
    CloseHandle(stack_mutex);
    WSACleanup();
#endif
    return 0;
}
