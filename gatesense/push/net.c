#include <stdio.h>
#include <stdlib.h>

#include "msg.h"

#include "config.h"
#include "net.h"
#include "netsetup.h"

extern int sensor_pid;

void net_send(float relhum, float relhum_temp, float temperature)
{
    static int seq = 0;
    sense_data_t sensor_data;
    sensor_data.id = CONFIG_OWN_ADDRESS;
    sensor_data.seq = seq++;
    sensor_data.humidity = (uint16_t) relhum;
    sensor_data.temperature = (uint16_t) temperature;

    int size = sizeof(sense_data_t);
    char *data = (char *)&sensor_data;
    
    netsetup_send_to(CONFIG_SINK, data, size);
    vtimer_usleep(10 * 1000);
}

void net_receive(char *data, int length)
{
#if 0
    net_cmd_t *cmd;
    msg_t msg;

    if (length == sizeof(net_cmd_t)) {
        cmd = (net_cmd_t*)data;
        printf("net: got command - player: %i; msg: %i; content: %i\n", cmd->player, cmd->msg, cmd->value);

        msg.type = cmd->msg;
        msg.content.value = (uint32_t)cmd->value;
        msg_send(&msg, game_pid, 1);
    }
#endif
}
