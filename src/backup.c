#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include "../lib/backup.h"
#include "../lib/communication.h"
#include "../lib/log.h"


/* Get the size of a file in bytes. 
 * 
 * @param path The path of the file.
*/
long long int get_file_size(const char *path) 
{
    struct stat st;
    if(stat(path, &st) == -1) 
    {
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
int backup_single_file(const char *src_path, int socket) 
{
    if(src_path == NULL) 
    {
        perror("src_path is NULL!");
        return -1;
    }

    FILE *file = fopen(src_path, "rb");

    if(file == NULL) 
    {
        perror("Could not open file!");
        return -1;
    }

    struct packet *p = create_or_modify_packet(NULL, 0, 0, PT_BACKUP_ONE_FILE, NULL);

    /* Send packet for start backup single file */
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
    {
        log_message("Error while trying to reach server!\n");
        return -1;
    }

    /* Create buffers */
    uint8_t data_buffer[63];
    struct packet p_buffer;

    int file_read_bytes = MAX_DATA_SIZE;
    long long file_size = get_file_size(src_path);
    int packets_quantity = ceil(file_size / (float)(MAX_DATA_SIZE));

    #ifdef DEBUG
    log_message("Sending file:");
    log_message(src_path);
    #endif

    int packet_sequence = 1;
    for(int i = 0; i < packets_quantity; i++) 
    {
        /* Sometimes the last packet is not full, 
         * so we need to know how many bytes we need to read. 
        */
        if(i == packets_quantity - 1)
        {
            file_read_bytes = file_size % MAX_DATA_SIZE;
            p->data[file_read_bytes] = '\0';
        }

        /* Read bytes from file. */
        fread(data_buffer, file_read_bytes, 1, file);

        /* Put read data into packet. */ 
        create_or_modify_packet(p, file_read_bytes, packet_sequence, PT_DATA, data_buffer);

        /* Send packet. */
        if(send_packet_and_wait_for_response(p, &p_buffer, PT_TIMEOUT, socket) != 0) 
        {
            printf("Error while sending file: %s \n", src_path);
            fclose(file);
            destroy_packet(p);
            return -1;
        }
        
        /* Reset packet sequence number. */ 
        if(packet_sequence == MAX_SEQUENCE)
            packet_sequence = 0;
        packet_sequence++;

    }

    fclose(file);

    /* Send end file packet. */
    create_or_modify_packet(p, 0, 0, PT_END_FILE, NULL);
    if(send_packet_and_wait_for_response(p, &p_buffer, PT_TIMEOUT, socket) != 0)
    {
        printf("Couldn't send end file packet!\n"); 
        destroy_packet(p);
        return -1;
    }

    destroy_packet(p);
    log_message("File sent successfully!");
    return 0;
}

int receive_file(const char *file_name, int socket)
{   
    log_message("Receiving file:");
    log_message(file_name);
    // Create a file with "file_name"
    FILE *file = fopen(file_name, "wb"); 
    if (file == NULL) 
    {
        perror("Error opening the file");
        return 1;
    }

    struct packet packet_buffer;
    struct packet *response = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    long long int packets_received = 1;
    int end_file_received = 0;

    /* Listen for packets and process them */
    while(!end_file_received) 
    {
        int listen_status = listen_packet(&packet_buffer, PT_TIMEOUT, socket);
        log_message("Packet of file received"); 


        if(packet_buffer.type == PT_END_FILE)
        {
            end_file_received = 1;
        }

        if(listen_status == -1)
        {
            perror("Error listening for packets");
            fclose(file);
            destroy_packet(response);
            return -1;
        } 
        else if (listen_status == -2) 
        {
            log_message("Timeout waiting for packets");
            log_message("File transfer failed");
            fclose(file);
            destroy_packet(response);
            return -2;
        }
        if(packet_buffer.type == PT_DATA)
        {
            /* Write received data to the file */
            fwrite(packet_buffer.data, packet_buffer.size, 1, file);

            /* Send ACK packet */
            if(send_packet(response, socket) == -1)
            {
                perror("Error sending ACK packet");
                fclose(file);
                destroy_packet(response);
                return -1;
            }
            packets_received++;
        }
    }

    fclose(file);

    /* Send OK packet */
    log_message("Sending OK packet");
    create_or_modify_packet(response, 0, 0, PT_OK, NULL);
    send_packet(response, socket);


    log_message("File received!");
    destroy_packet(response);
    return 0;
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