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
 * @author      Jonatan Zint <j.zint@fu-berlin.de>
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <unistd.h>
#include <stdio.h>
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"






const shell_command_t shell_commands[] = {
    {NULL, NULL, NULL}
};


int main(void)
{
    shell_t shell;

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}

