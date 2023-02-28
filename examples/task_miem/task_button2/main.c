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
    gpio_init(GPIO_PIN(PORT_A, 6), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_A, 7), GPIO_IN);
    gpio_write(GPIO_PIN(PORT_A, 6), 12);
    bool bPushed = false;
    
    while (1)
    {
        if ((bPushed == false) && (gpio_read(GPIO_PIN(PORT_A, 7)) == 128))
        {
            gpio_toggle(GPIO_PIN(PORT_A, 6));
            bPushed = true;
        } else if ((bPushed == true) && (gpio_read(GPIO_PIN(PORT_A, 7)) == 0))
        {
            bPushed = false;
        }

    }
    return 0;
}
