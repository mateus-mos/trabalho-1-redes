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
#include "../lib/backup.h"

#define DEST "127.0.0.1"
#define PORT 27015

int main() {
    printf("Creating socket...\n");
    int sockfd = create_socket("lo");
    printf("Socket created!\n");

    backup_single_file("/home/mateus/Documents/BCC/trabalho-1-redes/helloworld.txt", sockfd);




   // uint8_t data[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 'k'};

   // struct packet *p = create_packet(sizeof(data), 0, PT_BACKUP_FILES, data);

   // if(send(sockfd, p, sizeof(struct packet) + sizeof(data), 0) == -1) {
   //     perror("sendto");
   //     close(sockfd);
   //     exit(EXIT_FAILURE);
   // } 

   // printf("Sent packet!");

   // uint8_t data2[] = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '2'};

   // change_packet(p, sizeof(data2), 1, PT_ACK, data2);

   // if(send(sockfd, p, sizeof(struct packet) + sizeof(data2), 0) == -1) {
   //     perror("sendto");
   //     close(sockfd);
   //     exit(EXIT_FAILURE);
   // } 
    // Close the socket after sending and receiving frames
    close(sockfd);

    return 0;
}