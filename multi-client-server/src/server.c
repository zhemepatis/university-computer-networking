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
	pthread_t thread;
	int socket;
	char *name;
};

int client_count;
struct client clients[MAX_CLIENT_COUNT];

void *handleClientConnection(void *args);
char *getName(int client_socket);

int main() 
{ 
	struct addrinfo hints;
	struct addrinfo *server_address;

	int server_socket;

	pthread_t client_thread;

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
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	printf("Server is listening...\n");

	client_count = 0;
	for (;;) {
		if (client_count == MAX_CLIENT_COUNT) {
			printf("max client count has been reached\n");
			continue;
		}

        if ((clients[client_count].socket = accept(server_socket, NULL, NULL)) == -1) {
            perror("accept");
            continue;
        }
		
		pthread_create(&(clients[client_count].thread), NULL, handleClientConnection, clients);
    }

	// cleanup
    close(server_socket);

    return 0;
} 

void *handleClientConnection(void *args) {
	struct client *clients;
	int curr_client_idx;
	struct client *curr_client;

	char *msg_start = "PRANESIMAS";
	
	char buff[BUFF_LEN];

	clients = (struct client *) args;
	curr_client_idx = client_count;
	curr_client = &(clients[curr_client_idx]);

	// get client name
	curr_client->name = getName(curr_client->socket);
	++client_count;

	// receive and broadcast messages
	for (;;) {
		char *received_msg;

		bzero(buff, BUFF_LEN);
		read(curr_client->socket, buff, BUFF_LEN);

		received_msg = realloc(received_msg, strlen(buff) + 1);
		strcpy(received_msg, buff);

		bzero(buff, BUFF_LEN);
		strcpy(buff, msg_start);
		strcat(buff, curr_client->name);
		strcat(buff, ": ");
		strcat(buff, received_msg);

		write(curr_client->socket, buff, strlen(buff));
	}

	// cleanup 
	free(curr_client->name);
	close(curr_client->socket);
	--client_count;
}

int isTaken(char *name, struct client *clients) {
	for (int i = 0; i < client_count; ++i) {
		char *temp_name = clients[i].name;
			printf("puff\n");

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

	name = calloc(0, sizeof(char));
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
			free(name);
			continue;
		}

		write(client_socket, name_is_ok_msg, strlen(name_is_ok_msg));
		return name;				
	}
}