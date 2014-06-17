/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Posix UDP Server/Client example
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */
 
#include <stdint.h>

#include "vtimer.h"
#include "sht11.h"

#include "net.h"

int sensor_read(sht11_val_t *sht11_val)
{

    return sht11_read_sensor(sht11_val, HUMIDITY | TEMPERATURE);
}

void sensor_thread(void)
{
    while (1) {
        sht11_val_t sht11_val;
        int success = sensor_read(&sht11_val);
        
        printf("Relative humidity: %5.2f%% / Temperature compensated humidity; %5.2f%% ",
               (double) sht11_val.relhum, (double) sht11_val.relhum_temp);
        printf("Temperature: %-6.2f°C\n", (double) sht11_val.temperature);

        net_send(sht11_val.relhum, sht11_val.relhum_temp, sht11_val.temperature);
    
        vtimer_usleep(100 * 1000);
    }
}

