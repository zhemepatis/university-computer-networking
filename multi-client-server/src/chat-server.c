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
#include <libgen.h>
#include "socket.h"
#include "commands.h"
#include "files.h"
#include "name.h"

#define CHAT_SERVER_PORT "9001"
#define FILE_SERVER_PORT "9002"
#define FILE_SERVER_HOST "::1"

#define GET_FROM_FILE_SERVER_CMD "#get"
#define SAVE_TO_FILE_SERVER_CMD "#save"
#define DOWNLOADS_PATH "/home/zhemepatis/Downloads/"

int client_count = 0;
struct client clients[MAX_CLIENT_COUNT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int file_server_socket = -1;

int initChatServer();
void *runServer(void *server_socket_ptr);
void *handleConn(void *client_socket_ptr);
int connToFileServer();

void handleCmd(char *cmd, int client_socket);
void handleGet(char *file_name);
void handleSave(char *file_path);

void broadcastMessage(char *sender, char *msg);

void removeClient(int client_socket);

int main() 
{ 
	pthread_t chat_server_thread;
	int chat_server_socket;

	// init and start chat server
	chat_server_socket = initChatServer();
	listenForConn(chat_server_socket);
	pthread_create(&chat_server_thread, NULL, runServer, &chat_server_socket);

	// conn to file server
	file_server_socket = connToFileServer();

	// wait until chat server ends its work
	pthread_join(chat_server_thread, NULL);

    return 0;
}

int initChatServer() {
	struct addrinfo hints;
	struct addrinfo *server_address;
	int server_socket;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	server_address = getSokcetAddrInfo(NULL, CHAT_SERVER_PORT, hints);
	return bindSocket(server_address);
}

int connToFileServer() {
	struct addrinfo hints;
    struct addrinfo *server_address;
    int server_socket;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

	server_address = getSokcetAddrInfo(FILE_SERVER_HOST, FILE_SERVER_PORT, hints);
	server_socket = connToSocket(server_address);

	return server_socket;
}

void *runServer(void *server_socket_ptr) {
	int server_socket;

	server_socket = *(int *) server_socket_ptr;

	for (;;) {
		int client_socket;
		pthread_t client_thread;
		int *temp_client_socket;

		client_socket = acceptConn(server_socket);
		if (client_socket == -1) {
			continue;
		}

		pthread_mutex_lock(&mutex);
		if (client_count == MAX_CLIENT_COUNT) {
			printf("Max client count has been reached, connection refused\n");
			close(client_socket);
			continue;
		}
		
		temp_client_socket = malloc(sizeof(int));
		*temp_client_socket = client_socket;

		pthread_create(&client_thread, NULL, handleConn, (void *) temp_client_socket);
		pthread_mutex_unlock(&mutex);
	}

	// cleanup
	pthread_mutex_destroy(&mutex);
    close(server_socket);
}

void *handleConn(void *client_socket_ptr) {
	int client_socket;

	char *client_name;
	char buff[BUFF_LEN];

	client_socket = *(int *) client_socket_ptr;

	client_name = getName(client_socket);

	pthread_mutex_lock(&mutex);
	clients[client_count].socket = client_socket;
	clients[client_count].name = client_name;
	++client_count;
	pthread_mutex_unlock(&mutex);

	bzero(buff, BUFF_LEN);
	strcpy(buff, client_name);
	strcat(buff, " logged in\n");
	broadcastMessage("server", buff);

	// receive and broadcast messages
	for (;;) {
		char *received_msg;

		bzero(buff, BUFF_LEN);
		if(read(client_socket, buff, BUFF_LEN) < 1) {
			break;
		}

		handleCmd(buff, client_socket);

		received_msg = realloc(received_msg, strlen(buff) + 1);
		strcpy(received_msg, buff);
		broadcastMessage(client_name, received_msg);
	}

	bzero(buff, BUFF_LEN);
	strcpy(buff, client_name);
	strcat(buff, " logged out\n");
	broadcastMessage("server", buff);

	// cleanup
	pthread_mutex_lock(&mutex);
	removeClient(client_socket);
	pthread_mutex_unlock(&mutex);

	free(client_name);
	free(client_socket_ptr);
	close(client_socket);
}

void removeClient(int client_socket) {
	for(int i = 0; i < client_count; ++i) {
		if (clients[i].socket != client_socket) {
			continue;
		}

		for (int j = i; j < client_count - 1; ++j) {
			clients[j].socket = clients[j+1].socket;
			clients[j].name = clients[j+1].name;
		}
	}
	--client_count;
}

void handleCmd(char *cmd, int client_socket) {
	char *temp_cmd;
	char *action;
	char *file_path;

	temp_cmd = calloc(strlen(cmd) + 1, sizeof(char));
	strcpy(temp_cmd, cmd);

	// get cmd type
	action = parseNext(temp_cmd);
	if (action == NULL) {
		return;
	}

	// get file path
	file_path = parseNext(temp_cmd);
	if (file_path == NULL) {
		free(action);
		free(file_path);
		return;
	}

	// handle action
	if (strcmp(action, "#get") == 0) {
		handleGet(file_path);
	} 
	else if (strcmp(action, "#save") == 0) {
		handleSave(file_path);
	}
	
	// cleanup 
	free(action);
	free(file_path);
	free(temp_cmd);
}

void handleGet(char *file_name) {
	const char *send_protocol = "SIUSK ";
	char *file_path;
	int file_path_len;
	char buff[BUFF_LEN];

	bzero(buff, BUFF_LEN);
	strcpy(buff, send_protocol);
	strcat(buff, file_name);
	write(file_server_socket, buff, strlen(buff) + 1);

	file_path_len = strlen(DOWNLOADS_PATH) + strlen(file_name) + 1;
	file_path = calloc(file_path_len, sizeof(char));
	strcpy(file_path, DOWNLOADS_PATH);
	strcat(file_path, file_name);

	receiveFile(file_server_socket, file_path);	

	// cleanup
	free(file_path);
}

void handleSave(char *file_path) {
	const char *save_protocol = "SAUGOK ";
	char *file_name;
	char buff[BUFF_LEN];

	// get file name
	file_name = basename(file_path);

	// send protocol
	bzero(buff, BUFF_LEN);
	strcpy(buff, save_protocol);
	strcat(buff, file_name);
	write(file_server_socket, buff, strlen(buff) + 1);

	sendFile(file_server_socket, file_path);
}

void broadcastMessage(char *sender, char *msg) {
	char *msg_start = "PRANESIMAS";
	char buff[BUFF_LEN];

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < client_count; ++i) {
		bzero(buff, BUFF_LEN);
		strcpy(buff, msg_start);
		strcat(buff, sender);
		strcat(buff, ": ");
		strcat(buff, msg);

		write(clients[i].socket, buff, strlen(buff));
	}
	pthread_mutex_unlock(&mutex);
}