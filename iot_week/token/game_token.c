
#include <stdio.h>
#include <stdint.h>

#include "msg.h"
#include "vtimer.h"

#include "acc.h"
#include "game_token.h"
#include "../common/net.h"
#include "../common/game.h"

static int start_sequence[] = GAME_START_SEQ;


void start_seq(void)
{
    int reading = 1;
    msg_t msg;
    net_cmd_t cmd;
    int progress = 0;
    int last_pos = acc_current_pos;

    puts("game: now waiting for start sequence");

    while (reading) {
        if (msg_try_receive(&msg) > 0) {
            if (msg.content.value == MSG_GAME_START) {
                puts("game: got MSG_GAME_START");
                reading = 0;
            }
        }
        else {
            if (start_sequence[progress] == acc_current_pos) {
                printf("game: start_seq %i detected\n", progress);
                last_pos = acc_current_pos;
                ++progress;
                if (progress == GAME_START_SEQ_LEN) {
                    puts("game: detected start sequence, will start game now");
                    cmd.player = PLAYER;
                    cmd.msg = MSG_GAME_STATE;
                    cmd.value = acc_current_pos;
                    net_send(&cmd);

                    progress = 0;
                    reading = 0;
                }
            }
            else if (acc_current_pos != last_pos) {
                progress = 0;
            }
            vtimer_usleep(100 * 1000);
        }
    }
}

void wait_for_go(void)
{
    msg_t msg;
    int waiting = 1;

    while (waiting) {
        msg_receive(&msg);
        if (msg.content.value == MSG_GAME_GO) {
            puts("game: got go signal");
            waiting = 0;
        }
    }
}

void read_pos(void)
{
    net_cmd_t cmd;

    cmd.player = PLAYER;
    cmd.msg = MSG_GAME_STATE;
    cmd.value = (uint8_t)acc_current_pos;
    net_send(&cmd);
}

void idle(void)
{
    msg_t msg;
    int idling = 1;

    do {
        msg_receive(&msg);
        if (msg.content.value == MSG_GAME_OVER);
        idling = 0;
    } while (idling);
}


void game_token_thread(void)
{
    while (1) {
        start_seq();
        wait_for_go();
        read_pos();
        idle();
    }
}
