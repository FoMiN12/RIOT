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

int main(void)
{
    // ztimer_now(ZTIMER_MSEC);
    gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C, 13), GPIO_IN);
    gpio_write(GPIO_PIN(PORT_A, 5), 0);
    bool bPushed = false;
    bool ledOn = false;
    short i = 0;
    uint32_t k = 4000000000;
    uint32_t j = 4000000000;
    while (1)
    {
        if ((bPushed == false) && (gpio_read(GPIO_PIN(PORT_C, 13)) == 0))
        {
            bPushed = true;
            j = ztimer_now(ZTIMER_USEC)/1000;
            k = ztimer_now(ZTIMER_USEC)/1000;
        } 
        else if ((bPushed == true) && (gpio_read(GPIO_PIN(PORT_C, 13)) != 0))
        {
            bPushed = false;
            if ((ztimer_now(ZTIMER_USEC)/1000 - j) < 999)
            {    
                ledOn = !ledOn;
            }
            else
            {
                if (i == 0)
                {
                    i = 1;
                    j = 4000000000;
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
                else if (i == 1)
                {
                    i = 2;
                    j = 4000000000;
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
                else if (i == 2)
                {
                    i = 0;
                    j = 4000000000;
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
            }

        }

        if (ledOn == true)
        {
            if (i == 0)
            {
                if ((ztimer_now(ZTIMER_USEC)/1000 - k) > 499)
                {
                    gpio_toggle(GPIO_PIN(PORT_A, 5));
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
            }
            else if (i == 1)
            {
                if ((ztimer_now(ZTIMER_USEC)/1000 - k) > 999)
                {
                    gpio_toggle(GPIO_PIN(PORT_A, 5));
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
            }
            else if (i == 2)
            {
                if ((ztimer_now(ZTIMER_USEC)/1000 - k) > 1999)
                {
                    gpio_toggle(GPIO_PIN(PORT_A, 5));
                    k = ztimer_now(ZTIMER_USEC)/1000;
                }
            }
        }
        else
        {
            gpio_write(GPIO_PIN(PORT_A, 5), 0);
        }

    }
    return 0;
}
