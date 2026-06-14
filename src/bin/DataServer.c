#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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

    // Create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        die("socket");

    // Configure server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind socket to port
    if (bind(server_fd,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1)
    {
        die("bind");
    }

    // Start listening for clients
    if (listen(server_fd, BACKLOG) == -1)
        die("listen");

    // Accept incoming connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &client_len);

    if (client_fd == -1)
        die("accept");

    // Open destination file
    FILE *file = fopen("assets/file_received.txt", "a");
    if (file == NULL)
        die("fopen");

    int word_count;

    // Receive number of words
    if (recv(client_fd, &word_count, sizeof(word_count), 0) <= 0)
        die("recv word count");

    char buffer[BUFFER_SIZE];

    // Receive file contents
    for (int i = 0; i < word_count; ++i) {
        ssize_t bytes_received =
            recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0)
            die("recv data");

        buffer[bytes_received] = '\0';

        fprintf(file, "%s ", buffer);
    }

    printf("File received from Downloader.\n");

    fclose(file);
    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
