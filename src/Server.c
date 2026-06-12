#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define BACKLOG     5

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        die("socket");

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        die("bind");
    }

    if (listen(server_fd, BACKLOG) == -1)
        die("listen");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &client_len);

    if (client_fd == -1)
        die("accept");

    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes_received =
            recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
            break;

        buffer[bytes_received] = '\0';

        printf("Client: %s\n", buffer);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        if (send(client_fd, buffer, strlen(buffer), 0) == -1)
            die("send");

        if (strncmp(buffer, "Bye", 3) == 0)
            break;
    }

    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
