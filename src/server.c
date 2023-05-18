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

#define PORT 27015 

int main() {

    printf("Creating socket...\n");
    int sockfd = createSocket("lo");
    printf("Socket created!\n");

    char buffer[2048];
    while (1) {
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (len == -1) {
            perror("recvfrom");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("Received packet: %s\n", buffer);
    }

    // Close the socket after sending and receiving frames
    close(sockfd);
    return 0;
}