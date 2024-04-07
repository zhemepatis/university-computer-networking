#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include "messages.h"
#include "setup.h"

extern int client_count;
extern struct client clients[MAX_CLIENT_COUNT];
extern pthread_mutex_t mutex;

char *getMessage(char *sender, char *msg) {
	char *msg_start = "PRANESIMAS";
	char *buff;

	buff = calloc(strlen(msg_start) + strlen(msg) + 3, sizeof(char));
	strcpy(buff, msg_start);
	strcat(buff, sender);
	strcat(buff, ": ");
	strcat(buff, msg);

	return buff;
}

void sendMessage(int receiver_socket, char *sender, char *msg) {
	char *protocol_msg;

	protocol_msg = getMessage(sender, msg);
	write(receiver_socket, protocol_msg, strlen(protocol_msg));

	free(protocol_msg);
}

void broadcastMessage(char *sender, char *msg) {
	char *protocol_msg;

	protocol_msg = getMessage(sender, msg);

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < client_count; ++i) {
		write(clients[i].socket, protocol_msg, strlen(protocol_msg));
	}
	pthread_mutex_unlock(&mutex);

	free(protocol_msg);
}