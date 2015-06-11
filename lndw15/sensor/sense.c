/*
 * Copyright (C) 2015 Hauke Petersen <mail@haukepetersen.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @ingroup     lndw15_sensor
 * @{
 *
 * @file
 * @brief       Sensing module
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "kernel.h"
#include "hwtimer.h"
#include "net/ng_netif.h"
#include "net/ng_netapi.h"
#include "periph/adc.h"
#include "net/ng_netbase.h"

#include "demo_config.h"

#define PRIO                (THREAD_PRIORITY_MAIN - 2)

static char stack[THREAD_STACKSIZE_DEFAULT];

static unsigned int rate = CONF_SENSE_RATE;
static int state = 1;

static kernel_pid_t if_pid;
static uint8_t l2hdr[sizeof(ng_netif_hdr_t) + 2];

static uint8_t take_sample(void)
{
    int res = adc_sample(CONF_SENSE_ADC, CONF_SENSE_CHAN);
    /* normalize data a little bit (hacky) */
    res -= 144;
    res = (res * 2) / 6;
    printf("res %6i  - ", res);
    if (res <= 3) {
        res = 0;
    }
    return (uint8_t)(res); /* normalize to 8 bit */
}

void sense_send_data(uint8_t *data)
{
    ng_pktsnip_t *pkt;

    pkt = ng_pktbuf_add(NULL, data, 2, NG_NETTYPE_UNDEF);
    pkt = ng_pktbuf_add(pkt, l2hdr, sizeof(ng_netif_hdr_t) + 2, NG_NETTYPE_NETIF);
    ng_netapi_send(if_pid, pkt);
    LED_RED_TOGGLE;
}

void *sense_thread(void *arg)
{
    (void)arg;
    uint8_t pkt[2] = {CONF_COMM_MSGID, 0};

    while (1) {
        if (state) {
            /* get measurement */
            pkt[1] = take_sample();
            printf("Got sample: %u\n", (unsigned int)pkt[1]);
            /* send data */
            sense_send_data(pkt);
        }
        /* sleep a little bit */
        hwtimer_wait(HWTIMER_TICKS(rate));
    }

    /* never to be reached */
    return NULL;
}

void sense_set_state(int s)
{
    state = s;
}

void sense_set_rate(unsigned int new_rate)
{
    rate = new_rate;
}

void sense_init(kernel_pid_t radio)
{
    ng_netif_hdr_t *hdr = (ng_netif_hdr_t *)l2hdr;
    uint8_t dst_addr[] = CONF_COMM_SCALA_ADDR;

    /* set network interface */
    if_pid = radio;
    /* configure the netif header */
    ng_netif_hdr_init(hdr, 0, 2);
    ng_netif_hdr_set_dst_addr(hdr, dst_addr, 2);
    /* setup the ADC */
    adc_init(CONF_SENSE_ADC, ADC_RES_10BIT);
    /* start the sampling thread */
    thread_create(stack, sizeof(stack), PRIO, 0, sense_thread, NULL, "sense");
}
