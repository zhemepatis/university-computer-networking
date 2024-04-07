#ifndef FILES_H
    #define FILES_H

    int checkIfFileExists(char *file_path);
    void receiveFile(int sender, char *file_path);
    void sendFile(int receiver, char *file_path);

#endif