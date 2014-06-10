#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"

#include "demo_net.h"
#include "../demo_config.h"

const shell_command_t shell_commands[] = {
    {"send", "send data through udp", shell_send},
    {NULL, NULL, NULL}
};


int main(void)
{
    /* we assume the transceiver is started! */
    set_channel(DEMO_CHANNEL);
    set_address(DEMO_OWN_ADDRESS);
    start_demo();


    /* start shell */
    posix_open(uart0_handler_pid, 0);
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);

    return 0;
}
