/*
 * Copyright (C) 2015 Hauke Petersen <mail@haukepetersen.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @defgroup    lndw15_sensor Plant Sensor Application
 * @brief       Firmware for the plant sensor demo
 * @{
 *
 * @file
 * @brief       Bootstrap the communication module and the shell
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "board_uart0.h"
#include "posix_io.h"
#include "shell.h"
#include "kernel.h"

#include "comm.h"
#include "sense.h"
#include "demo_config.h"

#define SHELL_BUFSIZE           (64U)

static int cmd_sense(int argc, char **argv)
{
    int state;

    if (argc < 2) {
        printf("usage: %s <0|1>\n", argv[0]);
        return 1;
    }

    state = atoi(argv[1]);
    if (state == 1 || state == 0) {
        sense_set_state(state);
    }
    return 1;
}

static int cmd_send(int argc, char **argv)
{
    uint8_t data[2] = {CONF_COMM_MSGID, 0};

    if (argc < 2) {
        printf("usage: %s <val> (0 <= val <=255)\n", argv[0]);
        return 1;
    }

    data[1] = (uint8_t)atoi(argv[1]);
    sense_send_data(data);
    return 0;
}

static int cmd_rate(int argc, char **argv)
{
    unsigned int rate;

    if (argc < 2) {
        printf("usage: %s <rate>\n", argv[0]);
        return 1;
    }

    rate = (unsigned int)atoi(argv[1]);
    sense_set_rate(rate);
    return 0;
}

static int _readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void _putc(int c)
{
    putchar((char)c);
}

/**
 * @brief   Define some shell commands for testing the brain
 */
static const shell_command_t _commands[] = {
    { "sense", "turn sensing on or off", cmd_sense },
    { "send", "a test value", cmd_send },
    { "rate", "adjust the sensing rate", cmd_rate },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;
    kernel_pid_t radio;

    /* initialize the radio */
    radio = comm_init();
    if (radio == KERNEL_PID_UNDEF) {
        puts("ERROR initializing the radio, aborting");
        return 1;
    }

    /* initialize the sensing module */
    sense_init(radio);

    /* run the shell */
    (void) posix_open(uart0_handler_pid, 0);
    shell_init(&shell, _commands, SHELL_BUFSIZE, _readc, _putc);
    shell_run(&shell);

    /* never reached */
    return 0;
}
