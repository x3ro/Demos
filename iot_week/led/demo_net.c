/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Posix UDP Server/Client example
 *
 * @author      Jonatan Zint <j.zint@fu-berlin.de>
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <unistd.h>
#include <stdio.h>
#include "kernel.h"
#include "thread.h"

#include "demo_net.h"
#include "../demo_config.h"


#define MONITOR_STACK_SIZE  (KERNEL_CONF_STACKSIZE_MAIN)
#define IF_ID   (0)
#define RCV_BUFFER_SIZE     (32)
#define SERVER_PORT 8443
#define UDP_BUFFER_SIZE     (128)


char udp_server_stack_buffer[KERNEL_CONF_STACKSIZE_MAIN];
char addr_str[IPV6_MAX_ADDR_STR_LEN];


static void init_udp_server(void);

void set_address(radio_address_t a)
{
    msg_t mesg;
    transceiver_command_t tcmd;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = SET_ADDRESS;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

void set_channel(int32_t c)
{
    msg_t mesg;
    transceiver_command_t tcmd;

    if (transceiver_pid < 0) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &c;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = SET_CHANNEL;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

static void demo_send_to(char *ip, char *content)
{
    ipv6_addr_t dest;
    sockaddr6_t sa;
    int len, sock;

    if (!inet_pton(AF_INET6, ip, &dest)) {
        printf("ERROR: %s is not a valid IPv6 address\n", ip);
        return;
    }

    /* neighbour discovery protocol buggy in RIOT, so we need to add
       neighbours manually. */
    if (!ndp_neighbor_cache_search(&dest)) {
        ndp_neighbor_cache_add(IF_ID, &dest, &(dest.uint16[7]), 2, 0,
                               NDP_NCE_STATUS_REACHABLE,
                               NDP_NCE_TYPE_TENTATIVE,
                               0xffff);
    }


    sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock) {
        printf("Error Creating Socket!");
        return;
    }

    memset(&sa, 0, sizeof sa);
    sa.sin6_family = AF_INET;
    memcpy(&sa.sin6_addr, &dest, 16);
    sa.sin6_port = HTONS(SERVER_PORT);

    len = sendto(sock, content, strlen(content), 0, (struct sockaddr*) &sa,
                   sizeof sa);
    if (len < 0) {
        printf("Error sending packet!\n");
    } else {
        printf("Successful deliverd %i bytes over UDP to %s to 6LoWPAN\n",
               len, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &dest));
    }

     close(sock);
}

static char *pad(char *content)
{
    static char buf[10];
	strncpy(buf, content, 10);
	int len = strlen(buf);
	if ((len % 2) == 1) {
		/* PAD */
		buf[len] = 'X';
		buf[len + 1] = 0;
	}
	printf("content with pad: '%s'\n", buf);
	return buf;
}

void demo_send(char *content)
{
	demo_send_to(DEMO_NEIGHBOR_1, pad(content));
	demo_send_to(DEMO_NEIGHBOR_2, pad(content));
	demo_send_to(DEMO_NEIGHBOR_3, pad(content));
}

void shell_send(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: send <ipv6address> <msg>\n");
        return;
    }

    demo_send_to(argv[1], argv[2]);
}

void start_demo(void)
{
    ipv6_addr_t ipaddr;
    net_if_addr_t *addr_ptr = NULL;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    int udp_server_thread_pid;

    /* configure link-local address */
    ipv6_addr_set_link_local_prefix(&ipaddr);

    if (!ipv6_addr_set_by_eui64(&ipaddr, IF_ID, &ipaddr)) {
        printf("Can not set link-local by EUI-64 on interface %d\n", IF_ID);
        return;
    }

    if (!ipv6_net_if_add_addr(IF_ID, &ipaddr, NDP_ADDR_STATE_PREFERRED,
                NDP_OPT_PI_VLIFETIME_INFINITE,
                NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        printf("Can not add link-local address to interface %d\n", IF_ID);
        return;
    }

    /* Start UDP Server */
    udp_server_thread_pid = thread_create(udp_server_stack_buffer,
            KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_STACKTEST,
            init_udp_server, "init_udp_server");
    printf("UDP SERVER ON PORT %d (THREAD PID: %d)\n", HTONS(SERVER_PORT),
            udp_server_thread_pid);

    /* Print configured IPv6-Addresses */
    printf("Already configured IPv6-Addresses:\n");
    while (net_if_iter_addresses(0, &addr_ptr)) {
        if (inet_ntop(AF_INET6, addr_ptr->addr_data, addr_str,
                      IPV6_MAX_ADDR_STR_LEN)) {
            printf("\t%s/%d\n", addr_str, addr_ptr->addr_len);
        }
    }

}

static void init_udp_server(void)
{
    sockaddr6_t sa;
    char buffer_main[UDP_BUFFER_SIZE];
    int recsize;
    uint32_t fromlen;
    int sock, status;

    sock  = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    memset(&sa, 0, sizeof(sa));
    sa.sin6_family = AF_INET;
    sa.sin6_port = HTONS(SERVER_PORT);

    fromlen = sizeof(sa);

    status = bind(sock, (struct sockaddr *)&sa, sizeof(sa));
    if (-1 == status) {
        printf("Error bind failed!\n");
        /* missing not exposed close() function
         close(sock) */;
    }
    while (1) {
        recsize = recvfrom(sock, (void *)buffer_main, UDP_BUFFER_SIZE, 0,
                             (struct sockaddr *)&sa, &fromlen);

        if (recsize < 0) {
            printf("ERROR: recsize < 0!\n");
            break;
        }

        printf("UDP packet received, payload: '%s' size: %d\n", buffer_main, recsize);
    }

    printf("ERROR: stopping udp server!\n");
    close(sock);
}

