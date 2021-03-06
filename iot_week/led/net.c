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
#include "vtimer.h"

#include "../common/config.h"
#include "../common/game.h"
#include "../common/net.h"
#include "../common/netsetup.h"

extern int led_pid;


void net_send(net_cmd_t *cmd)
{
    int size = sizeof(net_cmd_t);
    char *data = (char *)cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    netsetup_send_to(CONFIG_N1, data, size);
    netsetup_send_to(CONFIG_N2, data, size);
    netsetup_send_to(CONFIG_N3, data, size);
}

void net_send_go(void)
{
    net_cmd_t cmd;
    cmd.msg = MSG_GAME_GO;
    cmd.player = PLAYER;

    int size = sizeof(net_cmd_t);
    char *data = (char *)&cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N1, data, size);
}

void net_send_over(void)
{
    net_cmd_t cmd;
    cmd.msg = MSG_GAME_OVER;
    cmd.player = PLAYER;

    int size = sizeof(net_cmd_t);
    char *data = (char *)&cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N1, data, size);
}

void net_receive(char *data, int length)
{
    net_cmd_t *cmd;
    msg_t msg;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("net: got command - player: %i; msg: %i; content: %i\n", cmd->player, cmd->msg, cmd->value);

        msg.type = (uint16_t)cmd->msg;
        msg.content.value = (uint32_t)cmd->value;
        printf("net: sending msg to %i \n", led_pid);
        msg_send(&msg, led_pid, 1);
    }
}
