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

#define BUTTON_1 (GPIO_PIN(PORT_B, 0)) // A3
#define BUTTON_2 (GPIO_PIN(PORT_C, 1)) // A4
#define BUTTON_3 (GPIO_PIN(PORT_C, 0)) // A5

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

    short clickCount = 0;
    short buttonSequence[4] = {0,0,0,0};
    short password[4] = {1, 2, 3, 3};

    uint32_t timePush = 0;
    
    bool bPushed = false;
    bool timeFlag = false;

    while (1)
    {
        //printf("b1: %i b2: %i b3: %i\n", (int)gpio_read(BUTTON_1),(int)gpio_read(BUTTON_2),(int)gpio_read(BUTTON_3));
        if (bPushed == false)
        {
            timePush = currentTime();
        }
       // printf("%i", (unsigned long int) (timePush - currentTime()));
        if (gpio_read(BUTTON_1) == 0 && gpio_read(BUTTON_2) == 0 && gpio_read(BUTTON_3) == 0) 
        {

            bPushed = false;
            timeFlag = false;

        }
        else if (clickCount == 4) { 
            if (buttonSequence[0] == password[0] && buttonSequence[1] == password[1]
                && buttonSequence[2] == password[2] && buttonSequence[3] == password[3]) {
                    gpio_toggle(LED_PIN); 
                }
            printf("%d\n%d\n%d\n%d\n\n", buttonSequence[0], buttonSequence[1],
            buttonSequence[2], buttonSequence[3]);
            buttonSequence[0] = 0;
            buttonSequence[1] = 0;
            buttonSequence[2] = 0;
            buttonSequence[3] = 0;
            clickCount = 0;
            //printf("%i\n", clickCount);

        } else if (clickCount < 4) {
            if (gpio_read(BUTTON_1) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000 && timeFlag == false) {
                    buttonSequence[clickCount] = 1;
                    timeFlag = true;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } else if (gpio_read(BUTTON_2) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000 && timeFlag == false) {
                    buttonSequence[clickCount] = 2;
                    timeFlag = true;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } else if (gpio_read(BUTTON_3) != 0) {
                bPushed = true;
                if (currentTime() - timePush > 2000 && timeFlag == false) {
                    buttonSequence[clickCount] = 3;
                    timeFlag = true;
                    clickCount++;
                }
                // printf("%i", clickCount);
            } 
        }
    }
    return 0;
}
