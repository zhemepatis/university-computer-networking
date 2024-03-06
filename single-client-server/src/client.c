#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT "9002"
#define HOST "::1"
// #define HOST "127.0.0.1"

#define BUFF_LEN 256

int main() {
    struct addrinfo hints;
    struct addrinfo *server_address;

    int client_socket;

    char buff[BUFF_LEN];

    char *file_name;
    int file_size;

    int bytes_read;
    int remaining;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(HOST, PORT, &hints, &server_address) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (struct addrinfo * result = server_address; result != NULL; result = result->ai_next) {
        if ((client_socket = socket(result->ai_family, SOCK_STREAM, 0)) == -1) {
            continue;
        }

        if (connect(client_socket, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }

        close(client_socket);
    }
    freeaddrinfo(server_address);

    if (client_socket == -1) {
        printf("no socket has been created\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter command: ");
    fgets(buff, BUFF_LEN, stdin);
    write(client_socket, buff, strlen(buff));

    // receive file name
    bzero(buff, BUFF_LEN);
    read(client_socket, buff, BUFF_LEN);
    file_name = calloc(strlen(buff) + 1, sizeof(char));
    strcpy(file_name, buff);

    // send feedback
    bzero(buff, BUFF_LEN);
    strcpy(buff, "file name received");
    write(client_socket, buff, strlen(buff));

    // receive file size
    bzero(buff, BUFF_LEN);
    read(client_socket, buff, BUFF_LEN);
    file_size = atoi(buff);

    FILE *fp = fopen(file_name, "wb");
    remaining = file_size;

    bzero(buff, BUFF_LEN);
    while ((remaining > 0) && (bytes_read = read(client_socket, buff, BUFF_LEN)) > 0) {
        fwrite(buff, sizeof(char), bytes_read, fp);
        remaining -= bytes_read;

        printf("Bytes received: %d/%d\n", file_size - remaining, file_size);

        bzero(buff, BUFF_LEN);
    }

    // cleanup
    fclose(fp);
    close(client_socket);

    return 0;
}
