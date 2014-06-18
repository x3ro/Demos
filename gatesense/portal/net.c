#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "msg.h"

#include "config.h"
#include "net.h"
#include "netsetup.h"

extern int sensor_pid;

void net_receive(char *from, char *data, int length)
{
    sense_data_t *sensor_data;

    if (length == sizeof(sense_data_t)) {
        sensor_data = (sense_data_t *) data;
        printf("net: got sensor data from %s - humidity: %" PRIu32 "; temperature: %" PRIu32 "\n", from, sensor_data->humidity, sensor_data->temperature);
    }
}
