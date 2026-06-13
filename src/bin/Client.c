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
        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
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

    while (1) {
        // Read user input
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        // Send message
        if (send(sockfd, buffer, strlen(buffer), 0) == -1)
            die("send");

        // Receive server response
        ssize_t bytes_received =
            recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
            break;

        buffer[bytes_received] = '\0';
        printf("Server: %s\n", buffer);

        // End conversation if "Bye"
        if (strncmp(buffer, "Bye", 3) == 0)
            break;
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
