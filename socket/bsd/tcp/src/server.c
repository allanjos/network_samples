#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "network.h"
#include "server.h"

#define BACKLOG 4
#define SERVER_PORT 5556

int socket_fd = 0;
int client_connection = 0;
socklen_t addr_in_len;
struct sockaddr_in addr_in;
time_t time_current;
struct tm *gmtime_current = NULL;
FILE *socket_file_fd = NULL;
char buffer_client_msg[10 * 1024] = {'\0'};

void app_term()
{
    if (client_connection) {
        printf("Closing connection with the client..\n");

        close(client_connection);

        client_connection = 0;
    }

    if (socket_fd) {
        printf("Closing socket..\n");

        close(socket_fd);

        socket_fd = 0;
    }
}

void sigint_handler(int sig)
{
    printf("sigint_handler\n");

    app_term();
}

int server_start()
{
    int rv = 0;

    printf("Starting server...\n");

    socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_fd < 0) {
        perror("socket\n");
        return 1;
    }

    bzero(&addr_in, sizeof(addr_in));

    addr_in.sin_family = PF_INET;
    addr_in.sin_port = htons(SERVER_PORT);
    addr_in.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr *) &addr_in, sizeof addr_in) < 0) {
        perror("bind\n");

        return 2;
    }

    listen(socket_fd, BACKLOG);

    while (1) {
        addr_in_len = sizeof(addr_in);

        client_connection = accept(socket_fd, (struct sockaddr *) &addr_in, &addr_in_len);

        if (client_connection < 0) {
            fprintf(stderr, "socket accept error\n");

            app_term();

            return 4;
        }

        printf("Client is connected.\n");

        // Read message from the socket

        rv = recv(client_connection, buffer_client_msg, sizeof(buffer_client_msg), 0);

        if (rv < 0) {
            fprintf(stderr, "Invalid socket message.\n");

            perror("Error message:");

            fprintf(stderr, "Internal error: %s\n", get_socket_error_message(rv));

            break;
        }
        else if (rv == 0) {
            fprintf(stderr, "No bytes received from client.\n");

            break;
        }
        else {
            printf("Message received from client: %s\n", buffer_client_msg);
        }

        // Send message to client

        printf("Send response to the client...\n");

        time_current = time(NULL);

        printf("UTC:   %s", asctime(gmtime(&time_current)));
        printf("local: %s", asctime(localtime(&time_current)));

        if (time_current < 0) {
            fprintf(stderr, "Invalid current time.\n");

            perror("Error on trying to get the current time.\n");

            app_term();

            return 6;
        }

        gmtime_current = gmtime(&time_current);

        sprintf(buffer_client_msg,
                "%.4i-%.2i-%.2iT%.2i:%.2i:%.2iZ\n",
                gmtime_current->tm_year + 1900,
                gmtime_current->tm_mon + 1,
                gmtime_current->tm_mday,
                gmtime_current->tm_hour,
                gmtime_current->tm_min,
                gmtime_current->tm_sec);

        printf("Message to send to client: %s\n", buffer_client_msg);

        send(client_connection, buffer_client_msg, strlen(buffer_client_msg), 0);
    }

    app_term();

    return 0;
}

int main() {
    printf("Initializing...\n");

    signal(SIGINT, sigint_handler);

    server_start();

    return 0;
}
