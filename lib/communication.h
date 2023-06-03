#ifndef COMMUNICATION_H 
#define COMMUNICATION_H 
#define START_MARKER 0x7E

/* Protocol type codes */
#define PT_BACKUP_ONE_FILE 0x0
#define PT_BACKUP_FILES 0x1
#define PT_RESTORE_ONE_FILE 0x2
#define PT_RESTORE_FILES 0x3
#define PT_CHOOSE_SERVER_DIR 0x4
#define PT_VERIFY_BACKUP 0x5
#define PT_RESTORE_FILE 0x6
#define PT_MD5 0x7
#define PT_DATA 0x8
#define PT_END_FILE 0x9
#define PT_END_GROUP_FILES 0xA
#define PT_ERROR 0xC
#define PT_ACK 0xD
#define PT_NACK 0xE

#define PT_TIMEOUT 3 //seconds

#define MAX_SIZE 63
#define MAX_SEQUENCE 63
#define MAX_TYPE 15

struct packet {
    uint8_t start_marker;
    uint8_t size; // 6 bits => 2^6 - 1 = 63 
    uint8_t sequence; // 6 bits => 2^6 - 1 = 63
    uint8_t type; // 4 bits => 2^4 - 1 = 15
    uint8_t data[63]; // Max length of data is 63 bytes
    //uint8_t parity; // 8 bytes for parity
};

struct packet *create_packet(uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data);

void change_packet(struct packet *p, uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data);

int is_a_valid_packet(struct packet *p);

int send_packet(int socket, struct packet *p);
int listen_packet(struct packet *buffer, int timeout, int socket);

void destroy_packet(struct packet *p);

#endif