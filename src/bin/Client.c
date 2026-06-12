#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 256
#define BACKLOG     5

static void error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int sockfd, protno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ipaddress> <port_num>\n", argv[0]);
        return EXIT_FAILURE;
    }

    protno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL)
        fprintf(stderr, "ERROR no sush host");

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(protno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("connection failed");

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR on writing");

        bzero(buffer, BUFFER_SIZE);
        n = read(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR on reading");

        printf("Server: %s\n", buffer);

        if (strncmp(buffer, "Bye", 3) == 0)
            break;
    }

    close(sockfd);
    return 0;
}
