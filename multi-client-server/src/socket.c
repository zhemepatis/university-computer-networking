#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/socket.h>
#include <netdb.h>
#include "socket.h"

struct addrinfo *getSokcetAddrInfo(char *host, char *port, struct addrinfo hints) {
	struct addrinfo *server_address;

	if (getaddrinfo(host, port, &hints, &server_address) != 0) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	return server_address;
}

int bindSocket(struct addrinfo *server_address) {
	int server_socket;

	for (struct addrinfo *result = server_address; result != NULL; result = result->ai_next) {
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

	return server_socket;
}

void listenForConn(int server_socket) {
    if (listen(server_socket, 5) == -1) {
		perror("listen");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	printf("Server is listening...\n");
}

int acceptConn(int server_socket) {
	int client_socket;

	if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
		perror("accept");
	}

	return client_socket;
}

int connToSocket(struct addrinfo *server_address) {
	int server_socket;

	for (struct addrinfo *result = server_address; result != NULL; result = result->ai_next) {
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
        printf("no socket has been created\n");
        exit(EXIT_FAILURE);
    }

	return server_socket;
}
