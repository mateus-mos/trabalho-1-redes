#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../lib/backup.h"
#include "../lib/communication.h"

/*
 * Sends a single file to the server. 
 * 
 * @param src_path The path of the file to be sent.
 * @param socket The socket to send the file.
 * @return 0 if the file was sent successfully, -1 otherwise.
 *  
*/
int backup_single_file(const char *src_path, int socket) {
    if(src_path == NULL) {
        perror("src_path is NULL");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(src_path, "rb");

    if(file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct packet *p = create_packet(0, 0, PT_BACKUP_ONE_FILE, NULL);

    send_packet(socket, p);
    listen_packet(p, PT_TIMEOUT, socket);
    
    /* Send packet and wait for reponse */

    printf("Sending file...\n\n\n");
    uint8_t buffer;
    int packet_sequence = 1;

    while(fread(&buffer, 1, 1, file) > 0) {
        /* Check if exist enough bytes */
        change_packet(p, 8, packet_sequence, PT_DATA, &buffer);
        send_packet(socket, p);
        if(listen_packet(p, PT_ACK, socket) == 0)
            printf("ACK received!\n");
        packet_sequence++;
    }

    /* Send end packet */
    printf("\n\n\nFile sent!\n");
    printf("socket: %d\n", socket);

    fclose(file);
    destroy_packet(p);
}

int receive_file(char file_name, int socket){
    /* Open a file to write the bytes */

    /* Receive packtes */


}

/*
 * Sends multiple files to the server.
 * 
 * @param src_dir The path of the directory containing the files to be sent.
 * @param socket The socket to send the files.
 * @return 0 if the files were sent successfully, -1 otherwise.
 * 
*/
//int backup_multiple_files(const char *src_dir, int socket) {
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