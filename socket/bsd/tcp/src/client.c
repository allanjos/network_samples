#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>

#include "network.h"
#include "client.h"

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5556
#define SOCKET_BUFFER_SIZE 1024

char socket_buffer[SOCKET_BUFFER_SIZE];

int socket_client = -1;
struct sockaddr_in server_address;

void app_term()
{
    if (socket_client != -1) {
        close(socket_client);
    }
}

int tcp_client_start(void)
{
    int rv;

    socket_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_client == -1) {
        fprintf(stderr, "Error on client socket creation.\n");

        return -1;
    }

    printf("Socket to connect to the server is created.\n");

    server_address.sin_family = PF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    rv = connect(socket_client, (struct sockaddr *) &server_address, sizeof(server_address));

    if (rv < 0) {
        fprintf(stderr, "Error on client socket creation.\n");

        return -1;
    }

    printf("Connected to the server.\n");

    // Send message to the server

    time_t time_current;
    struct tm *gmtime_current = NULL;

    if ((time_current = time(NULL)) < 0) {
        fprintf(stderr, "Invalid current time.\n");

        perror("Error on trying to get the current time.\n");

        app_term();

        return 6;
    }

    gmtime_current = gmtime(&time_current);

    sprintf(socket_buffer,
            "Message from client %.4i-%.2i-%.2iT%.2i:%.2i:%.2iZ\n",
            gmtime_current->tm_year + 1900,
            gmtime_current->tm_mon + 1,
            gmtime_current->tm_mday,
            gmtime_current->tm_hour,
            gmtime_current->tm_min,
            gmtime_current->tm_sec);

    printf("String to send (%ld bytes): %s\n", strlen(socket_buffer), socket_buffer);

    rv = send(socket_client, socket_buffer, strlen(socket_buffer), 0);

    if (rv < 0) {
        fprintf(stderr, "Error on sending packet to server.\n");
    }
    else {
        printf("Packet sent to the server successfully.\n");
    }

    // Read responses from the server

    rv = recv(socket_client, socket_buffer, sizeof(socket_buffer), 0);

    if (rv < 0) {
        fprintf(stderr, "Invalid server message.\n");

        perror("Error message:");

        fprintf(stderr, "Internal error: %s\n", get_socket_error_message(rv));
    }
    else if (rv == 0) {
        fprintf(stderr, "No bytes received from server.\n");
    }
    else {
        printf("Message received from server: %s\n", socket_buffer);
    }

    app_term();

    return 0;
}

void sigint_handler(int sig)
{
    printf("sigint_handler\n");

    app_term();
}

int main() {
    printf("Initializing...\n");

    signal(SIGINT, sigint_handler);

    tcp_client_start();

    return 0;
}

