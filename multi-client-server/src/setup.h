#ifndef SETUP_H
    #define SETUP_H

    #define MAX_CLIENT_COUNT 5
    #define BUFF_LEN 256

    struct client {
        int socket;
        char *name;
    };

#endif