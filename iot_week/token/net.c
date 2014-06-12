
#include <stdio.h>
#include <stdlib.h>

#include "msg.h"

#include "../common/config.h"
#include "../common/net.h"
#include "../common/netsetup.h"

extern int game_pid;


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
    msg_t msg;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("net: got command - player: %i; msg: %i; content: %i\n", cmd->player, cmd->msg, cmd->value);

        msg.content.value = (uint32_t)cmd->msg;
        msg_send(&msg, game_pid, 1);
    }
}


// static char *pad(char *content)
// {
//     static char buf[10];
//     strncpy(buf, content, 10);
//     int len = strlen(buf);
//     if ((len % 2) == 1) {
//         /* PAD */
//         buf[len] = 'X';
//         buf[len + 1] = 0;
//     }
//     printf("content with pad: '%s'\n", buf);
//     return buf;
// }

// void demo_send(char *content)
// {
//     netsetup_send_to(CONFIG_NEIGHBOR_P1_TOKEN, pad(content), strlen(content));
//     netsetup_send_to(CONFIG_NEIGHBOR_P2_TOKEN, pad(content), strlen(content));
//     netsetup_send_to(CONFIG_NEIGHBOR_P1_LED, pad(content), strlen(content));
//     netsetup_send_to(CONFIG_NEIGHBOR_P2_LED, pad(content), strlen(content));
//     netsetup_send_to(CONFIG_NEIGHBOR_PORTAL, pad(content), strlen(content));
// }
