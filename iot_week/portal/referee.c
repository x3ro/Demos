
#include "msg.h"

#include "referee.h"
#include "../common/net.h"
#include "../common/game.h"

#define P1      1
#define P2      -1
#define DRAW    0

int winner(int p1, int p2);


static int np1 = -1;
static int np2 = -1;


void referee_thread(void)
{
    msg_t msg;
    net_cmd_t *cmd;
    net_cmd_t res;

    res.player = 0;
    res.msg = MSG_GAME_SCORE;

    while (1) {
        msg_receive(&msg);
        cmd = (net_cmd_t*)(msg.content.ptr);

        printf("ref: got command %i %i %i\n", cmd->player, cmd->msg, cmd->value);

        if (cmd->msg == MSG_GAME_START) {
            puts("ref: reset state because of GAME_START");
            np1 = -1;
            np2 = -1;
        }
        else if (cmd->msg == MSG_GAME_STATE) {
            if (cmd->player == 1) {
                puts("ref: got PLAYER_1 score");
                np1 = cmd->value;
            }
            else if (cmd->player == 2) {
                puts("ref: got PLAYER_2 score");
                np2 = cmd->value;
            }
            if (np1 != -1 && np2 != -1) {
                puts("ref: we have a winner:");
                switch (winner(np1, np2)) {
                    case P1:
                        puts("     PLAYER_1 WON");
                        res.value = GAME_STATE_PLAYER_1;
                        break;
                    case P2:
                        puts("     PLAYER_2 WON");
                        res.value = GAME_STATE_PLAYER_2;
                        break;
                    default:
                        puts("     nor not: its DRAW");
                        res.value = GAME_STATE_DRAW;
                        break;
                }
                net_send(&res);
            }
        }

        msg_reply(&msg, &msg);
    }
}

int winner(int p1, int p2)
{
    if (p1 == p2) {
        return DRAW;
    }
    if (p1 == GAME_STONE && p2 == GAME_SCISSORS) {
        return P1;
    }
    if (p1 == GAME_STONE && p2 == GAME_PAPER) {
        return P2;
    }
    return (-1) * winner(p2, p1);
}
