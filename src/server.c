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

int main()
{
    log_message("Creating socket...");
    int socket = create_socket("eno1");
    log_message("Socket created!");
    log_message("Server up and running!");

    char current_directory[100];
    get_current_directory(current_directory, sizeof(current_directory));

    log_message(current_directory);


    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    char *file_name = NULL;
    char *full_path_to_file = NULL;

    while (1)
    {
        log_message("Waiting for request...");
        listen_packet(&buffer, 9999, socket);

        switch (buffer.type)
        {
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

            if (receive_file(file_name, socket) == -1)
            {
                log_message("Error receiving file!");
                create_or_modify_packet(packet, 0, 0, PT_ERROR, "Error receiving file!");
                send_packet(packet, socket); // send ERROR
                free(file_name);
                break;
            }

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
            log_message("File to restore:");
            log_message(file_name);

            if (access(file_name, F_OK) != 0)
            {
                log_message("File does not exist!");

                create_or_modify_packet(packet, MAX_DATA_SIZE, 0, PT_ERROR, "File does not exist!");
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

            log_message("Sending:");
            log_message(file_name);

            if (send_single_file(file_name, socket) != 0)
            {
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

        case PT_END_FILE:
            create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
            send_packet(packet, socket);
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

            char *ssdir_command = uint8ArrayToString(buffer.data, buffer.size);
            log_message("ssdir command received:");
            log_message(ssdir_command);

            if (chdir(ssdir_command) != 0)
            {
                log_message("Error changing directory!");

                if(create_new_directory(ssdir_command) == 0)
                {
                    log_message("New directory created with name:");
                    log_message(ssdir_command);
                    chdir(ssdir_command);

                    get_current_directory(current_directory, sizeof(current_directory));
                    create_or_modify_packet(packet, MAX_DATA_SIZE, 0, PT_OK, current_directory);
                    send_packet(packet, socket);
                }
                else 
                {
                    create_or_modify_packet(packet, 0, 0, PT_ERROR, NULL);
                    send_packet(packet, socket);
                }
            }
            else
            {
                get_current_directory(current_directory, sizeof(current_directory));
                log_message("Changed directory to:");
                log_message(current_directory);

                create_or_modify_packet(packet, MAX_DATA_SIZE, 0, PT_OK, current_directory);
                send_packet(packet, socket);
            }
            break;

        case PT_VERIFY_BACKUP:
            log_message("PT_VERIFY_BACKUP received!");
            char md5[MAX_PACKET_SIZE];
            file_name = uint8ArrayToString(buffer.data, buffer.size);
            log_message("File:");
            log_message(file_name);

            if (access(file_name, F_OK) != 0)
            {
                log_message("File does not exist!");
                create_or_modify_packet(packet, 0, 0, PT_ERROR, NULL);
                send_packet(packet, socket);
                log_message("Sending error message to client...");
                free(full_path_to_file);
                continue;
            }

            if (file_to_md5(file_name, md5) == -1)
            {
                log_message("File does not exist!");
                create_or_modify_packet(packet, 0, 0, PT_ERROR, NULL);
                send_packet(packet, socket);
                log_message("Sending error message to client...");
                continue;
            }

            log_message("Sending md5 for client...");

            create_or_modify_packet(packet, MAX_DATA_SIZE, 0, PT_MD5, md5);
            send_packet(packet, socket);
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