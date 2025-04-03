#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_socket);
        exit(1);
    }

    printf("Connected to the server!\n");
    printf("Guess a number between 1 and 100.\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Enter your guess: ");
        if (!fgets(buffer, BUFFER_SIZE, stdin)) {
            perror("Input error");
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Server closed the connection.\n");
            } else {
                perror("Receive failed");
            }
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);

        if (strcmp(buffer, "correct") == 0) {
            printf("Congratulations! You guessed the number!\n");
            break;
        }
    }

    close(client_socket);
    return 0;
}
