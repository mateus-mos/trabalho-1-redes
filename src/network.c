#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "../lib/network.h"
#include "../lib/log.h"

double time_passed(clock_t start, clock_t end);
void shift_bits(struct packet *packet);

uint8_t calculate_vertical_parity(uint8_t packet_data[], size_t length) 
{
    uint8_t parity = 0;
    int odd = -1;

    for (size_t i = 0; i < length; i++) 
    {
        for(unsigned int j = 0; j < length; j++)
        {
            if((1U << i) & packet_data[j])
            { 
                odd *= -1;
            }
        }
        uint8_t byte_parity = odd == 1? 1 : 0;
        parity = parity | (byte_parity << (i % 8));
        odd = -1;
    }

    return parity;
}

/* Verify packet parameters */
int verify_packet_parameters(uint8_t size, uint8_t sequence, uint8_t type) 
{
    if ((size > MAX_DATA_SIZE) || (sequence > MAX_SEQUENCE) || (type > MAX_TYPE)) 
        return 0;

    return 1;
}

/* 
 * Creates or modifies a packet with the given parameters.
 * 
 * @param packet The packet pointer, if NULL creates a new packet, otherwise modifies the existing packet.
 * @param size The size of the data in the packet.
 * @param sequence The sequence number of the packet.
 * @param type The type of the packet.
 * @param data The data to be sent in the packet.
 * @return A pointer to the created or modified packet.
 * 
 * @see destroy_packet
*/
struct packet *create_or_modify_packet(struct packet *packet, uint8_t size, uint8_t sequence, uint8_t type, void *data) 
{
    if (!verify_packet_parameters(size, sequence, type)) 
    {
        perror("Invalid packet!");
        return NULL;
    }

    if (packet == NULL) 
    {
        packet = calloc(1, sizeof(struct packet));
        if (packet == NULL) 
        {
            perror("Alloc failed!");
            exit(EXIT_FAILURE);
        }
    }

    packet->start_marker = START_MARKER;
    packet->size = size;
    packet->sequence = sequence;
    packet->type = type;
    packet->parity = 0;

    if (data != NULL) 
    {
        memcpy(&packet->data, data, size);
        packet->parity = calculate_vertical_parity(data, size);
    }

    return packet;
}

/* 
 * Destroys a packet.
 * 
 * @param p The packet to be destroyed.
 * 
 * @see create_or_modify_packet
*/
void destroy_packet(struct packet *p) 
{
    free(p);
}

int send_packet(struct packet *packet, int socket)
{
    if(send(socket, packet, sizeof(struct packet), 0) == -1) 
    {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    } 
    //shift_bits(packet);
    return 0;
}

/* Sends a packet and waits for a ACK or OK.
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
int send_packet_and_wait_for_response(struct packet *packet, struct packet *response, int timeout, int socket)
{
    int ACK_OK_received = 0;
    struct packet *response_aux = create_or_modify_packet(NULL, 0, 0, PT_ACK, NULL);

    while(!ACK_OK_received)
    {
        send_packet(packet, socket);

        //log_message("Packet sent, waiting for response...");
        int listen_response = listen_packet(response_aux, timeout, socket);
        //log_message("Response received!"

    
        if(listen_response == -1) 
            return -1;
        else if(response->type == PT_ACK || response->type == PT_OK || response->type == PT_ERROR)
            ACK_OK_received = 1;
    }

    create_or_modify_packet(response, response_aux->size, response_aux->sequence, response_aux->type, response_aux->data);

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
    int packet_size = packet->size; // ?
    packet->type = packet->type >> 6;

    for(int i = 0; i < packet_size; i++)
        packet->data[i] = packet->data[i] >> 12;
}

/* clear buffer */
void clear_buffer(struct packet *packet)
{
    memset(packet, 0, sizeof(struct packet));
}

/* 
 * Listens for a valid packet.
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
int listen_packet(struct packet *buffer, int timeout, int socket)
{
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
    while(time_passed(start, now) < timeout)
    {
        clear_buffer(buffer);
        t_out.tv_sec = timeout - time_passed(start, now);

        int ready = select(socket + 1, &read_fds, NULL, NULL, &t_out);
        
        if (ready == -1) 
        {
            return -1;
        } 
        else if (ready == 0) 
        {
            return -2; // Timeout
        } 
        else 
        {
            ssize_t bytes_received = recvfrom(socket, buffer, sizeof(struct packet), 0, NULL, NULL);
            
            if (bytes_received == -1) 
                return -1;

            /* Checks if the packet is from client and if it's parity is right  */ 
            if(is_a_valid_packet(buffer) == 0)
            {
                struct packet *nack = create_or_modify_packet(NULL, 0, 0, PT_NACK, NULL);
                send_packet(nack, socket);
                destroy_packet(nack);
            }
            else{
                return 0;
            }
        }
        now = clock();
    }
    return -2; 
}


/* 
 * Checks if the start marker is correct and if the parity is correct.
 * 
 * @param p The packet to be checked.
*/
int is_a_valid_packet(struct packet *p)
{
    if(p->start_marker != START_MARKER)
        return 0;
    if(p->parity != calculate_vertical_parity(p->data, p->size))
        return 0;
    
    return 1;
}

double time_passed(clock_t start, clock_t end)
{
    return ((double)(end - start) / CLOCKS_PER_SEC);
}