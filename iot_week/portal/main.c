/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       Main bootstrapping and shell setup
 *
 * @author      Christian Mehlis <christian.mehlis@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>

#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"

#include "../common/config.h"
#include "../common/netsetup.h"
#include "../common/net.h"

void forward(int argc, char **argv);

const shell_command_t shell_commands[] = {
    {"send", "send data through udp", shell_send},
    {"fw", "forward data into WSN", forward},
    {NULL, NULL, NULL}
};


void forward(int argc, char **argv)
{
    net_cmd_t cmd;

    if (argc == 3) {
        cmd.player = atoi(argv[0]);
        cmd.msg = atoi(argv[1]);
        cmd.value = atoi(argv[2]);
        net_send(&cmd);
    }
}


int main(void)
{
    puts("Game PORTAL Up");

    /* we assume the transceiver is started! */
    netsetup_set_channel(CONFIG_CHANNEL);
    netsetup_set_address(CONFIG_OWN_ADDRESS);
    netsetup_register_ondata(net_receive);
    printf("CHANNEL: %d\tADDRESS: %d\n", CONFIG_CHANNEL, CONFIG_OWN_ADDRESS);
    netsetup_start();

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}

