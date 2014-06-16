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

extern int ref_pid;


void net_send(net_cmd_t *cmd)
{
    int size = sizeof(cmd);
    char *data = (char *)cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N1, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N2, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N3, data, size);
}

void net_receive(char *data, int length)
{
    net_cmd_t *cmd;
    msg_t msg;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("cmd: player[%i] msg[%i] value[%i]\n", cmd->player, cmd->msg, cmd->value);

        msg.type = cmd->msg;
        msg.content.ptr = (char *)cmd;
        msg_send_receive(&msg, &msg, ref_pid);
    }
}
