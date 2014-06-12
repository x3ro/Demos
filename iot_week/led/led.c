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
#include "../common/game.h"
#include "../common/net.h"

#define STEPS       (50)
#define PAUSE       (10 * 1000)


static color_rgb_t cd_sequ[GAME_LED_COUNTDOWN_LEN] = GAME_LED_COUNTDOWN;
static color_rgb_t win_sequ[GAME_LED_WIN_LEN] = GAME_LED_WIN;
static color_rgb_t lose_sequ[GAME_LED_LOSE_LEN] = GAME_LED_LOSE;

static uint16_t last_cmd;


static void notify_done(void)
{
    net_cmd_t cmd;

    cmd.player = PLAYER;
    cmd.value = 0;

    switch (last_cmd) {
        case MSG_GAME_START:
            cmd.msg = MSG_GAME_GO;
            net_send(&cmd);
            break;
        case MSG_GAME_SCORE:
            cmd.msg = MSG_GAME_OVER;
            net_send(&cmd);
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
            ret = msg_receive(&msg);
        } else {
            ret = msg_try_receive(&msg);
        }

        /* if message was receive, act on it */
        if (ret == 1) {
            if (msg.type != last_cmd) {
                switch (msg.type) {
                    case MSG_GAME_START:
                        sequ = cd_sequ;
                        state = 1;
                        limit = GAME_LED_COUNTDOWN_LEN;
                        last_cmd = msg.type;
                        break;
                    case MSG_GAME_SCORE:
                        if (PLAYER & msg.content.value) {
                            sequ = win_sequ;
                            limit = GAME_LED_WIN_LEN;
                        }
                        else {
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
                color_rgb2hsv(&sequ[state - 1], &hsv_0);
                color_rgb2hsv(&sequ[state ], &hsv_1);
                step_h = (hsv_0.h - hsv_1.h) / STEPS;
                step_s = (hsv_0.s - hsv_1.s) / STEPS;
                step_v = (hsv_0.v - hsv_1.v) / STEPS;
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
                notify_done();
            }
        }

        /* wait for next step */
        vtimer_usleep(PAUSE);
    }
}
