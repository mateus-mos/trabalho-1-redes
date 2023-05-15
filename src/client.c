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
#include "../lib/ConexaoRawSocket.h"

int main() {
    //// Create a raw socket using the AF_PACKET address family, SOCK_RAW type, and htons(ETH_P_ALL) protocol
    //// AF_PACKET allows for direct access to the network interface at the packet level
    //// SOCK_RAW provides access to the underlying transport provider
    //// ETH_P_ALL specifies that all Ethernet frames will be captured
    //int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    //if (sockfd == -1) {
    //    perror("socket");
    //    exit(EXIT_FAILURE);
    //}

    //// Bind the socket to a specific network interface, e.g., "eth0"
    //// This is done using the setsockopt function with the SO_BINDTODEVICE option
    //struct ifreq ifr;
    //memset(&ifr, 0, sizeof(ifr));
    //strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    //if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
    //    perror("setsockopt");
    //    exit(EXIT_FAILURE);
    //}
    int sockfd = ConexaoRawSocket("lo");


    // Construct the ethernet frame with destination MAC address, source MAC address, ethertype, payload, and checksum
    // MAC addresses are 6 bytes long and are represented as uint8_t arrays
    // Ethertype is a 2-byte value that specifies the protocol used in the payload
    // Payload is the data being sent, in this case, a "Hello, World!" string
    // Checksum is a 4-byte value used for error detection
    uint8_t src_mac[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t dst_mac[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint16_t ethertype = htons(0x0801);
    char payload[] = "Hello, World!";
    uint32_t checksum = htonl(0x1a2b3c4d);

    // Assemble the frame by copying each part into a contiguous memory block
    uint8_t frame[sizeof(src_mac) + sizeof(dst_mac) + sizeof(ethertype) + sizeof(payload) + sizeof(checksum)];
    memcpy(frame, dst_mac, sizeof(dst_mac));
    memcpy(frame + sizeof(dst_mac), src_mac, sizeof(src_mac));
    memcpy(frame + sizeof(dst_mac) + sizeof(src_mac), &ethertype, sizeof(ethertype));
    memcpy(frame + sizeof(dst_mac) + sizeof(src_mac) + sizeof(ethertype), payload, sizeof(payload));
    memcpy(frame + sizeof(dst_mac) + sizeof(src_mac) + sizeof(ethertype) + sizeof(payload), &checksum, sizeof(checksum));

    // Send the ethernet frame using the send() function
    // The frame is sent as a sequence of bytes, and the size of the frame is passed as an argument
    ssize_t sent = send(sockfd, frame, sizeof(frame), 0);
    if (sent == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Receive an ethernet frame using the recv() function
    // The received frame will be stored in a buffer (recv_frame) with a specified size (4096 bytes)
    // recv_len holds the actual length of the received frame
    uint8_t recv_frame[4096];
    ssize_t recv_len = recv(sockfd, recv_frame, sizeof(recv_frame), 0);
    if (recv_len == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    // Close the socket after sending and receiving frames
    close(sockfd);

    return 0;
}