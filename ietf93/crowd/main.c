/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     demo_ietf93_disco
 * @{
 *
 * @file
 * @brief       Disco application
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "board.h"
#include "thread.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_udp.h"
#include "net/ng_netbase.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DISCO_ADDR          "fe80::5855:6367:f925:a99a"
#define DISCO_CHANNEL       (11U)
#define DISCO_PORT          (23000U)

static uint8_t port[2];
static ng_ipv6_addr_t addr;

extern int udp_cmd(int argc, char **argv);


static void udp_send(const char *str)
{
    uint8_t data[20];
    ng_pktsnip_t *payload, *udp, *ip;
    ng_netreg_entry_t *sendto;

    memcpy(data, str, strlen(str));

    /* allocate payload */
    payload = ng_pktbuf_add(NULL, data, strlen(str), NG_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return;
    }
    /* allocate UDP header, set source port := destination port */
    udp = ng_udp_hdr_build(payload, port, 2, port, 2);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        ng_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header */
    ip = ng_ipv6_hdr_build(udp, NULL, 0, (uint8_t *)&addr, sizeof(addr));
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        ng_pktbuf_release(udp);
        return;
    }
    /* send packet */
    sendto = ng_netreg_lookup(NG_NETTYPE_UDP, NG_NETREG_DEMUX_CTX_ALL);
    if (sendto == NULL) {
        puts("Error: unable to locate UDP thread");
        ng_pktbuf_release(ip);
        return;
    }
    ng_pktbuf_hold(ip, ng_netreg_num(NG_NETTYPE_UDP,
                                     NG_NETREG_DEMUX_CTX_ALL) - 1);
    while (sendto != NULL) {
        ng_netapi_send(sendto->pid, ip);
        sendto = ng_netreg_getnext(sendto);
    }
    printf("Send %s\n", str);
}

static void set_chan(uint16_t channel)
{
    kernel_pid_t netifs[NG_NETIF_NUMOF];

    ng_netif_get(netifs);
    ng_netapi_set(netifs[0], NETCONF_OPT_CHANNEL, 0, (void *)&channel, 2);
}

static int cmd_party(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("PARTY");
    udp_send("party");
    return 0;
}

static int cmd_lame(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("lame");
    udp_send("lame");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "party", "start the party", cmd_party },
    { "lame", "be lame", cmd_lame },
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;
    const char *disco_addr = DISCO_ADDR;

    port[0] = (uint8_t)DISCO_PORT;
    port[1] = (uint8_t)(DISCO_PORT >> 8);
    ng_ipv6_addr_from_str(&addr, disco_addr);

    char buf[100];
    ng_ipv6_addr_to_str(buf, &addr, 100);
    printf("got address: %s\n", buf);


    set_chan(DISCO_CHANNEL);

    shell_init(&shell, shell_commands, STDIO_RX_BUFSIZE, getchar, putchar);
    shell_run(&shell);

    return 0;
}
