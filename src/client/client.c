#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "../include/protocol.h"

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

    // Inicializa logging (arquivos dedicados ao cliente para evitar interleaving entre processos)
    init_logging("client_request.txt", "client_responses.txt");

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
    // Buffer de recepção com acumulação por linha
    char rbuf[65536];
    size_t rlen = 0;

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
            log_request("CLIENT SEND: GET");
            const char *msg = "GET\n";
            if (send(sock, msg, (int)strlen(msg), 0) < 0) { perror("Send failed"); break; }
        } else {
            char cmd[64];
            int len = snprintf(cmd, sizeof(cmd), "GETB %lld\n", (num_ids_to_request - received) < batch ? (num_ids_to_request - received) : batch);
            log_request("CLIENT SEND: %.*s", len - 1, cmd); // log sem o \n
            if (send(sock, cmd, len, 0) < 0) { perror("Send failed"); break; }
        }
        // Ler uma linha completa de resposta (até '\n')
        char *nl = NULL;
        while ((nl = memchr(rbuf, '\n', rlen)) == NULL) {
            int n = recv(sock, rbuf + rlen, (int)sizeof(rbuf) - 1 - (int)rlen, 0);
            if (n <= 0) { printf("[CLIENT] Server disconnected or error.\n"); goto done; }
            rlen += (size_t)n;
            rbuf[rlen] = '\0';
        }

        // Extrai a linha
        size_t linelen = (size_t)(nl - rbuf);
        char line[65536];
        if (linelen >= sizeof(line)) linelen = sizeof(line) - 1;
        memcpy(line, rbuf, linelen);
        line[linelen] = '\0';
        // Avança o buffer remanescente
        size_t rem = rlen - (linelen + 1);
        memmove(rbuf, nl + 1, rem);
        rlen = rem;

        // Normaliza CR
        char *cr = strchr(line, '\r'); if (cr) *cr = '\0';

        // Log da resposta completa
        log_response("CLIENT RECV: %s", line);

        if (strcmp(line, "EMPTY") == 0) { printf("[CLIENT] Server has no more IDs.\n"); break; }

        if (batch == 1) {
            // Resposta unitária já foi consumida (uma linha). Nenhuma ação extra.
            received++;
        } else {
            // Primeiro número é a contagem
            char *p = line;
            long count = strtol(p, &p, 10);
            if (count <= 0) break;
            // Não precisamos consumir os IDs aqui (benchmark), pois a linha já foi integralmente consumida
            received += count;
        }
    }

    double end_time = get_time();

    printf("[CLIENT] Received %lld/%lld IDs.\n", received, num_ids_to_request);
    printf("[CLIENT] Total time taken: %.3f seconds.\n", end_time - start_time);

done:
    close(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    close_logging();
    return 0;
}
