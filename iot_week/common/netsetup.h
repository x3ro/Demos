


#ifndef __NETSETUP_H
#define __NETSETUP_H

#include "net_if.h"
#include "sixlowpan.h"
#include "sys/socket.h"
#include "inet_pton.h"


void netsetup_set_address(radio_address_t a);
void netsetup_set_channel(int32_t c);
void netsetup_register_ondata(void (*cb)(char *data, int length));

void netsetup_start(void);

void netsetup_send_to(char *ip, char *content, int length);
void shell_send(int argc, char **argv);

#endif /* __NETSETUP_H */
