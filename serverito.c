#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5

void log_message(const char* client_ip, const char* message) {
    printf("%s: %s\n", client_ip, message);
}

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    log_message(client_ip, "connected");

     srand(time(NULL) + getpid());
    int number = rand() % 100 + 1;
    char buffer[BUFFER_SIZE];

    while(1) {
        int received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if(received <= 0) break;
        buffer[received] = '\0';

        int guess = atoi(buffer);
        char response[BUFFER_SIZE];

        if(guess < number) {
            strcpy(response, "higher");
            log_message(client_ip, buffer);
        } else if(guess > number) {
            strcpy(response, "lower");
            log_message(client_ip, buffer);
        } else {
            strcpy(response, "correct");
            log_message(client_ip, "correct");
        }

        send(client_sock, response, strlen(response), 0);

        if(strcmp(response, "correct") == 0) break;
    }

    log_message(client_ip, "disconnected");
    close(client_sock);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    if(listen(server_sock, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port %s\n", argv[1]);

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        if(client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        if(fork() == 0) {
            close(server_sock);
            handle_client(client_sock, client_addr);
            exit(0);
        }
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
