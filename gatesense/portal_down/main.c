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

#include <stdio.h>

#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "kernel.h"
#include "thread.h"

#include "config.h"
#include "netsetup.h"

typedef struct {
    uint16_t id;
    uint16_t seq;
    uint32_t temperature;
    uint32_t humidity;
} sense_data_t;

void sensor_send(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: sensor <temp> <hum>\n");
        return;
    }

    sense_data_t data;
    data.id = 0;
    data.seq = 0;
    data.temperature = atoi(argv[2]);;
    data.humidity = atoi(argv[3]);
    
    netsetup_send_to(CONFIG_LED, &data, sizeof(sense_data_t));
    //netsetup_send_to(CONFIG_LED_2, hum, strlen(argv[2]));
}

const shell_command_t shell_commands[] = {
    {"sensor", "new data to visualize", sensor_send},
    {NULL, NULL, NULL}
};

int main(void)
{
    puts("Portal down Token Up");

    /* we assume the transceiver is started! */
    netsetup_set_channel(CONFIG_CHANNEL);
    netsetup_set_address(CONFIG_OWN_ADDRESS);
    printf("CHANNEL: %d\tADDRESS: %d\n", CONFIG_CHANNEL, CONFIG_OWN_ADDRESS);
    netsetup_start();

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);
}
