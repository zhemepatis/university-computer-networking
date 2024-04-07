#ifndef MESSAGES_H
    #define MESSAGES_H

    char *getMessage(char *sender, char *msg);
    void sendMessage(int receiver_socket, char *sender, char *msg);
    void broadcastMessage(char *sender, char *msg);

#endif