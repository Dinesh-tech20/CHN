
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082
#define BUFFER_SIZE 4096

int main() {

    int sock;
    struct sockaddr_in serv_addr;

    char id[50];
    char file_data[BUFFER_SIZE];
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Open local data file
    FILE *fp = fopen("data.txt", "r");

    if (fp == NULL) {
        perror("File opening failed");
        return 1;
    }

    // Read file contents
    size_t size = fread(file_data, 1,
                        BUFFER_SIZE - 1, fp);

    file_data[size] = '\0';

    fclose(fp);

    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
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
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {

        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to server.\n");

    // Enter Student ID
    printf("Enter Student ID to search: ");

    if (fgets(id, sizeof(id), stdin) == NULL) {
        close(sock);
        return 1;
    }

    id[strcspn(id, "\n")] = '\0';

    // Prepare request
    int len = snprintf(request, sizeof(request),
                       "%s\n%s", id, file_data);

    if (len < 0 || len >= BUFFER_SIZE) {
        printf("Request too large.\n");
        close(sock);
        return 1;
    }

    // Send ID and file data
    if (write(sock, request, len) < 0) {

        perror("Send failed");
        close(sock);
        return 1;
    }

    // Receive server response
    memset(response, 0, BUFFER_SIZE);

    int n = read(sock, response, BUFFER_SIZE - 1);

    if (n <= 0) {

        printf("Server disconnected.\n");
        close(sock);
        return 1;
    }

    response[n] = '\0';

    printf("Server Response: %s\n", response);

    // Close socket
    close(sock);

    return 0;
}
