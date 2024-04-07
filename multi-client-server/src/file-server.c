#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include "socket.h"
#include "files.h"
#include "parsing.h"
#include "file-commands.h"

#define PORT "9002"

int initFileServer();
void runServer(int server_socket);
void *handleConn(void *client_socket_ptr);

int main() 
{ 
	int server_socket;

	server_socket = initFileServer();
	listenForConn(server_socket);
	runServer(server_socket);

	// cleanup
	close(server_socket);

    return 0;
} 

int initFileServer() {
	struct addrinfo hints;
	struct addrinfo *server_address;
	int server_socket;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	server_address = getSokcetAddrInfo(NULL, PORT, hints);
	return bindSocket(server_address);
}

void runServer(int server_socket) {
	for (;;) {
		int client_socket;
		pthread_t client_thread;
		int *temp_client_socket;

		client_socket = acceptConn(server_socket);
		if (client_socket == -1) {
			continue;
		}
		
		temp_client_socket = malloc(sizeof(int));
		*temp_client_socket = client_socket;

		pthread_create(&client_thread, NULL, handleConn, (void *) temp_client_socket);
	}
}

void *handleConn(void *client_socket_ptr) {
	int client_socket;
	char buff[BUFF_LEN];

	client_socket = *(int *) client_socket_ptr;
	free(client_socket_ptr);

	for (;;) {
		char *received_msg;

		bzero(buff, BUFF_LEN);
		if(read(client_socket, buff, BUFF_LEN) < 1) {
			break;
		}

		handleCmd(buff, client_socket);
	}
}