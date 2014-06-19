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
#define TEMP_MIN        (27000.0f)
#define HUM_MAX         (90000.0f)
#define HUM_MIN         (40000.0f)

#define HUE_MAX         (310.0f)


void update_hsv(color_hsv_t *hsv, float val, int type)
{
    if (type == NODE_LED_TEMPERATURE) {
        puts("will put temp");
        val -= TEMP_MIN;
        printf("temp ist after minus %i\n", (unsigned int)val);
        val /= (TEMP_MAX - TEMP_MIN);
        printf("temp ist after math  %i.%i\n", (unsigned int)val, (unsigned int)(val * 1000));
    }
    else if (type == NODE_LED_HUMIDITY) {
        puts("will put hum");
        val -= HUM_MIN;
        printf("hum ist after minus %i\n", (unsigned int)val);
        val /= (HUM_MAX - HUM_MIN);
        printf("hum ist after math %i.%i\n", (unsigned int)val, (unsigned int)(val * 1000));
    }
    else {
        return;
    }

    if (val < 0.0f) {
        val = 0.0f;
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

    rgb.r = 0;
    rgb.g = 0;
    rgb.b = 0;

    float val;

    /* initialize RGB-LED */
    rgbled_init(&led, PWM_0, 1, 2, 0);

    /* go threw the base colors */
    rgb.r = 255;
    rgbled_set(&led, &rgb);
    vtimer_usleep(1000 * 1000);
    rgb.r = 0;
    rgb.g = 255;
    rgbled_set(&led, &rgb);
    vtimer_usleep(1000 * 1000);
    rgb.g = 0;
    rgb.b = 255;
    rgbled_set(&led, &rgb);
    vtimer_usleep(1000 * 1000);
    rgb.b = 0;
    rgbled_set(&led, &rgb);

    while (1) {
        /* see if something has come up */
        msg_receive(&msg);
        val = (float)msg.content.value;

        printf("led: got a message from net thread: %i\n", (unsigned int)val);

        /* calculate new color */
        update_hsv(&hsv, val, msg.type);

         /* set color */
        color_hsv2rgb(&hsv, &rgb);
        rgbled_set(&led, &rgb);
    }
}
