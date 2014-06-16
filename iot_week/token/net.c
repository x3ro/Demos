
#include <stdio.h>
#include <stdlib.h>

#include "msg.h"

#include "../common/config.h"
#include "../common/game.h"
#include "../common/net.h"
#include "../common/netsetup.h"

extern int game_pid;


void net_send(net_cmd_t *cmd)
{
    int size = sizeof(net_cmd_t);
    char *data = (char *)cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    netsetup_send_to(CONFIG_N1, data, size);
    netsetup_send_to(CONFIG_N2, data, size);
    netsetup_send_to(CONFIG_N3, data, size);
}

void net_send_start(int s)
{
    net_cmd_t cmd;
    cmd.player = PLAYER;
    cmd.msg = MSG_GAME_START;
    cmd.value = s;

    int size = sizeof(net_cmd_t);
    char *data = (char *)&cmd;

    netsetup_send_to(CONFIG_N0, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N1, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N2, data, size);
    vtimer_usleep(10 * 1000);
    netsetup_send_to(CONFIG_N3, data, size);
    vtimer_usleep(10 * 1000);
}

void net_send_state(int s)
{
    net_cmd_t cmd;
    cmd.player = PLAYER;
    cmd.msg = MSG_GAME_STATE;
    cmd.value = s;

    int size = sizeof(net_cmd_t);
    char *data = (char *)&cmd;

    netsetup_send_to(CONFIG_N3, data, size);
}

void net_receive(char *data, int length)
{
    net_cmd_t *cmd;
    msg_t msg;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("net: got command - player: %i; msg: %i; content: %i\n", cmd->player, cmd->msg, cmd->value);

        msg.type = cmd->msg;
        msg.content.value = (uint32_t)cmd->value;
        msg_send(&msg, game_pid, 1);
    }
}
