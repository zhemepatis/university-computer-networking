#ifndef FILE_COMMANDS_H
    #define FILE_COMMANDS_H

    void handleCmd(char *cmd, int client_socket);
    void handleGet(int client_socket, char *cmd);
    void handleSave(int client_scoket, char *cmd);

#endif