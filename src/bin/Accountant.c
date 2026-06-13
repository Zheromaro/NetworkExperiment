#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 256

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ipaddress> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const int port = atoi(argv[2]);

    // Create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        die("socket");

    // Resolve hostname
    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Unknown host: %s\n", argv[1]);
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Configure server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    memcpy(
        &server_addr.sin_addr.s_addr,
        server->h_addr,
        server->h_length
    );

    // Connect to server
    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1)
    {
        die("connect");
    }

    char buffer[BUFFER_SIZE];
    int num1;
    int num2;
    int choice;
    int result;

    while (1) {
        // Receive first prompt
        ssize_t bytes_received =
            recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
            break;

        buffer[bytes_received] = '\0';

        printf("Server: %s\n", buffer);

        // Send first number
        scanf("%d", &num1);

        if (send(sockfd, &num1, sizeof(num1), 0) == -1)
            die("send num1");

        // Receive second prompt
        bytes_received =
            recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
            break;

        buffer[bytes_received] = '\0';

        printf("Server: %s\n", buffer);

        // Send second number
        scanf("%d", &num2);

        if (send(sockfd, &num2, sizeof(num2), 0) == -1)
            die("send num2");

        // Receive menu
        bytes_received =
            recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
            break;

        buffer[bytes_received] = '\0';

        printf("Server: %s\n", buffer);

        // Send operation choice
        scanf("%d", &choice);

        if (send(sockfd, &choice, sizeof(choice), 0) == -1)
            die("send choice");

        // Exit calculator
        if (choice == 5)
            break;

        // Receive calculation result
        ssize_t result_bytes =
            recv(sockfd, &result, sizeof(result), 0);

        if (result_bytes <= 0)
            break;

        printf("Server: The result is %d\n", result);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
