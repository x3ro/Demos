/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "shell.h"
#include "board.h"

#include "brain.h"
#include "config.h"

#define SHELL_BUFSIZE       (64U)

static int _readc(void)
{
    return (int)getchar();
}

static void _putc(int c)
{
    putchar((char)c);
}

static const shell_command_t _commands[] = {
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_t shell;
    LED_RED_OFF;

    /* initialize (and run) the brain */
    puts("initializing the brain");
    brain_init();

    /* run the shell for debugging purposes */
    puts("running the shell");
    shell_init(&shell, _commands, SHELL_BUFSIZE, _readc, _putc);
    shell_run(&shell);

    /* should never be reached */
    return 0;
}
