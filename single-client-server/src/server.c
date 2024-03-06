#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define PORT "9002"

#define FILE_STORAGE_PATH "storage/"
#define BUFF_LEN 256

int parseGet(char *cmd, char **filename);

int main() {
    struct addrinfo hints;
    struct addrinfo *server_address;

    int server_socket;
    int client_socket;

    int server_msg_len;
    char buff[BUFF_LEN];

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &server_address) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (struct addrinfo * result = server_address; result != NULL; result = result->ai_next) {
        if ((server_socket = socket(result->ai_family, SOCK_STREAM, 0)) == -1) {
            continue;
        }

        int opt = 0;
        if (setsockopt(server_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &opt, sizeof(opt)) == -1) {
            perror("setsockopt(IPV6_V6ONLY)");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        opt = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) == -1) {
            perror("setsockopt(SO_REUSEADDR)");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        if (bind(server_socket, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }

        close(server_socket);
    }
    freeaddrinfo(server_address);

    if (server_socket == -1) {
        printf("no socket has been created\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening...\n");

    for (;;) {
        char *file_name;
        char *file_path;
        int file_path_len;

        FILE *fp;
        int file_size;
        char file_size_str[256];

        int remaining;
        int bytes_read;

        if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            perror("accept");
            continue;
        }

        // get command
        bzero(buff, BUFF_LEN);

        read(client_socket, buff, BUFF_LEN);
        printf("Received command: %s\n", buff);

        // parse command
        if (parseGet(buff, &file_name) != 0) {
            printf("unknown command\n");
            continue;
        }

        bzero(buff, BUFF_LEN);

        file_path_len = strlen(FILE_STORAGE_PATH) + strlen(file_name);
        file_path = calloc(file_path_len + 1, sizeof(char));
        sprintf(file_path, "%s%s", FILE_STORAGE_PATH, file_name);

        // open file
        fp = fopen(file_path, "rb");

        // get file size
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        sprintf(file_size_str, "%d", file_size);
        
        // get back to the file beginning
        fseek(fp, 0, SEEK_SET);

        // send file name and size
        write(client_socket, file_name, strlen(file_name) + 1);
        read(client_socket, buff, BUFF_LEN);
        write(client_socket, file_size_str, strlen(file_size_str) + 1);

        // send the flie
        printf("Sending file...\n");
        remaining = file_size;
        bzero(buff, BUFF_LEN);
        FILE *fuck = fopen("acne.pdf", "wb");
        while((bytes_read = fread(buff, 1, BUFF_LEN, fp)) > 0) {
            write(client_socket, buff, strlen(buff));
            fwrite(buff, bytes_read, sizeof(char), fuck);
            remaining -= bytes_read;

            printf("Bytes sent: %d/%d\n", file_size - remaining, file_size);

            bzero(buff, BUFF_LEN);
        }
        printf("File has been sent.\n");

        // cleanup
        fclose(fp);
        fclose(fuck);
        close(client_socket);
    }  

    // cleanup
    close(server_socket);

    return 0;
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