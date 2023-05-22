#ifndef COMMUNICATION_H 
#define COMMUNICATION_H 
#include <stdint.h>

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

struct packet {
    uint8_t start_marker;
    uint8_t size:6;
    uint8_t sequence:4;
    uint8_t type:4;
    uint8_t *data;
    uint8_t parity[8];
};
#endif