#ifndef CHAT_COMMANDS_H
    #define CHAT_COMMANDS_H

    void handleCmd(char *cmd, int client_socket);
    void handleGet(char *file_name, int client_socket);
    void handleSave(char *file_path, int client_socket);

#endif