#ifndef FILES_H
    #define FILES_H

    // const char *receive_file_msg = "SAUGOK";
    // const char *send_file_msg = "SIUSK";
    // const char *file_exists_msg = "FAILASOK";
    // const char *job_done_msg = "BAIGTA";

    int checkIfFileExists(char *file_path);
    void receiveFile(int sender, char *file_path);
    void sendFile(int receiver, char *file_path);

#endif