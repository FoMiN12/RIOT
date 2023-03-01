/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Blinky application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "thread.h"

#define LED_AUTO_GREEN (GPIO_PIN(PORT_C, 7)) // D9
#define LED_AUTO_YELLOW (GPIO_PIN(PORT_B, 6)) // D10
#define LED_AUTO_RED (GPIO_PIN(PORT_A, 7)) // D11

#define LED_USER_GREEN (GPIO_PIN(PORT_A, 6)) // D12
#define LED_USER_RED (GPIO_PIN(PORT_A, 5)) // D13

#define BUTTON_USER (GPIO_PIN(PORT_C, 13)) // button

uint32_t currentTime(void) {
    return ztimer_now(ZTIMER_USEC) / 1000;
}

int main(void)
{
    // ztimer_now(ZTIMER_MSEC);

    gpio_init(LED_AUTO_GREEN, GPIO_OUT);
    gpio_init(LED_AUTO_YELLOW, GPIO_OUT);
    gpio_init(LED_AUTO_RED, GPIO_OUT);

    gpio_init(LED_USER_GREEN, GPIO_OUT);
    gpio_init(LED_USER_RED, GPIO_OUT);

    gpio_init(BUTTON_USER, GPIO_IN);

    gpio_write(LED_AUTO_GREEN, 12);
    gpio_write(LED_USER_RED, 12);

    // gpio_write(GPIO_PIN(PORT_B, 6), 12);
    // gpio_write(GPIO_PIN(PORT_A, 7), 12);
    // bool bPushed = false;

    uint32_t lastTrafficChangingTime = currentTime();
    

    uint32_t autoGreenTime = 14000;
    uint32_t autoRedTime = 8000;
    uint32_t changingTime = 4000;
    
    bool isAutoGreen = true;
    bool isChangingSoon = false;
    
    uint32_t changingStartTime;
    uint32_t curTime;

    while (1)
    {
        curTime = currentTime();
        // printf("led button: %i led yellow: %i\n", gpio_read(BUTTON_USER), (int)gpio_read(LED_AUTO_YELLOW));
        if (!isChangingSoon) {
            if ((isAutoGreen && (!gpio_read(BUTTON_USER) || (curTime - lastTrafficChangingTime > autoGreenTime)))  
              || (!isAutoGreen && (curTime - lastTrafficChangingTime > autoRedTime))) 
            {
                changingStartTime = curTime;
                isChangingSoon = true;
                gpio_write(LED_AUTO_YELLOW, 12);
                // printf("%i\n", (int)gpio_read(LED_AUTO_YELLOW));
            }
        } else {
            if (curTime - changingStartTime > changingTime) {
                if (isAutoGreen) { 
                    gpio_write(LED_AUTO_GREEN, 0);
                    gpio_write(LED_AUTO_RED, 12);
                    
                    gpio_write(LED_USER_GREEN, 12);
                    gpio_write(LED_USER_RED, 0);
                } else {
                    gpio_write(LED_AUTO_GREEN, 12);
                    gpio_write(LED_AUTO_RED, 0);

                    gpio_write(LED_USER_GREEN, 0);
                    gpio_write(LED_USER_RED, 12);
                }
                gpio_write(LED_AUTO_YELLOW, 0);
                

                isAutoGreen = !isAutoGreen;
                isChangingSoon = false;
                lastTrafficChangingTime = curTime; 
            }
        }
        
        
        // else if (!isAutoGreen && (curTime - lastTrafficChangingTime > autoRedTime)) {
        //     isChangingSoon = true;
        //     changingStartTime = curTime;
        //     gpio_write(LED_AUTO_YELLOW, 12);
        // }
        // if ((bPushed == false) && (gpio_read(GPIO_PIN(PORT_C, 13)) != 0))
        // {
        //     gpio_toggle(GPIO_PIN(PORT_B, 6));
        //     bPushed = true;
        // } else if ((bPushed == true) && (gpio_read(GPIO_PIN(PORT_C, 13)) == 0))
        // {
        //     bPushed = false;
        // }
        // printf("%i\n", (int) gpio_read(GPIO_PIN(PORT_C, 13)));

    }
    return 0;
}
