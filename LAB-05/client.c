
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {

    int sock;

    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Initialize server address
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Server IP address
    if (inet_pton(AF_INET, "127.0.0.1",
                  &serv_addr.sin_addr) <= 0) {

        printf("Invalid address\n");
        close(sock);
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {

        perror("Connection failed");
        close(sock);
        return -1;
    }

    printf("Connected to server.\n");

    // Continuous communication
    while (1) {

        printf("\nEnter request (ANALYZE | TEXT) or bye: ");

        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        // Send request
        if (send(sock, input, strlen(input), 0) < 0) {

            perror("Send failed");
            break;
        }

        // Exit condition
        if (strcmp(input, "bye") == 0) {

            printf("Connection terminated.\n");
            break;
        }

        // Receive response
        memset(buffer, 0, BUFFER_SIZE);

        int n = read(sock, buffer, BUFFER_SIZE - 1);

        if (n <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[n] = '\0';

        printf("Server Response: %s\n", buffer);
    }

    // Close socket
    close(sock);

    return 0;
}
