#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include "files.h"
#include "parsing.h"
#include "file-commands.h"

#define FILE_STORAGE_PATH "storage/"

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
	char *ready_protocol = "PASIRUOSES";
	char *file_name;
	char *file_path;
	int file_path_len;

	file_name = parseNext(cmd);
	file_path_len = strlen(FILE_STORAGE_PATH) + strlen(file_name) + 1;
	file_path = calloc(file_path_len, sizeof(char));
	strcpy(file_path, FILE_STORAGE_PATH);
	strcat(file_path, file_name);

	write(client_socket, ready_protocol, strlen(ready_protocol) + 1);
	receiveFile(client_socket, file_path);

	// cleanup
	free(file_path);
}