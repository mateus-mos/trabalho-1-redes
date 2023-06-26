#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../lib/backup.h"
#include "../lib/network.h"
#include "../lib/log.h"
#include "../lib/utils.h"

/*
 * Sends a single file to the server. 
 * 
 * @param src_path The path of the file to be sent.
 * @param socket The socket to send the file.
 * @return 0 if the file was sent successfully, -1 otherwise.
 *  
*/
int send_single_file(char *src_path, int socket) 
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

    struct packet *p = create_or_modify_packet(NULL, MAX_FILE_NAME_SIZE, 0, PT_BACKUP_ONE_FILE, src_path);

    if(p == NULL) 
    {
        perror("Could not create packet!");
        return -1; 
    }

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
        #ifdef DEBUG
        log_message("Couldn't send end file packet!\n");
        #endif
        destroy_packet(p);
        return -1;
    }

    destroy_packet(p);
    log_message("File sent successfully!");
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

int send_multiple_files(char files[][MAX_FILE_NAME_SIZE], int files_quantity, int socket)
{
    /* Send packet for start backup multiple files */
    struct packet *p = create_or_modify_packet(NULL, 0, 0, PT_BACKUP_MULTIPLE_FILES, NULL);

    /* Send packet for start backup multiple files */
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
        return -1;

    for(int i = 0; i < files_quantity; i++)
    {
        if(send_single_file(files[i], socket) != 0)
        {
            #ifdef DEBUG
            log_message("Error while backing up multiple files!");
            #endif
            return -1;
        }
    }

    /* Send end backup multiple files packet */
    create_or_modify_packet(p, 0, 0, PT_END_GROUP_FILES, NULL);
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
    {
        #ifdef DEBUG
        log_message("Error while sending end backup multiple files packet!");
        #endif
        destroy_packet(p);
        return -1;
    }

    return 0;
}

/* 
 * Receives a single file from the client.
 *
 * @param full_path The full path of the file to be received.
 * @param socket The socket to receive the file.
 * 
 * @return 0 if the file was received successfully, -1 otherwise.
 * 
*/
int receive_file(char *full_path, int socket)
{   
    log_message("Receiving file:");
    log_message(full_path);

    // Create a file with "file_name"
    FILE *file = fopen(full_path, "wb"); 
    if (file == NULL) 
    {
        perror("Error opening the file");
        return -1;
    }

    struct packet packet_buffer;
    struct packet *response = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    long long int packets_received = 1;
    int end_file_received = 0;

    /* Listen for packets and process them */
    while(!end_file_received) 
    {
        int listen_status = listen_packet(&packet_buffer, PT_TIMEOUT, socket);

        if(packet_buffer.type == PT_END_FILE)
            end_file_received = 1;

        if(listen_status != 0) // parando aqui
        {
            printf("\n listen_status: %d\n", listen_status);
            log_message("An error ocurred while listening for packets");
            log_message("Is the server still running?");
            fclose(file);
            destroy_packet(response);
            return -1;
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
 * Receives multiple files from the client.
 * 
 * @param socket The socket to receive the files.
 * @return 0 if the files were received successfully, -1 otherwise.
 * 
*/
int receive_multiple_files(int socket)
{
    int end_files = 0;
    struct packet packet_buffer;
    char *file_name = NULL;

    while(end_files == 0)
    {
        if(listen_packet(&packet_buffer, PT_TIMEOUT, socket) != 0)
        {
            #ifdef DEBUG
                log_message("An error ocurred while listening for packets");
                log_message("Is the server still running?");
            #endif
            return -1;
        }

        /* Convert the file name from uint8_t array to string */
        // uint8_array_to_string(packet_buffer.size, packet_buffer.data, file_name, sizeof(file_name));
        
        if(packet_buffer.type == PT_BACKUP_ONE_FILE)
        {
            file_name = uint8ArrayToString(packet_buffer.data, packet_buffer.size);
            receive_file(file_name, socket);
        }
        else if(packet_buffer.type == PT_END_GROUP_FILES)
            end_files = 1;
        else
        {

            if(file_name != NULL)
                free(file_name);
            #ifdef DEBUG
            log_message("Received unexpected packet type while receiving multiple files");
            return -1;
            #endif
        }

    }

    if(file_name != NULL)
        free(file_name);
    return 0;
}


void restore_single_file(char *file_name, int socket) {
    struct packet *p = create_or_modify_packet(NULL, MAX_FILE_NAME_SIZE, 0, PT_RESTORE_ONE_FILE, file_name); 

    printf("\n nome do arq: %s \n", p->data);
    printf("\n file_name value: %s\n", file_name);
    /* Send packet for start restore single file */
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
    {
        printf(" Error while sending start restore single file packet!\n");
        return;
    }

    if(p->type == PT_ERROR)
    {
        char *error_msg = uint8ArrayToString(p->data, p->size);

        printf(" Error while restoring file!\n");
        printf(" Error message: %s\n", error_msg);

        destroy_packet(p);
        free(error_msg);
        return;
    }

    create_or_modify_packet(p, MAX_FILE_NAME_SIZE, 0, PT_BACKUP_ONE_FILE, file_name);

    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
    {
        printf(" Error while sending backup single file packet!\n");
        destroy_packet(p);
        return;
    }

    /* Send file */
    receive_file(file_name, socket);

    /* Send end restore single file packet */
    create_or_modify_packet(p, 0, 0, PT_END_FILE, NULL);
    if(send_packet_and_wait_for_response(p, p, PT_TIMEOUT, socket) != 0)
    {
        printf(" Error while sending end restore single file packet!\n");
        destroy_packet(p);
        return;
    }

    destroy_packet(p);
    printf(" File restored successfully!\n");
    return;
}

void restore_multiple_files(char files[][MAX_FILE_NAME_SIZE], int files_quantity, int socket)
{
}
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