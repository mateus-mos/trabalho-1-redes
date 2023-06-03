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
void shift_bits(struct packet *packet);

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
    
    /* Verify Limits*/
    if((size > MAX_DATA_SIZE) || (sequence > MAX_SEQUENCE) || (type > MAX_TYPE))
    {
        perror("Invalid packet!");
        return NULL;
    } 

    struct packet *p;
    p = malloc(sizeof(struct packet));

    if(p == NULL) {
        perror("Malloc failed!");
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
struct packet *change_packet(struct packet *p, uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data){
    /* Verify Limits*/
    if((size > MAX_DATA_SIZE) || (sequence > MAX_SEQUENCE) || (type > MAX_TYPE))
    {
        perror("Invalid packet!");
        return NULL;
    } 

    if(p == NULL) {
        perror("packet is NULL");
        exit(EXIT_FAILURE);
    } 

    p->size = size;
    p->sequence = sequence;
    p->type = type;
    memcpy(p->data, data, size);

    return p;
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

/* Sends a packet and waits for a ACK.
 * If an NACK is received, the packet is sent again.
 * 
 * @param packet The packet to be sent. 
 * @param response The response packet to be filled.
 * @param timeout The timeout in seconds.
 * @param socket The socket to be used.
 * 
 * @return 0 if the packet was sent and the response was received.
 * @return -1 if an error occurred.
 * @return -2 if the timeout expired.
 * 
*/
int send_packet_and_wait_for_response(struct packet *packet, struct packet *response, int timeout, int socket){
    int ACK_received = 0;
    while(!ACK_received){
        send_packet(socket, packet);

        listen_packet(response, timeout, socket); // listen its own packet (LOOPBACK)
        int listen_response = listen_packet(response, timeout, socket);

        if(listen_response == -2) 
            return -2;
        else if(listen_response == -1) 
            return -1;
        if(response->type == PT_ACK)
            ACK_received = 1;
    }

    return 0;
}

int send_packet(int socket, struct packet *packet){
    if(send(socket, packet, sizeof(struct packet), 0) == -1) {
        perror("sendto");
        close(socket);
        exit(EXIT_FAILURE);
    } 
    //shift_bits(packet);
    return 0;
}

/* Shift bits function */
//void shift_bits(struct packet *packet, )
//{
//    int packet_size = packet->size;
//
//    /* Shift sequence */
//    uint8_t shifted= packet->sequence >> 6;
//    packet->size = packet->size | shifted;
//    packet->sequence = packet->sequence << 2;
//
//    /* Shift type */
//    shifted = packet->type >> 2;
//    packet->sequence = packet->sequence | shifted;
//    packet->type = packet->type << 6;
//
//
//}

/* unshift bits */
void unshift_bits(struct packet *packet)
{
    packet->sequence = packet->sequence >> 2;
    int packet_size = packet->size;
    packet->type = packet->type >> 6;
    for(int i = 0; i < packet_size; i++)
        packet->data[i] = packet->data[i] >> 12;
}

/* 
 * Listens for a packet of a given type.
 * 
 * @param buffer The buffer to be filled.
 * @param timeout The timeout in seconds. 
 * @param socket The socket to be listened.
 * 
 * @return 0 if the packet was received.
 * @return -1 if an error occurred. 
 * @return -2 if the timeout expired.
 * 
*/
int listen_packet(struct packet *buffer, int timeout, int socket){
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket, &read_fds);

    struct timeval t_out;
    t_out.tv_usec = 0;

    clock_t start = clock();
    clock_t now = clock();
    
    /* The timeout needs to be checked in the while loop
     * because other packets that it's not the one we want
    * can be received (packets that isn't comming from client). 
    */
    while(time_passed(start, now) < timeout){
        t_out.tv_sec = timeout - time_passed(start, now);

        int ready = select(socket + 1, &read_fds, NULL, NULL, &t_out);
        if (ready == -1) {
            return -1;
        } else if (ready == 0) {
            return -2; // Timeout
        } else {
            ssize_t bytes_received = recvfrom(socket, buffer, sizeof(buffer), 0, NULL, NULL);
            if (bytes_received == -1) {
                return -1;
            }
            /* Checks if the packet is from client. */ 
            if(is_a_valid_packet(buffer))
                return 0;
        }
        now = clock();
    }

    return -2;
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