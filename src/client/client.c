#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "protocol.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define close closesocket
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
#endif

    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s <ip_address> <port> <num_ids> [batch]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);
    long long num_ids_to_request = atoll(argv[3]);
    long long batch = 1;
    if (argc == 5) {
        batch = atoll(argv[4]);
        if (batch <= 0) batch = 1;
    }

    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected to server. Requesting %lld IDs (batch=%lld)...\n", num_ids_to_request, batch);

    double start_time = get_time();

    long long received = 0;
    while (received < num_ids_to_request) {
        if (batch == 1) {
            if (send(sock, "GET", 3, 0) < 0) { perror("Send failed"); break; }
        } else {
            char cmd[64];
            int len = snprintf(cmd, sizeof(cmd), "GETB %lld", (num_ids_to_request - received) < batch ? (num_ids_to_request - received) : batch);
            if (send(sock, cmd, len, 0) < 0) { perror("Send failed"); break; }
        }
        int read_size = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (read_size <= 0) { printf("[CLIENT] Server disconnected or error.\n"); break; }
        buffer[read_size] = '\0';
        // Normalizar linha
        char *cr = strchr(buffer, '\r'); if (cr) *cr='\0';
        char *lf = strchr(buffer, '\n'); if (lf) *lf='\0';

        if (strcmp(buffer, "EMPTY") == 0) { printf("[CLIENT] Server has no more IDs.\n"); break; }

        if (batch == 1) {
            received++;
        } else {
            // Primeiro número é a contagem
            char *p = buffer;
            long count = strtol(p, &p, 10);
            if (count <= 0) break;
            // Consumir ids (não armazenamos)
            for (long i = 0; i < count; ++i) {
                long long idv = strtoll(p, &p, 10);
                (void)idv;
            }
            received += count;
        }
    }

    double end_time = get_time();

    printf("[CLIENT] Received %lld/%lld IDs.\n", received, num_ids_to_request);
    printf("[CLIENT] Total time taken: %.3f seconds.\n", end_time - start_time);

    close(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
