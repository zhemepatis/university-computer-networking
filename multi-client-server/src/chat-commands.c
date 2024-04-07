#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <libgen.h>
#include "files.h"
#include "parsing.h"
#include "messages.h"
#include "chat-commands.h"
#include "setup.h"

#define DOWNLOADS_PATH "/home/zhemepatis/Downloads/"

extern int file_server_socket;

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
		handleGet(file_path, client_socket);
	} 
	else if (strcmp(action, "#save") == 0) {
		handleSave(file_path, client_socket);
	}
	
	// cleanup 
	free(action);
	free(file_path);
	free(temp_cmd);
}

void handleGet(char *file_name, int client_socket) {
	const char *send_protocol = "SIUSK ";
	char *ready_protocol = "PASIRUOSES";
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

void handleSave(char *file_path, int client_socket) {
	const char *save_protocol = "SAUGOK ";
	char *file_name;
	char buff[BUFF_LEN];

	file_name = basename(file_path);

	bzero(buff, BUFF_LEN);
	strcpy(buff, save_protocol);
	strcat(buff, file_name);
	write(file_server_socket, buff, strlen(buff) + 1);

	bzero(buff, BUFF_LEN);
	read(file_server_socket, buff, BUFF_LEN);
	if (strcmp(buff, "PASIRUOSES") != 0) {
		sendMessage(client_socket, "server", "error while saving file");
	}

	sendFile(file_server_socket, file_path);
}