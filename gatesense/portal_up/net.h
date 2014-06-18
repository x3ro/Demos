/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       Networking helper file
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef __NET_H
#define __NET_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint16_t id;
    uint16_t seq;
    uint32_t temperature;
    uint32_t humidity;
} sense_data_t;

void net_send(float relhum, float relhum_temp, float temperature);

void net_receive(char *from, char *data, int length);

#endif /* __NET_H */
