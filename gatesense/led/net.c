#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "msg.h"

#include "config.h"
#include "net.h"
#include "netsetup.h"

extern int led_pid;

void net_receive(char *data, int length)
{
    sense_data_t *sensor_data;
    msg_t msg;

    if (length == sizeof(sense_data_t)) {
        sensor_data = (sense_data_t *) data;
        printf("net: got sensor data - humidity: %" PRIu32 "; temperature: %" PRIu32 "\n", sensor_data->humidity, sensor_data->temperature);

        int32_t val = 0;
        if (NODE_TYPE == NODE_LED_HUMIDITY) {
            val = (sensor_data->humidity/1000) - 40 /* 40% rel hum offset */;
        } else if (NODE_TYPE == NODE_LED_TEMPERATURE) {
            val = (sensor_data->temperature/1000) - 20 /* 20 grad offset */;
        }
        if (val < 0) val = 0;
         
        msg.content.value = (uint32_t) val;
        msg.type = 1;
        msg_send(&msg, led_pid, 1);
    }
}
