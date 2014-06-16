/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       LED color and transition control
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdio.h>
#include <math.h>

#include "msg.h"
#include "periph/pwm.h"
#include "rgbled.h"
#include "vtimer.h"

#include "led.h"
#include "color.h"
#include "../common/config.h"
#include "../common/game.h"
#include "../common/net.h"

#define STEPS       (50)
#define PAUSE       (10 * 1000)


static color_rgb_t cd_sequ[GAME_LED_COUNTDOWN_LEN] = GAME_LED_COUNTDOWN;
static color_rgb_t win_sequ[GAME_LED_WIN_LEN] = GAME_LED_WIN;
static color_rgb_t lose_sequ[GAME_LED_LOSE_LEN] = GAME_LED_LOSE;
static color_rgb_t draw_sequ[GAME_LED_DRAW_LEN] = GAME_LED_DRAW;

static uint16_t last_cmd = 0xffff;


static void notify_done(void)
{
    switch (last_cmd) {
        case MSG_GAME_START:
            puts("led: sending GAME_GO message");
            net_send_go();
            break;
        case MSG_GAME_SCORE:
            puts("led: sending GAME_OVER message");
            net_send_over();
            break;
    }
}


void led_thread(void)
{
    msg_t msg;
    int ret;

    color_hsv_t hsv_0;
    color_hsv_t hsv_1;
    color_rgb_t rgb;

    float step_h = 1.0f;
    float step_s = 0.02f;
    float step_v = 0.02f;
    int step = 0;

    int state = 1;
    int limit = 1;
    color_rgb_t *sequ = cd_sequ;

    rgbled_t led;

    /* initialize RGB-LED */
    rgbled_init(&led, PWM_0, 0, 1, 2);

    while (1) {
        /* see if something has come up */
        if (state == limit) {
            notify_done();
            ++state;
        }
        if (state >= limit) {
            ret = msg_receive(&msg);
        } else {
            ret = msg_try_receive(&msg);
        }

        /* if message was receive, act on it */
        if (ret == 1) {
            printf("led: got message %i\n", msg.type);
            if (msg.type != last_cmd) {
                switch (msg.type) {
                    case MSG_GAME_START:
                        puts("led: starting game");
                        sequ = cd_sequ;
                        state = 1;
                        limit = GAME_LED_COUNTDOWN_LEN;
                        last_cmd = msg.type;
                        break;
                    case MSG_GAME_SCORE:
                        printf("led: displaying score: %i\n", (unsigned int)msg.content.value);
                        if (PLAYER == msg.content.value) {
                            puts("led: WIN sequence");
                            sequ = win_sequ;
                            limit = GAME_LED_WIN_LEN;
                        }
                        else if (msg.content.value == GAME_STATE_DRAW) {
                            puts("led: DRAW sequence");
                            sequ = draw_sequ;
                            limit = GAME_LED_DRAW_LEN;
                        }
                        else {
                            puts("led: LOSE sequence");
                            sequ = lose_sequ;
                            limit = GAME_LED_LOSE_LEN;
                        }
                        state = 1;
                        last_cmd = msg.type;
                        break;
                }
            }
        }

        if (state < limit) {
            /* update color */
            if (step == 0) {
                printf("led: setting color fr 0x%02x 0x%02x 0x%02x\n",
                       sequ[state - 1].r, sequ[state - 1].g, sequ[state - 1].b);
                printf("led: setting color to 0x%02x 0x%02x 0x%02x\n",
                       sequ[state].r, sequ[state].g, sequ[state].b);

                color_rgb2hsv(&sequ[state - 1], &hsv_0);
                color_rgb2hsv(&sequ[state ], &hsv_1);

                step_h = (hsv_0.h - hsv_1.h) / STEPS;
                step_s = (hsv_0.s - hsv_1.s) / STEPS;
                step_v = (hsv_0.v - hsv_1.v) / STEPS;
                printf("led: steps are h+= %i s+= %i v+= %i\n", (int)(step_h * 100),(int)(step_s * 100),
                                                                (int)(step_v * 100));
            }

            /* set color */
            color_hsv2rgb(&hsv_0, &rgb);
            rgbled_set(&led, &rgb);
            ++step;
            hsv_0.h += step_h;
            hsv_0.s += step_s;
            hsv_0.v += step_v;

            /* check if transition is finished */
            if (step == STEPS) {
                step = 0;
                ++state;
            }
        }

        /* wait for next step */
        vtimer_usleep(PAUSE);
    }
}
