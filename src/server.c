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
#include "../lib/socket.h"
#include "../lib/communication.h"
#include "../lib/backup.h"
#include "../lib/log.h"

#define PORT 27015 

int main() {

    log_message("Creating socket...");
    int socket = create_socket("enp7s0");
    log_message("Socket created!");
    log_message("Server up and running!");
    log_message("Waiting for requests...");

    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    //int packets_received = 0;

    while (1) {
        listen_packet(&buffer, 9999, socket); // Remove later (LOOPBACK)
        //listen_packet(&buffer, 9999, socket); // Remove later (LOOPBACK)


        switch(buffer.type){
            case PT_ACK:
                printf("ACK received: %d\n", buffer.type);
                break;
            case PT_NACK:
                printf("NACK received: %d\n", buffer.type);
                break;
            case PT_BACKUP_ONE_FILE:
                log_message("BACKUP_ONE_FILE received!");
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket); // send OK 

                receive_file("backup.txt", socket);
                log_message("Waiting for request...");
                break;
            case PT_BACKUP_MULTIPLE_FILES:
                printf("BACKUP_FILES received: %d\n", buffer.type);
                create_or_modify_packet(packet, 0, 0, PT_OK, NULL);
                send_packet(packet, socket); // send OK 
                receive_multiple_files(socket);
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