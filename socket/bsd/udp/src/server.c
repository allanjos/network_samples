#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define TM_BUF_SIZE 1500

char socket_buffer[TM_BUF_SIZE];
int socket_server;
int stop_requested = 0;
int server_port;

void app_term()
{
    if (socket_server != -1) {
        close(socket_server);
    }
}

int udp_server_start(void)
{
    struct sockaddr_in source_sock_address;
    struct sockaddr_in server_sock_address;
    int rv;
    unsigned int address_length;

    printf("Server port: %d\n", server_port);

    server_sock_address.sin_family = AF_INET;
    server_sock_address.sin_port = htons(server_port);
    server_sock_address.sin_addr.s_addr = 0;

    socket_server = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_server < 0) {
        printf("Error on socket creation.\n");

        goto udpServerEnd;
    }

    rv = bind(socket_server, (const struct sockaddr *) &server_sock_address, sizeof(server_sock_address));

    if (rv < 0) {
        fprintf(stderr, "Error on trying to bind the port and address.\n");

        goto udpServerEnd;
    }

    while (!stop_requested) {
        printf("Waiting for packets...\n");

        address_length = sizeof(source_sock_address);

        rv = recvfrom(socket_server,
                             socket_buffer,
                             TM_BUF_SIZE,
                             0,
                             (struct sockaddr *) &source_sock_address,
                             &address_length);

        if (rv < 0) {
            fprintf(stderr, "Error on receiving packet.\n");
            break;
        }

        printf("Received data: %s\n", socket_buffer);
    }

    udpServerEnd:

    app_term();

    return 0;
}

void sigint_handler(int sig)
{
    printf("sigint_handler: signal %d\n", sig);

    stop_requested = 1;

    sleep(1);

    exit(1);
}

int main(int argc, char *argv[])
{
    printf("UDP server\n\n");

    if (argc < 2) {
        printf("Usage:\n%s <SERVER PORT>\n\n", argv[0]);

        return 0;
    }

    server_port = atoi(argv[1]);

    signal(SIGINT, sigint_handler);

    udp_server_start();

    return 0;
}
