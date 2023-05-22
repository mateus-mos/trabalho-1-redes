#include <stdio.h>
#include "../lib/communication.h"

struct packet create_packet(uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data){
    struct packet *p = malloc(sizeof(struct packet));

    p->size = size;
    p->sequence = sequence;
    p->type = type;

}