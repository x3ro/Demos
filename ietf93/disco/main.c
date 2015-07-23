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
#include "periph/gpio.h"
#include "net/ng_netbase.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DISCO_PIN           GPIO(PA, 6)
#define DISCO_CHANNEL       (11U)
#define DISCO_PORT          (23000U)

#define QUEUE_SIZE          (10U)
#define STACKSIZE           (THREAD_STACKSIZE_MAIN)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)

char stack[STACKSIZE];


void on_data(uint8_t *data, size_t len)
{
    DEBUG("got data\n");
    if (len >= 5 && memcmp(data, "party", 5) == 0) {
        puts("It's party time...");
        gpio_set(DISCO_PIN);
    }
    if (len >= 4 && memcmp(data, "lame", 4) == 0) {
        puts("Lame!");
        gpio_clear(DISCO_PIN);
    }
}

void *disco(void *arg)
{
    (void)arg;
    msg_t msg, queue[QUEUE_SIZE];
    ng_netreg_entry_t reg;

    msg_init_queue(queue, QUEUE_SIZE);

    reg.pid = thread_getpid();
    reg.demux_ctx = DISCO_PORT;
    ng_netreg_register(NG_NETTYPE_UDP, &reg);

    while (1) {
        msg_receive(&msg);

        DEBUG("got netapi message\n");
        if (msg.type == NG_NETAPI_MSG_TYPE_RCV) {
            ng_pktsnip_t *pkt = (ng_pktsnip_t *)msg.content.ptr;
            on_data((uint8_t *)(pkt->data), pkt->size);
            ng_pktbuf_release(pkt);
        }
    }

    /* never reached */
    return NULL;
}

void set_chan(uint16_t channel)
{
    kernel_pid_t netifs[NG_NETIF_NUMOF];

    ng_netif_get(netifs);
    ng_netapi_set(netifs[0], NETCONF_OPT_CHANNEL, 0, (void *)&channel, 2);
}

int main(void)
{
    shell_t shell;


    gpio_init(DISCO_PIN, GPIO_DIR_OUT, GPIO_NOPULL);
    thread_create(stack, STACKSIZE, PRIO, 0, disco, NULL, "disco");

    set_chan(DISCO_CHANNEL);

    shell_init(&shell, NULL, STDIO_RX_BUFSIZE, getchar, putchar);
    shell_run(&shell);

    return 0;
}
