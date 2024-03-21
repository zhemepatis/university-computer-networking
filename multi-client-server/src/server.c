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

#define PORT "9001"

#define FILE_STORAGE_PATH "storage/"
#define BUFF_LEN 256

#define MAX_CLIENT_COUNT 5

struct client {
	int socket;
	char *name;
};

int client_count = 0;
struct client clients[MAX_CLIENT_COUNT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handleClientConnection(void *curr_client);
char *getName(int client_socket);
void broadcastMessage(char *sender, char *msg);

int main() 
{ 
	struct addrinfo hints;
	struct addrinfo *server_address;

	int server_socket;
	int client_socket;

	pthread_t client_thread;

	int server_msg_len;
	char buff[BUFF_LEN];

	// initialise server
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
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	printf("Server is listening...\n");

	// accept connections
	for (;;) {
		pthread_t threadId;
		int *temp_client_socket;

        if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            perror("accept");
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

		pthread_create(&threadId, NULL, handleClientConnection, (void *) temp_client_socket);
		pthread_mutex_unlock(&mutex);
    }

	// cleanup
	pthread_mutex_destroy(&mutex);
    close(server_socket);

    return 0;
} 

void *handleClientConnection(void *client_socket_ptr) {
	int client_socket;

	char *client_name;
	char buff[BUFF_LEN];

	client_socket = *(int *) client_socket_ptr;
	free(client_socket_ptr);

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
	pthread_mutex_unlock(&mutex);

	free(client_name);
	close(client_socket);
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

int isTaken(char *name, struct client *clients) {
	for (int i = 0; i < client_count; ++i) {
		char *temp_name = clients[i].name;

		if (strcmp(name, temp_name) == 0) {
			return 1;
		}
	}

	return 0;
}

char *getName(int client_socket) {
	char *enter_name_msg = "ATSIUSKVARDA\n";
	char *name_is_ok_msg = "VARDASOK\n";

	char buff[BUFF_LEN];
	char *name;

	for (;;) {
		write(client_socket, enter_name_msg, strlen(enter_name_msg));

		bzero(buff, BUFF_LEN);
		read(client_socket, buff, BUFF_LEN);

		if (strlen(buff) == 0 || buff[0] == '\n') {
			printf("Invalid name\n");
			continue;
		}

		name = calloc(strlen(buff),  sizeof(char));
		strncpy(name, buff, strlen(buff) - 1);

		if (name == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

		if (isTaken(name, clients) == 1) {
			printf("Name is already taken\n");
			free(name);
			continue;
		}

		write(client_socket, name_is_ok_msg, strlen(name_is_ok_msg));
		return name;				
	}
}