#include <stdio.h>
#include <stdlib.h>

#include "msg.h"

#include "config.h"
#include "net.h"
#include "netsetup.h"

extern int sensor_pid;

void net_receive(char *data, int length)
{
    sense_data_t *sensor_data;
    msg_t msg;

    if (length == sizeof(sense_data_t)) {
        sensor_data = (sense_data_t *) data;
        printf("net: got sensor data - humidity: %i; temperature: %i\n", sensor_data->humidity, sensor_data->temperature);
    }
}
