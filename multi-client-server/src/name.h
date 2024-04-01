#ifndef NAME_H
    #define NAME_H

    int isTaken(char *name);
    char *getName(int client_socket);
    char *promptName(int client_socket);
    int validateName(char *name);

#endif