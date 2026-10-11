// Roll: 25157142906 Service Type: A d = 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ROLL "25157142906"
#define PORT 30906
#define BUFFER_SIZE 2048

void process_request(char *message, char *response)
{
    char *separator;
    char *command;
    char *text;
    int i;
    size_t len;

    separator = strchr(message, '|');

    if (separator == NULL) {
        snprintf(response, BUFFER_SIZE, "ERROR: Invalid request");
        return;
    }

    *separator = '\0';
    command = message;
    text = separator + 1;

    if (strcmp(command, "UP") == 0) {
        for (i = 0; text[i] != '\0'; i++) {
            text[i] = (char)toupper((unsigned char)text[i]);
        }
    }
    else if (strcmp(command, "LOW") == 0) {
        for (i = 0; text[i] != '\0'; i++) {
            text[i] = (char)tolower((unsigned char)text[i]);
        }
    }
    else if (strcmp(command, "REV") == 0) {
        len = strlen(text);

        for (i = 0; i < (int)(len / 2); i++) {
            char temp = text[i];
            text[i] = text[len - 1 - i];
            text[len - 1 - i] = temp;
        }
    }
    else {
        snprintf(response, BUFFER_SIZE, "ERROR: Invalid command");
        return;
    }

    snprintf(response, BUFFER_SIZE, "%s", text);
}

int main(void)
{
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    printf("Server running on port %d\n", PORT);
    printf("Service Type: A - Text Transformer\n");

    while (1) {
        client_len = sizeof(client_addr);

        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));

        int received = recvfrom(server_fd, buffer, sizeof(buffer) - 1, 0,
                                (struct sockaddr *)&client_addr,
                                &client_len);

        if (received < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[received] = '\0';

        inet_ntop(AF_INET, &client_addr.sin_addr,
                  client_ip, sizeof(client_ip));

        printf("\nClient: %s:%d\n",
               client_ip, ntohs(client_addr.sin_port));

        /* Check the roll-number prefix */
        size_t roll_len = strlen(ROLL);

        if (strncmp(buffer, ROLL, roll_len) != 0 ||
            buffer[roll_len] != '#') {
            snprintf(response, sizeof(response),
                     "ERROR: Unauthorized client");

            printf("Received message: %s\n", buffer);
            printf("Sent response: %s\n", response);

            sendto(server_fd, response, strlen(response), 0,
                   (struct sockaddr *)&client_addr, client_len);
            continue;
        }

        /* Remove the roll-number prefix */
        char *message = buffer + roll_len + 1;

        printf("Received message: %s\n", message);

        /* Exit command */
        if (strcmp(message, "EXIT") == 0) {
            printf("Exit command received. Shutting down server.\n");
            break;
        }

        process_request(message, response);

        printf("Sent response: %s\n", response);

        sendto(server_fd, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, client_len);
    }

    close(server_fd);
    printf("Server closed successfully.\n");

    return 0;
}
