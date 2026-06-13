#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BACKLOG 5

static void die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

static void send_message(int client_fd, const char *message)
{
    if (send(client_fd, message, strlen(message), 0) == -1)
        die("send");
}

static int receive_int(int client_fd)
{
    int value;

    ssize_t bytes_received =
        recv(client_fd, &value, sizeof(value), 0);

    if (bytes_received <= 0)
        die("recv");

    return value;
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

    // Calculator prompts
    static const char *prompt_num1 =
        "Enter Number 1";

    static const char *prompt_num2 =
        "Enter Number 2";

    static const char *prompt_menu =
        "Enter your choice:\n"
        "1. Add\n"
        "2. Subtract\n"
        "3. Multiply\n"
        "4. Divide\n"
        "5. Exit";

    while (1) {
        // Request operation
        send_message(client_fd, prompt_menu);
        int choice = receive_int(client_fd);
        printf("Choice: %d\n", choice);

        // Exit calculator
        if (choice == 5)
            break;

        // Request first number
        send_message(client_fd, prompt_num1);
        int num1 = receive_int(client_fd);
        printf("Number1: %d\n", num1);

        // Request second number
        send_message(client_fd, prompt_num2);
        int num2 = receive_int(client_fd);
        printf("Number2: %d\n", num2);

        // Perform calculation
        int result = 0;

        switch (choice) {
            case 1:
                result = num1 + num2;
                break;

            case 2:
                result = num1 - num2;
                break;

            case 3:
                result = num1 * num2;
                break;

            case 4:
                result = (num2 == 0)
                    ? -1
                    : num1 / num2;
                break;

            default:
                result = -1;
                break;
        }

        // Send result to client
        if (send(client_fd,
                 &result,
                 sizeof(result),
                 0) == -1)
        {
            die("send result");
        }
    }

    close(client_fd);
    close(server_fd);

    return EXIT_SUCCESS;
}
