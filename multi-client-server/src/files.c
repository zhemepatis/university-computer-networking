#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "files.h"
#include "setup.h"


void receiveFile(int sender, char *file_path) {
    const char *send_file_size_protocol = "SIUSKFAILODYDI";
    int file_size;
    int bytes_read;
    char buff[BUFF_LEN];
    int remaining;
    
    // receive file size
    read(sender, &file_size, sizeof(file_size));

    // open file 
    FILE *fp = fopen(file_path, "wb");
    remaining = file_size;

    // receive file
    bzero(buff, BUFF_LEN);
    printf("Receiving file %s\n", file_path);
    while ((remaining > 0) && ((bytes_read = read(sender, buff, BUFF_LEN)) > 0)) {
        fwrite(buff, sizeof(char), bytes_read, fp);
        remaining -= bytes_read;

        printf("Bytes received: %d/%d\n", file_size - remaining, file_size);

        bzero(buff, BUFF_LEN);
    }
    printf("File has been received\n");

    // cleanup
    fclose(fp);
}

void sendFile(int receiver, char *file_path) {
    int fp;
    struct stat file_stat;
    int file_size;
    off_t offset;
    int remaining;
    int sent_bytes;
    char buff[BUFF_LEN];

    // open file, get stats
    fp = open(file_path, O_RDONLY);
    fstat(fp, &file_stat);
    file_size = file_stat.st_size;

    int i = write(receiver, &file_size, sizeof(file_size));
    printf("write: %d\n", i);

    // send file
    offset = 0;
    remaining = file_size;

    printf("Sending file %s\n", file_path);
    while (((sent_bytes = sendfile(receiver, fp, &offset, BUFF_LEN)) > 0) && (remaining > 0))
    {
        remaining -= sent_bytes;
        printf("Bytes sent: %d/%d\n", file_size - remaining, file_size);
    }
    printf("File has been sent.\n");        
}