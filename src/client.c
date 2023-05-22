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
#include "../lib/communication.h"

#define DEST "127.0.0.1"
#define PORT 27015

int main() {
    printf("Creating socket...\n");
    int sockfd = create_socket("lo");
    printf("Socket created!\n");

    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 'a', 'b', 'c', 'd'};

    struct packet *p = create_packet(sizeof(data), 0, PT_BACKUP_FILES, data);

    if(send(sockfd, p, 4 + sizeof(data), 0) == -1) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent packet!");
    // Close the socket after sending and receiving frames
    close(sockfd);

    return 0;
}