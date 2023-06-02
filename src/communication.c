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
#include <time.h>
#include "../lib/communication.h"

double time_passed(clock_t start, clock_t end);

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
    memcpy(p->data, data, size);
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
    if(send(socket, p, sizeof(struct packet), 0) == -1) {
        perror("sendto");
        close(socket);
        exit(EXIT_FAILURE);
    } 
    printf("Packet sent!\n");

    return 0;
}


/* 
 * Listens for a packet of a given type.
 * 
 * @param buffer The buffer to be filled.
 * @param type The type of the packet to be listened.
 * @param socket The socket to be listened.
 * 
 * @return 0 if the packet was received, -1 if not.
*/
int listen_response(struct packet *buffer, uint8_t type, int socket){
    int response_received = 0;

    printf("Waiting for response...\n");

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket, &read_fds);

    struct timeval timeout;
    timeout.tv_usec = 0;

    clock_t start = clock();
    clock_t now = clock();
    
    while(time_passed(start, now) < PT_TIMEOUT && buffer->type != type){
        timeout.tv_sec = PT_TIMEOUT - time_passed(start, now);

        int ready = select(socket + 1, &read_fds, NULL, NULL, &timeout);
        if (ready == -1) {
            perror("select");
            return -1;
        } else if (ready == 0) {
            printf("Timeout expired!\n");
            return -1; // Timeout
        } else {
            ssize_t bytes_received = recvfrom(socket, buffer, sizeof(buffer), 0, NULL, NULL);
            if (bytes_received == -1) {
                perror("recvfrom");
                return -1;
            }
        }
    }

    if(time_passed(start, now) >= PT_TIMEOUT){
        printf("Timeout expired!\n");
        return -1;
    }

    return 0;
}

/* 
 * Checks if a packet is valid.
 * 
 * @param p The packet to be checked.
*/
int is_a_valid_packet(struct packet *p){
    if(p->start_marker != START_MARKER){
        return 0;
    }
    return 1;
}

double time_passed(clock_t start, clock_t end){
    return ((double)(end - start) / CLOCKS_PER_SEC);
}