
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// Convert string to uppercase
void to_upper(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Convert string to lowercase
void to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Reverse the string
void reverse(char *str) {
    int i = 0;
    int j = strlen(str) - 1;

    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connection
    new_socket = accept(server_fd,
        (struct sockaddr *)&address,
        (socklen_t *)&addrlen);

    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(new_socket, buffer,
                           BUFFER_SIZE - 1);

        if (valread <= 0)
            break;

        buffer[valread] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check termination
        if (strcmp(buffer, "bye") == 0)
            break;

        char command[10] = {0};
        char text[BUFFER_SIZE] = {0};

        // Parse input
        if (sscanf(buffer, " %9[^|]|%1023[^\n]",
                   command, text) != 2) {
            strcpy(text, "Invalid Request");
        }
        else {
            // Remove spaces
            command[strcspn(command, " ")] = '\0';

            if (strcmp(command, "UP") == 0) {
                to_upper(text);
            }
            else if (strcmp(command, "LOW") == 0) {
                to_lower(text);
            }
            else if (strcmp(command, "REV") == 0) {
                reverse(text);
            }
            else {
                strcpy(text, "Invalid Command");
            }
        }

        send(new_socket, text, strlen(text), 0);
    }

    close(new_socket);
    close(server_fd);

    printf("Server terminated.\n");
    return 0;
}
