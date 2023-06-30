#ifndef NETWORK_H 
#define NETWORK_H 
#define START_MARKER 0x7E

/* Protocol type codes */
#define PT_BACKUP_ONE_FILE 0x0
#define PT_BACKUP_MULTIPLE_FILES 0x1
#define PT_RESTORE_ONE_FILE 0x2
#define PT_RESTORE_FILES 0x3
#define PT_SET_SERVER_DIR 0x4
#define PT_VERIFY_BACKUP 0x5
#define PT_RESTORE_FILE 0x6
#define PT_MD5 0x7
#define PT_DATA 0x8
#define PT_END_FILE 0x9
#define PT_END_GROUP_FILES 0xA
#define PT_ERROR 0xC
#define PT_OK 0xD
#define PT_ACK 0xE
#define PT_NACK 0xF
#define PT_TIMEOUT 5 //seconds

#define MAX_PACKET_SIZE 72
#define MAX_DATA_SIZE 63
#define MAX_SEQUENCE 15 
#define MAX_TYPE 15

struct packet {
    uint8_t start_marker; // 1 byte
    uint8_t size; // 6 bits
    uint8_t sequence; // 6 bits 
    uint8_t type; // 4 bits
    uint8_t parity; // 1 byte for parity
    uint8_t data[63]; // Max length of data is 63 bytes
};

int is_a_valid_packet(struct packet *p);

int send_packet_and_wait_for_response(struct packet *packet, struct packet *response, int timeout, int socket);
int send_packet(struct packet *p, int socket);
int listen_packet(struct packet *buffer, int timeout, int socket);
struct packet *create_or_modify_packet(struct packet *packet, uint8_t size, uint8_t sequence, uint8_t type, void *data); 
void destroy_packet(struct packet *p);

#endif