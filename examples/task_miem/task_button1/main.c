#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "thread.h"


int main(void)
{
    gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C, 13), GPIO_IN);
    bool bPushed = false;
    
    while (1)
    {
        if ((bPushed == false) && (gpio_read(GPIO_PIN(PORT_C, 13)) != 0))
        {
            gpio_toggle(GPIO_PIN(PORT_A, 5));
            bPushed = true;
        } else if ((bPushed == true) && (gpio_read(GPIO_PIN(PORT_C, 13)) == 0))
        {
            bPushed = false;
        }
    }

    return 0;
}
