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

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"

#include "../common/config.h"
#include "../common/net.h"
#include "../common/netsetup.h"

extern int led_pid;


void net_send(net_cmd_t *cmd)
{
    int size = sizeof(cmd);
    char *data = (char *)cmd;

    netsetup_send_to(CONFIG_NEIGHBOR_P1_TOKEN, data, size);
    netsetup_send_to(CONFIG_NEIGHBOR_P2_TOKEN, data, size);
    netsetup_send_to(CONFIG_NEIGHBOR_P1_LED, data, size);
    netsetup_send_to(CONFIG_NEIGHBOR_P2_LED, data, size);
    netsetup_send_to(CONFIG_NEIGHBOR_PORTAL, data, size);
}

void net_receive(char *data, int length)
{
    net_cmd_t *cmd;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("cmd: player[%i] msg[%i] value[%i]\n", cmd->player, cmd->msg, cmd->value);
    }
}
