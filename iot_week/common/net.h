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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

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
