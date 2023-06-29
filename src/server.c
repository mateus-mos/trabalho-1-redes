#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include "../lib/socket.h"
#include "../lib/network.h"
#include "../lib/backup.h"
#include "../lib/log.h"
#include "../lib/utils.h"

#define FILE_MAP_NAME_TO_PATH "name_to_path.txt" 

int main() {
    log_message("Creating socket...");
    int socket = create_socket("enp7s0");
    log_message("Socket created!");
    log_message("Server up and running!");

    char current_directory[100];
    get_current_directory(current_directory, sizeof(current_directory));
    strcat(current_directory, "/");


    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    char *file_name = NULL;
    char *full_path_to_file = NULL;
    
    //int packets_received = 0;

    while (1) {
        log_message("Waiting for request...");
        listen_packet(&buffer, 9999, socket); 

        switch(buffer.type){
            case PT_ACK:
                log_message("ACK received!");
                break;
            case PT_NACK:
                log_message("NACK received!");
                break;
            case PT_BACKUP_ONE_FILE:
                log_message("BACKUP_ONE_FILE received!");

                // Send OK
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket); 

                file_name = uint8ArrayToString(buffer.data, buffer.size);
                full_path_to_file = concatenate_strings(current_directory, file_name);

                if(receive_file(full_path_to_file, socket) == -1){
                    log_message("Error receiving file!");
                    create_or_modify_packet(packet, 0, 0, PT_ERROR, "Error receiving file!");
                    send_packet(packet, socket); // send ERROR
                    free(file_name);
                    break;
                }

                save_file_info(file_name, full_path_to_file, FILE_MAP_NAME_TO_PATH);

                break;
            case PT_BACKUP_MULTIPLE_FILES:
                log_message("BACKUP_MULTIPLE_FILES received!");

                // Send OK
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket); 

                // Receive files
                receive_multiple_files(socket);
                break;

            case PT_RESTORE_ONE_FILE:
                log_message("RESTORE_FILE received!");

                // Receive file name
                file_name = uint8ArrayToString(buffer.data, buffer.size); 
                full_path_to_file = get_file_path(FILE_MAP_NAME_TO_PATH, file_name);

                if(full_path_to_file == NULL){
                    log_message("File does not exist!");

                    create_or_modify_packet(packet, 0, 0, PT_ERROR, "File does not exist!");
                    send_packet(packet, socket); // send ERROR

                    free(file_name);
                    break;
                }
                else
                {
                    log_message("Requested file exists!");
                }

                // Send OK
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket); 

                log_message("Getting file from:");
                log_message(full_path_to_file);
                log_message("Sending to:");
                log_message(file_name);

                if(send_single_file(full_path_to_file, file_name, socket) != 0){ 
                    log_message("Error sending file!"); 

                    // Error sending file
                    create_or_modify_packet(packet, 0, 0, PT_ERROR, "Error sending file!");
                    send_packet(packet, socket); // send ERROR

                    free(file_name);
                    free(full_path_to_file);
                    break;
                }

                free(file_name);
                free(full_path_to_file);
                break;

            case PT_RESTORE_FILES:
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket);
                break;
            case PT_END_GROUP_FILES:
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket);
                break;
            case PT_SET_SERVER_DIR:
                log_message("SET_SERVER_DIR received!");
                
                char *ssdir_command= uint8ArrayToString(buffer.data, buffer.size);
                log_message("ssdir command:");
                log_message(ssdir_command);

                if(chdir(ssdir_command) != 0)
                {
                    log_message("Error changing file!");
                    create_or_modify_packet(packet, 0, 0, PT_ERROR, NULL);
                    send_packet(packet, socket);
                }
                else
                {
                    log_message("Dir changed:");
                    log_message(ssdir_command);
                    get_current_directory(current_directory, sizeof(current_directory));
                    // Send OK
                    create_or_modify_packet(packet, MAX_DATA_SIZE, 0, PT_OK, current_directory);
                    send_packet(packet, socket); 
                }
                break;
            default:
                printf("Invalid packet received!\n");
                break;
        }
        
        // Reset the buffer
        buffer.start_marker = 0;
    }

    // Close the socket after sending and receiving frames
    close(socket);
    return 0;
}