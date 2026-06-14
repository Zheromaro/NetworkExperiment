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
        fprintf(stderr, "Usage: %s <server_ipaddress> <port>\n",
                argv[0]);
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

    // Open source file
    FILE *file = fopen("assets/file.txt", "r");
    if (file == NULL)
        die("fopen");

    int word_count = 0;
    char buffer[BUFFER_SIZE];

    // Count words
    while (fscanf(file, "%255s", buffer) == 1)
        ++word_count;

    // Send word count
    if (send(sockfd,
             &word_count,
             sizeof(word_count),
             0) == -1)
    {
        die("send word count");
    }

    rewind(file);

    // Send file contents
    while (fscanf(file, "%255s", buffer) == 1) {
        if (send(sockfd,
                 buffer,
                 strlen(buffer) + 1,
                 0) == -1)
        {
            die("send data");
        }
    }

    printf("File sent to DataServer.\n");

    fclose(file);
    close(sockfd);

    return EXIT_SUCCESS;
}
