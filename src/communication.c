#include <stdlib.h>
#include <string.h>
#include "../lib/communication.h"

struct packet *create_packet(uint8_t size, uint8_t sequence, uint8_t type, uint8_t *data){
    struct packet *p = malloc(sizeof(struct packet) + size * sizeof(uint8_t));

    /* Process data if necessary */

    p->start_marker = START_MARKER;
    p->size = size;
    p->sequence = sequence;
    p->type = type;
    memcpy(&p->data, data, size);

    return p;
}