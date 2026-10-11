// Roll: 25157142906 Service Type: A d = 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 2048

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len;
    char input[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    if (argc != 3) {
        printf("Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(30906);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        printf("Invalid server IP address\n");
        close(sockfd);
        return 1;
    }

    server_len = sizeof(server_addr);

    printf("Connected to UDP server at %s:%d\n", argv[1], 30906);
    printf("Enter UP|text, LOW|text, REV|text or EXIT\n");

    while (1) {
        printf("\nEnter request: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            strcpy(input, "EXIT");
        }

        input[strcspn(input, "\n")] = '\0';

        /* Add roll number prefix */
        int written = snprintf(message, sizeof(message),
                               "%s#%s", argv[2], input);

        if (written < 0 || written >= (int)sizeof(message)) {
            printf("Request is too long.\n");
            continue;
        }

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr,
                   server_len) < 0) {
            perror("sendto failed");
            break;
        }

        /* EXIT is sent, then the client terminates */
        if (strcmp(input, "EXIT") == 0) {
            printf("Exit command sent. Client terminated.\n");
            break;
        }

        memset(response, 0, sizeof(response));

        int received = recvfrom(sockfd, response,
                                sizeof(response) - 1, 0,
                                (struct sockaddr *)&server_addr,
                                &server_len);

        if (received < 0) {
            perror("recvfrom failed");
            break;
        }

        response[received] = '\0';

        printf("Server response: %s\n", response);
    }

    close(sockfd);

    return 0;
}
