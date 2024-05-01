#ifndef SOCKET_H
    #define SOCKET_H

    struct addrinfo *getSocketAddrInfo(char *host, char *port, struct addrinfo hints);
    int bindSocket(struct addrinfo *server_address);
    void listenForConn(int server_socket);
    int connToSocket(struct addrinfo *server_address);
    int acceptConn(int server_socket);

#endif