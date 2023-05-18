#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../lib/socket.h"

#define DEST "127.0.0.1"
#define PORT 27015

int main() {
    printf("Creating socket...\n");
    int sockfd = createSocket("lo");
    printf("Socket created!\n");

    char message[] = "Hello, Server!";
    if(send(sockfd, message, sizeof(message), 0) == -1) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent packet: %s\n", message);
    // Close the socket after sending and receiving frames
    close(sockfd);

    return 0;
}