#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <pthread.h>
#include "socket.h"
#include "files.h"
#include "commands.h"

#define PORT "9002"
#define FILE_STORAGE_PATH "storage/"

int initFileServer();
void runServer(int server_socket);
void *handleConn(void *client_socket_ptr);

void handleCmd(char *cmd, int client_socket);
void handleGet(int client_socket, char *cmd);
void handleSave(int client_scoket, char *cmd);

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

void handleCmd(char *cmd, int client_socket) {
	char *temp_cmd;
	char *action;

	temp_cmd = calloc(strlen(cmd) + 1, sizeof(char));
	strcpy(temp_cmd, cmd);

	action = parseNext(temp_cmd);
	if (strcmp(action, "SAUGOK") == 0) {
		handleSave(client_socket, temp_cmd);
	}
	else if (strcmp(action, "SIUSK") == 0) {
		handleGet(client_socket, temp_cmd);
	}
}

void handleGet(int client_socket, char *cmd) {
	char *file_name;
	char *file_path;
	int file_path_len;

	file_name = parseNext(cmd);
	file_path_len = strlen(FILE_STORAGE_PATH) + strlen(file_name) + 1;
	file_path = calloc(file_path_len, sizeof(char));
	strcpy(file_path, FILE_STORAGE_PATH);
	strcat(file_path, file_name);

	sendFile(client_socket, file_path);

	// cleanup
	free(file_path);
}

void handleSave(int client_socket, char *cmd) {
	char *file_name;
	char *file_path;
	int file_path_len;

	file_name = parseNext(cmd);
	file_path_len = strlen(FILE_STORAGE_PATH) + strlen(file_name) + 1;
	file_path = calloc(file_path_len, sizeof(char));
	strcpy(file_path, FILE_STORAGE_PATH);
	strcat(file_path, file_name);

	receiveFile(client_socket, file_path);

	// cleanup
	free(file_path);
}