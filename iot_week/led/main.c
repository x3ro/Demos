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

#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "kernel.h"
#include "thread.h"

#include "../common/config.h"
#include "../common/netsetup.h"
#include "../common/net.h"
#include "led.h"


static char led_stack[KERNEL_CONF_STACKSIZE_MAIN];

int led_pid;

const shell_command_t shell_commands[] = {
    {"send", "send data through udp", shell_send},
    {NULL, NULL, NULL}
};


int main(void)
{
    puts("Game LED Up");

    /* we assume the transceiver is started! */
    netsetup_set_channel(CONFIG_CHANNEL);
    netsetup_set_address(CONFIG_OWN_ADDRESS);
    netsetup_register_ondata(net_receive);
    printf("CHANNEL: %d\tADDRESS: %d\n", CONFIG_CHANNEL, CONFIG_OWN_ADDRESS);
    netsetup_start();

    /* start led and game threads */
    led_pid = thread_create(led_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN - 2, CREATE_STACKTEST,
                            led_thread, "led ctrl");
    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
