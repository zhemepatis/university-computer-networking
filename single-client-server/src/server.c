#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <limits.h>

#define PORT "9001"
#define FILE_STORAGE_PATH "storage/"
#define BUFF_LEN 256

char * toUpperStr(char str[]);
int parseGet(char *cmd, char **filename);

int main() {
    struct addrinfo hints;
    struct addrinfo *server_address;

    int server_socket;
    int client_socket;

    int client_msg_len;
    int server_msg_len;
    char buff[BUFF_LEN];

    // set hints to get more info about server
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &server_address) != 0) {
        printf("ERROR: getaddrinfo failed\n");
        exit(1);
    }

    for (struct addrinfo * result = server_address; result != NULL; result = result->ai_next) {
        if ((server_socket = socket(result->ai_family, SOCK_STREAM, 0)) == -1) {
            continue;
        }

        int opt = 0;
        if (setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &opt, sizeof(opt)) == -1) {
            printf("ERROR: setsockopt(IPV6_V6ONLY) failed\n");
            exit(1);
        }

        opt = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) == -1) {
            printf("ERROR: setsockopt(SO_REUSEADDR) failed\n");
            break;
        }

        // bind socket
        if (bind(server_socket, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }

        close(server_socket);
    }
    freeaddrinfo(server_address);

    // listen for clients
    if (listen(server_socket, 5) == -1) {
        printf("ERROR: could not listen socket\n");
        close(client_socket);
        exit(1);
    }
    printf("Server is listening...\n");

    for (;;) {
        memset(&buff, 0, sizeof(buff));

        // accept client connection
        if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            printf("ERROR: error while accepting connection\n");
            exit(1);
        }

        client_msg_len = recv(client_socket, buff, sizeof(buff), 0);
        printf("Received command: %s\n", buff);

        char *file_name;
        if(parseGet(buff, &file_name) != 0) {   // TODO: stop flow after error
            char *server_response = "unknown command\n";
            send(client_socket, server_response, sizeof(server_response), 0);
        }

        int file_path_len = strlen(FILE_STORAGE_PATH) + strlen(file_name);
        char *file_path = calloc(file_path_len + 1, sizeof(char));
        sprintf(file_path, "%s%s", FILE_STORAGE_PATH, file_name);
        
        // TODO: stop flow after error
        FILE *fp;
        if ((fp = fopen(file_path, "rb")) == NULL) {
            perror("fopen");
        }

        // get size of the file
        int file_size;
        char server_response[50];

        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        sprintf(server_response, "%d\n", file_size);

        send(client_socket, server_response, sizeof(server_response), 0);

        // read and send file
        fseek(fp, 0, SEEK_SET);
        int bytes_read;
        while ((bytes_read = fread(buff, sizeof(char), sizeof(buff), fp)) > 0) {
            send(client_socket, buff, bytes_read, 0);
        }

        // send(client_socket, server_response, sizeof(server_response), 0);


        // cleanup
        close(client_socket);
        free(file_name);
        free(file_path);
    }  

    // cleanup
    close(server_socket);

    return 0;
}

char * toUpperStr(char str[]) {
    int str_len = strlen(str);
    char * result = calloc(str_len + 1, sizeof(char));

    for(int i = 0; i < str_len; ++i) {
        result[i] = toupper(str[i]);
    }

    return result;
}

int parseGet(char *cmd, char **filename) {
    regex_t pattern;
    if (regcomp(&pattern, "get[[:space:]]+([^ \t\n]+)", REG_EXTENDED) != 0) {
        return 1;
    }

    regmatch_t matches[2];
    if (regexec(&pattern, cmd, 2, matches, 0) != 0) {
        regfree(&pattern);
        return 1;
    }

    if (matches[1].rm_so == -1) {
        regfree(&pattern);
        return 1;
    }

    int len = matches[1].rm_eo - matches[1].rm_so;
    *filename = calloc(len + 1, sizeof(char));
    if (*filename == NULL) {
        regfree(&pattern);
        return 1;
    }

    strncpy(*filename, cmd + matches[1].rm_so, len);

    // clean up
    regfree(&pattern);

    return 0;
}