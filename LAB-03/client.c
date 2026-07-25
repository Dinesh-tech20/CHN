
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Server IP
    if (inet_pton(AF_INET, "127.0.0.1",
                  &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    // Connect
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Connected to server.\n");

    while (1) {
        printf("\nEnter command (UP | text, LOW | text, REV | text) or bye:\n");

        if (fgets(input, BUFFER_SIZE, stdin) == NULL)
            break;

        send(sock, input, strlen(input), 0);

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "bye") == 0)
            break;

        memset(buffer, 0, BUFFER_SIZE);

        int n = read(sock, buffer, BUFFER_SIZE - 1);

        if (n <= 0)
            break;

        buffer[n] = '\0';

        printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
