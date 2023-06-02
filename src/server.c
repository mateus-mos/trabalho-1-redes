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

#define PORT 27015 

int main() {

    printf("Creating socket...\n");
    int sockfd = create_socket("lo");
    printf("Socket created!\n");
    printf("Waiting for client...\n");

    struct packet buffer;
    struct packet *p = create_packet(0, 0, PT_ACK, NULL);
    int packets_received = 0;
    int ignore = 1;

    while (1) {
        ssize_t len = recvfrom(sockfd, &buffer, sizeof(buffer), 0, NULL, NULL);
        if (len == -1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }


        /* The "ignore" variable is used to ignore duplicated packets.
         * Remove this "if" when change the interface.
         */
        if(ignore == 1 && is_a_valid_packet(&buffer)){
            /* The above "if" ignore packets that server sent.
             * This occurs only in the "loopback" interface. 
             * Remove this "if" when change the interface.
             */
            if(memcmp(&buffer, p, sizeof(struct packet)) != 0){
                printf("Packet received: %d\n", packets_received);
                send_packet(sockfd, p);
                packets_received++;
            }
            ignore *= -1;
        } else if(ignore == -1 && is_a_valid_packet(&buffer)){
            ignore *= -1;
        }
        
        buffer.start_marker = 0;
    }

    // Close the socket after sending and receiving frames
    close(sockfd);
    return 0;
}