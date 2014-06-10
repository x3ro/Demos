#ifndef DEMO_NET_H_
#define DEMO_NET_H_

#include "net_if.h"
#include "sixlowpan.h"
#include "sys/socket.h"
#include "inet_pton.h"

void start_demo(void);
void set_address(radio_address_t a);
void set_channel(int32_t c);

void shell_send(int argc, char **argv);
void demo_send(char *content);

#endif /* DEMO_NET_H_ */
