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

void update_hsv(color_hsv_t *hsv, uint32_t new_val, uint32_t old_value)
{
    hsv->h = (new_val / 20) * 360;
}

void led_thread(void)
{
    msg_t msg;
    rgbled_t led;
    color_hsv_t hsv;
    hsv.h = 1.0f;
    hsv.s = 1.0f;
    hsv.v = 1.0f;
    color_rgb_t rgb;
    
    uint32_t new_val = 0;
    uint32_t old_val = 0;

    /* initialize RGB-LED */
    rgbled_init(&led, PWM_0, 0, 1, 2);

    while (1) {
        /* see if something has come up */
        msg_receive(&msg);
        new_val = msg.content.value;
        
        update_hsv(&hsv, new_val, old_val);
        
         /* set color */
        color_hsv2rgb(&hsv, &rgb);
        rgbled_set(&led, &rgb);
        old_val = new_val;
    }
}
