#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 256
#define PORT "9001"
#define HOST "::1"
// #define HOST "127.0.0.1"

int main() {
    struct addrinfo hints;
    struct addrinfo *server_address;

    int server_socket;

    char buff[BUFF_LEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(HOST, PORT, &hints, &server_address) != 0) {
        printf("ERROR: getaddrinfo failed\n");
        exit(1);
    }

    for (struct addrinfo * result = server_address; result != NULL; result = result->ai_next) {
        // create a socket
        if ((server_socket = socket(result->ai_family, SOCK_STREAM, 0)) == -1) {
            continue;
        }

        // connect to server
        if (connect(server_socket, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }

        close(server_socket);
    }
    freeaddrinfo(server_address);

    // send msg to server
    fprintf(stdout, "Enter command: ");
    fgets(buff, BUFF_LEN, stdin);
    send(server_socket, buff, strlen(buff), 0);

    // clean the buff
    memset(&buff, 0, sizeof(BUFF_LEN));

    // receive server msg
    recv(server_socket, buff, BUFF_LEN, 0);
    fprintf(stdout, "Server sent: %s\n", buff);

    // cleanup
    close(server_socket);
    return 0;
}
