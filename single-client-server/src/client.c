#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 256
#define PORT "9002"

#define SUCCESS '0'
#define ERROR '1'

#define HOST "::1"
// #define HOST "127.0.0.1"

char * receive(int sock);
char *omitFirstChar(char *str);

int main() {
    struct addrinfo hints;
    struct addrinfo *server_address;

    int server_socket;

    char buff[BUFF_LEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(HOST, PORT, &hints, &server_address) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (struct addrinfo * result = server_address; result != NULL; result = result->ai_next) {
        if ((server_socket = socket(result->ai_family, SOCK_STREAM, 0)) == -1) {
            continue;
        }

        if (connect(server_socket, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }

        close(server_socket);
    }
    freeaddrinfo(server_address);

    if (server_socket == -1) {
        fprintf(stderr, "no socket has been created\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Enter command: ");
    fgets(buff, BUFF_LEN, stdin);
    send(server_socket, buff, strlen(buff), 0);

    // receive file name
    memset(&buff, 0, sizeof(buff));
    recv(server_socket, buff, sizeof(buff), 0);
    char *file_name = calloc(strlen(buff) + 1, sizeof(char));
    strcpy(file_name, buff);

    // send feedback
    memset(&buff, 0, sizeof(buff));
    strcpy(buff, "file name received");
    send(server_socket, buff, sizeof(buff), 0);

    // receive file size
    memset(&buff, 0, sizeof(buff));
    int res = recv(server_socket, buff, sizeof(buff), 0);
    int file_size = atoi(buff);
    printf("buff: %s\n", buff);

    FILE *fp = fopen(file_name, "wb");
    int remain_data = file_size;
    int len;

    printf("REMAIN: %d\n", remain_data);

    memset(&buff, 0, sizeof(buff));
    while ((remain_data > 0) && (len = recv(server_socket, buff, BUFF_LEN, 0)) > 0) {
        printf("lne: %d\n", len);

        fwrite(buff, sizeof(char), len, fp);
        remain_data -= len;

        memset(&buff, 0, sizeof(buff));
    }

    // cleanup
    fclose(fp);
    close(server_socket);

    return 0;
}
