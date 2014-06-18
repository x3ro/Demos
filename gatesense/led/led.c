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
#include "config.h"
#include "net.h"


#define TEMP_MAX        (35000.0f)
#define TEMP_MIN        (20000.0f)
#define HUM_MAX         (95000.0f)
#define HUM_MIN         (35000.0f)

#define HUE_MAX         (310.0f)


void update_hsv(color_hsv_t *hsv, float val, int type)
{
    if (type == NODE_LED_TEMPERATURE) {
        val -= TEMP_MIN;
        val = (TEMP_MAX - TEMP_MIN) / val;
    }
    else if (type == NODE_LED_HUMIDITY) {
        val -= HUM_MIN;
        val = (HUM_MAX - HUM_MIN) / val;
    }
    else {
        return;
    }

    if (val < 0.0f) {
        val = 0.0f);
    }
    else if (val > 1.0f) {
        val = 1.0f;
    }

    hsv->h = val * HUE_MAX;
}

void led_thread(void)
{
    msg_t msg;
    rgbled_t led;
    color_hsv_t hsv;
    color_rgb_t rgb;

    hsv.h = 0.0f;
    hsv.s = 1.0f;
    hsv.v = 1.0f;

    float val;

    /* initialize RGB-LED */
    rgbled_init(&led, PWM_0, 0, 1, 2);

    /* go threw the base colors */
    rgb = {255, 0, 0};
    rgbled_set(&led, &rgb);
    vtimer_usleep(1000 * 1000);
    rgb = {0, 255, 0};
    rgbled_set(&led, &rgb);
    vtimer_usleep(1000 * 1000);
    rgb = {0, 0, 255};
    rgbled_set(&led, &rgb);

    while (1) {
        /* see if something has come up */
        msg_receive(&msg);
        val = (float)msg.content.value;

        /* calculate new color */
        update_hsv(&hsv, val, msg.type);

         /* set color */
        color_hsv2rgb(&hsv, &rgb);
        rgbled_set(&led, &rgb);
    }
}
