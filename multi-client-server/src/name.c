#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include "name.h"
#include "setup.h"

extern int client_count;
extern struct client clients[MAX_CLIENT_COUNT];

int isTaken(char *name) {
	for (int i = 0; i < client_count; ++i) {
		char *temp_name = clients[i].name;

		if (strcmp(name, temp_name) == 0) {
			return 1;
		}
	}

	return 0;
}

char *getName(int client_socket) {
	char *name_is_ok_msg = "VARDASOK\n";

	char buff[BUFF_LEN];
	char *name;

	for (;;) {
		name = promptName(client_socket);

		if (validateName(name) == 1) {
			printf("Invalid name\n");
			free(name);
			continue;
		}

		if (isTaken(name) == 1) {
			printf("Name is already taken\n");
			free(name);
			continue;
		}

		write(client_socket, name_is_ok_msg, strlen(name_is_ok_msg));
		return name;				
	}
}

char *promptName(int client_socket) {
	char *enter_name_msg = "ATSIUSKVARDA\n";
	char *buff = calloc(BUFF_LEN, sizeof(char));
	char *name;

	write(client_socket, enter_name_msg, strlen(enter_name_msg));

	bzero(buff, BUFF_LEN);
	read(client_socket, buff, BUFF_LEN);
	buff[strcspn(buff, "\r\n")] = 0;

	name = realloc(buff, strlen(buff) + 1);

	return name;
}

int validateName(char *name) {
	if (strlen(name) == 0 || name == NULL) {
		return 1;
	}

	return 0;
}