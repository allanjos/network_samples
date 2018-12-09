#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

#include "network.h"

int socket_client;
char *server_address;
int server_port;
char socket_buffer[SOCKET_BUFFER_SIZE];
char *message;

void app_term()
{
    fprintf(stderr, "Terminating.\n");

    if (socket_client != -1) {
        close(socket_client);
    }
}

int udp_client_start()
{
    struct sockaddr_in server_socket_addr;

    printf("Server address: %s\n", server_address);
    printf("Server port: %d\n", server_port);
 
    // address family
    server_socket_addr.sin_family = AF_INET;
    // destination port
    server_socket_addr.sin_port = htons(server_port);
    // destination IP address
    server_socket_addr.sin_addr.s_addr = inet_addr(server_address);
 
    socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
 
    if (socket_client < 0) {
       fprintf(stderr, "Error on socket creation.\n");

       app_term();

       return 1;
    }

    printf("Socket created.\n");

    int rv;
 
    printf("Sending packets...\n");

    strcpy(socket_buffer, message);

    rv = sendto(socket_client,
                socket_buffer,
                SOCKET_BUFFER_SIZE,
                0,
                (const struct sockaddr *) &server_socket_addr,
                sizeof(server_socket_addr));

    if (rv < 0) {
        fprintf(stderr, "Error on trying to send packet.\n");

        app_term();

        return 1;
    }

    app_term();
 
    return 0;
}

void sigint_handler(int sig)
{
    printf("sigint_handler: signal %d\n", sig);

    app_term();
}

int main(int argc, char *argv[])
{
    printf("UDP client\n\n");

    if (argc < 4) {
        printf("Usage:\n%s <SERVER ADDRESS> <SERVER PORT> <MESSAGE>\n\n", argv[0]);

        return 0;
    }

    server_address = argv[1];
    server_port = atoi(argv[2]);
    message = argv[3];

    signal(SIGINT, sigint_handler);

    udp_client_start();

    return 0;
}
