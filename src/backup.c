#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include "../lib/backup.h"
#include "../lib/communication.h"


/* Get the size of a file in bytes. 
 * 
 * @param path The path of the file.
*/
long long int get_file_size(const char *path) {
    struct stat st;
    if(stat(path, &st) == -1) {
        perror("Could not get file size!");
        return -1;
    }
    return st.st_size;
}

/*
 * Send the file data to the server.
*/
void send_file_data(){
}

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
        perror("src_path is NULL!");
        return -1;
    }

    FILE *file = fopen(src_path, "rb");

    if(file == NULL) {
        perror("Could not open file!");
        return -1;
    }

    struct packet *p = create_packet(0, 0, PT_BACKUP_ONE_FILE, NULL);

    /* Send packet for start backup single file */
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0){
        printf("Error while trying to reach server!\n");
        return -1;
    }

    uint8_t data_buffer[63];
    struct packet p_buffer;


    int file_read_bytes = MAX_DATA_SIZE;
    long long file_size = get_file_size(src_path);
    int packets_quantity = ceil(file_size / (float)(MAX_DATA_SIZE));

    printf("\n\n");
    printf("Sending file: %s\n", src_path);
    printf("Packets quantity: %d\n", packets_quantity);
    printf("File size: %lld\n", file_size);

    int packet_sequence = 1;
    for(int i = 0; i < packets_quantity; i++) 
    {
        printf("\rSending packets [%d/%d]...", i+1, packets_quantity);
        fflush(stdout);

        /* Change length for the last packet. */
        if(i == packets_quantity - 1)
            file_read_bytes = file_size % MAX_DATA_SIZE;

        /* Read bytes from file. */
        fread(data_buffer, file_read_bytes, 1, file);

        /* Put read data into packet. */ 
        change_packet(p, file_read_bytes, packet_sequence, PT_DATA, data_buffer);

        /* Send packet. */
        if(send_packet_and_wait_for_response(p, &p_buffer, PT_TIMEOUT, socket) != 0) 
        {
            printf("Error while file: %s \n", src_path);
            fclose(file);
            destroy_packet(p);
            return -1;
        }
        
        if(packet_sequence == MAX_SEQUENCE)
            packet_sequence = 0;
        packet_sequence++;
    }
    printf("\nFile sent successfully!\n\n\n");

    fclose(file);
    destroy_packet(p);
    return 0;
}

int receive_file(char file_name, int socket){
    
    /* Create a file with "file_name" */
    FILE *file;

    file = fopen(file_name, "w"); // To do: verify mode

    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    /* Flag PT_DATA */
    struct packet *p = create_packet(0, 0, PT_DATA, NULL);

    // fprintf(file, "content\n");

    /* Read the packets from socket and concatenate into "file_name" */
    /* Send ACK */
    /* If parity is wrong send NACK */
    /* Until receive END packet. */
    /* Flag END_FILE */

    fclose(file);
    /* End */
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