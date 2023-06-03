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

#define PORT 27015 

int main() {

    printf("Creating socket...\n");
    int socket = create_socket("lo");
    printf("Socket created!\n");
    printf("Waiting for client...\n");

    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);
    //int packets_received = 0;
    int ignore = 1;

    while (1) {
        listen_packet(&buffer, 9999, socket);

        /* The "ignore" variable is used to ignore duplicated packets.
         * Remove this "if" when change the interface.
         */
        if(ignore == 1){
            /* The above "if" ignore packets that server sent.
             * This occurs only in the "loopback" interface. 
             * Remove this "if" when change the interface.
             */
            if(memcmp(&buffer, packet, sizeof(struct packet)) != 0){
                switch(buffer.type){
                    case PT_ACK:
                        printf("ACK received: %d\n", buffer.type);
                        break;
                    case PT_DATA:
                        printf("DATA received: %d\n", buffer.type);
                        send_packet(packet, socket); // Send ACK
                        break;
                    case PT_NACK:
                        printf("NACK received: %d\n", buffer.type);
                        break;
                    case PT_BACKUP_ONE_FILE:
                        printf("BACKUP_ONE_FILE received: %d\n", buffer.type);
                        send_packet(packet, socket); // Send ACK
                        receive_file("backup.txt", socket);
                        break;
                    case PT_BACKUP_FILES:
                        printf("BACKUP_FILES received: %d\n", buffer.type);
                        break;
                    default:
                        printf("Invalid packet received!\n");
                        break;
                }
            }
            ignore *= -1;
        } else if(ignore == -1 && is_a_valid_packet(&buffer)){
            ignore *= -1;
        }
        
        // Reset the buffer
        buffer.start_marker = 0;
    }

    // Close the socket after sending and receiving frames
    close(socket);
    return 0;
}