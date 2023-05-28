#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "../lib/communication.h"


/* 
 * Creates a packet with the given parameters.
 * 
 * @param size The size of the data in the packet.
 * @param sequence The sequence number of the packet.
 * @param type The type of the packet.
 * @param data The data to be sent in the packet.
 * @return A pointer to the created packet.
 * 
 * @see destroy_packet
 * @see change_packet_data
*/
struct packet *create_packet(uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data){
    struct packet *p = malloc(sizeof(struct packet));

    if(p == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    p->start_marker = START_MARKER;
    p->size = size;
    p->sequence = sequence;
    p->type = type;
    if(data != NULL)
        memcpy(&p->data, data, size);

    return p;
}

/* 
 * Changes the data of a packet.
 * 
 * @param p The packet to be changed.
 * @param size The new size of the data in the packet. 
 * @param sequence The new sequence number of the packet. 
 * @param type The new type of the packet. 
 * @param data The new data to be sent in the packet. 
 * 
 * @see create_packet
 * @see destroy_packet
*/
void change_packet(struct packet *p, uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data){
    if(p == NULL) {
        perror("packet is NULL");
        exit(EXIT_FAILURE);
    }

    p->size = size;
    p->sequence = sequence;
    p->type = type;
    memcpy(p->data, data, p->size);
}

/* 
 * Destroys a packet.
 * 
 * @param p The packet to be destroyed.
 * 
 * @see create_packet
*/
void destroy_packet(struct packet *p){
    free(p);
}

/* 
 * Creates a buffer from a packet.
 * 
 * @param p The packet to be converted.
 * @param buffer The buffer to be filled.
 * @return A pointer to the buffer.
 * 
 * @see parse_packet_from_buffer
*/
struct packet *parse_packet_from_buffer(struct packet *p, uint8_t *buffer){
    p->start_marker = buffer[0];
    p->size = buffer[1];
    p->sequence = buffer[2];
    p->type = buffer[3];
    memcpy(&p->data, &buffer[4], p->size);

    return p;
}

int send_packet(int socket, struct packet *p){
    if(send(socket, p, sizeof(struct packet) + p->size, 0) == -1) {
        perror("sendto");
        close(socket);
        exit(EXIT_FAILURE);
    } 
    printf("Packet sent!\n");

    return 0;
}


int listen_response(uint8_t *buffer, uint8_t type, int socket){
    int response_received = 0;

    printf("Waiting for response...\n");

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = PT_TIMEOUT;
    timeout.tv_usec = 0;

    int ready = select(socket , &read_fds, NULL, NULL, &timeout);
    if (ready == -1) {
        perror("select");
        return -1;
    } else if (ready == 0) {
        printf("Timeout expired!\n");
        return 0; // Timeout
    } else {
        ssize_t bytes_received = recvfrom(socket, buffer, sizeof(buffer), 0, NULL, NULL);
        if (bytes_received == -1) {
            perror("recvfrom");
            return -1;
        }
        buffer[bytes_received] = '\0';
        return bytes_received;
    }

    //while(response_received == 0 && timeout_over(start_time) == 0 ){
    //    ssize_t len = recvfrom(socket, buffer, sizeof(buffer), 0, NULL, NULL);

    //    if (len == -1) {
    //        perror("recvfrom");
    //        return 1;
    //    }

    //    if(is_a_valid_packet(buffer)){
    //        if(buffer[3] == type){
    //            response_received = 1;
    //            printf("ACK received!\n");
    //        } 
    //    }
    //}

    //if(response_received == 0){
    //    printf("Timeout!\n");
    //    return 1;
    //}

    return 0;
}

/* 
 * Checks if a packet is valid.
 * 
 * @param p The packet to be checked.
*/
int is_a_valid_packet(uint8_t *buffer){
    if(buffer[0] != START_MARKER){
        return 0;
    }
    return 1;
}