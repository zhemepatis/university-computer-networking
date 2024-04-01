#ifndef SOCKET_H
    #define SOCKET_H
    #define MAX_CLIENT_COUNT 5
    #define BUFF_LEN 256

    struct client {
        int socket;
        char *name;
    };

    struct addrinfo *getSokcetAddrInfo(char *host, char *port, struct addrinfo hints);
    int bindSocket(struct addrinfo *server_address);
    void listenForConn(int server_socket);
    int connToSocket(struct addrinfo *server_address);
    int acceptConn(int server_socket);

#endif