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


// #define BUTTON_USER (GPIO_PIN(PORT_C, 13)) // button

#define BUTTON_1 (GPIO_PIN(PORT_A, 9)) // D8
#define BUTTON_2 (GPIO_PIN(PORT_C, 7)) // D9
#define BUTTON_3 (GPIO_PIN(PORT_B, 6)) // D10

#define LED_PIN (GPIO_PIN(PORT_A, 5)) // D13

uint32_t currentTime(void) {
    return ztimer_now(ZTIMER_USEC) / 1000;
}

int main(void)
{
    gpio_init(BUTTON_1, GPIO_IN);
    gpio_init(BUTTON_2, GPIO_IN);
    gpio_init(BUTTON_3, GPIO_IN);
    gpio_init(LED_PIN, GPIO_OUT);

    int clickCount = 0;
    int buttonSequence[4] = {0,0,0,0};
    int password[4] = {1, 2, 3, 3};

    uint32_t timePush = 0;
    
    bool bPushed = false;

    
    


    while (1)
    {
        //printf("b1: %i b2: %i b3: %i\n", (int)gpio_read(BUTTON_1),(int)gpio_read(BUTTON_2),(int)gpio_read(BUTTON_3));
        if (bPushed == false)
        {
            timePush = currentTime();
        }
       // printf("%i", (unsigned long int) (timePush - currentTime()));
        if (clickCount < 4) {
            if (gpio_read(BUTTON_1) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000) {
                    buttonSequence[clickCount] = 1;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } else if (gpio_read(BUTTON_2) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000) {
                    buttonSequence[clickCount] = 2;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } else if (gpio_read(BUTTON_3) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000) {
                    buttonSequence[clickCount] = 3;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } 
        }   else if (gpio_read(BUTTON_1) == 0 && gpio_read(BUTTON_2) == 0 && gpio_read(BUTTON_3) == 0) {
            bPushed = false;
        } else if (clickCount == 4) { 
            if (clickCount == 3 && buttonSequence[0] == password[0] && buttonSequence[1] == password[1]
                && buttonSequence[2] == password[2] && buttonSequence[3] == password[3]) {
                    gpio_toggle(LED_PIN); 
                }
            buttonSequence[0] = 0;
            buttonSequence[1] = 0;
            buttonSequence[2] = 0;
            buttonSequence[3] = 0;
            clickCount = 0;
            printf("%i\n", clickCount);
        }
        // printf("led button: %i led yellow: %i\n", gpio_read(BUTTON_USER), (int)gpio_read(LED_AUTO_YELLOW));
        
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
