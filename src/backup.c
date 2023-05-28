#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../lib/backup.h"
#include "../lib/communication.h"

void backup_single_file(const char *src_path, int socket) {
    if(src_path == NULL) {
        perror("src_path is NULL");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(src_path, "rb");

    if(file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct packet *p = create_packet(0, 0, 0, 0);

    printf("Sending file...\n\n\n");
    uint8_t buffer;
    while(fread(&buffer, 1, 1, file) > 0) {
        printf("%c", buffer);
        //send_packet(socket, p);
        //destroy_packet(p);
    }
    printf("\n\n\nFile sent!\n");
    printf("socket: %d\n", socket);

    fclose(file);
    destroy_packet(p);
}

//void backup_multiple_files(const char *src_dir, int socket) {
//}
//
//void restore_single_file(const char *src_path, int socket) {
//}
//
//void restore_multiple_files(const char *src_dir, int socket) {
//}
//
//void set_server_directory(const char *dir) {
//}
//
//void cd_local(const char *dir) {
//}
//
//int verify_file_md5(const char *file_path) {
//}
