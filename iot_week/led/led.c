#include <stdio.h>

#include "msg.h"

#include "led.h"
#include "color.h"


void led_thread(void)
{
    uint32_t event;
    color_hsv_t hsv = {0.0f, 1.0f, 1.0f};
    color_rgb_t rgb;
    msg_t msg;

    while (1) {
        msg_receive(&msg);
        event = (uint32_t)msg.content.value;





    }
}
