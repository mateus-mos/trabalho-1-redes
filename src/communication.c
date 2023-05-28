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
#include <unistd.h>
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