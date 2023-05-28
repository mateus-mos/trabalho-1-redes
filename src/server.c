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

    uint8_t buffer[2048];
    while (1) {
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (len == -1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if(is_a_valid_packet(buffer)){
            printf("\nReceived a packet from client!\n");
            printf("Packet size: %d\n", buffer[1]);
            printf("Packet type: %d\n", buffer[3]);
            printf("Packet data: %s\n\n", &buffer[4]);
        }
    }

    // Close the socket after sending and receiving frames
    close(sockfd);
    return 0;
}