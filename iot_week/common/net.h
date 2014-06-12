#ifndef __NET_H
#define __NET_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t player;
    uint8_t msg;
    uint8_t value;
} net_cmd_t;


void net_send(net_cmd_t *cmd);

void net_receive(char *data, int length);

#endif /* __NET_H */
