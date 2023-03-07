#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "thread.h"


int main(void)
{
    gpio_init(GPIO_PIN(PORT_B, 8), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 9), GPIO_IN);

    gpio_init(GPIO_PIN(PORT_A, 5), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_A, 6), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_A, 7), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 6), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_C, 7), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_A, 9), GPIO_IN);

    gpio_init(GPIO_PIN(PORT_A, 8), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 10), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 4), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 5), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 3), GPIO_IN);

    

    // gpio_read(GPIO_PIN(PORT_A, 7));
    // gpio_read(GPIO_PIN(PORT_B, 6));
    // gpio_read(GPIO_PIN(PORT_C, 7));
    // gpio_read(GPIO_PIN(PORT_A, 9));

    while (true)
    {
        printf("A6: %i A7: %i B6: %i C7: %i A9: %i\n", (int)gpio_read(GPIO_PIN(PORT_A, 6)), (int)gpio_read(GPIO_PIN(PORT_A, 7)), (int)gpio_read(GPIO_PIN(PORT_B, 6)),
         (int)gpio_read(GPIO_PIN(PORT_C, 7)), (int)gpio_read(GPIO_PIN(PORT_A, 9)));
    }
    

    // gpio_init(GPIO_PIN(PORT_A, 7), GPIO_OUT);
    // gpio_init(GPIO_PIN(PORT_B, 6), GPIO_OUT);
    // gpio_init(GPIO_PIN(PORT_C, 7), GPIO_OUT);
    // gpio_init(GPIO_PIN(PORT_A, 9), GPIO_OUT);

    // gpio_read(GPIO_PIN(PORT_A, 7));
    // gpio_read(GPIO_PIN(PORT_B, 6));
    // gpio_read(GPIO_PIN(PORT_C, 7));
    // gpio_read(GPIO_PIN(PORT_A, 9));
    
    // bool bPushed = false;
    
    // while (1)
    // {
    //     if ((bPushed == false) && (gpio_read(GPIO_PIN(PORT_C, 13)) != 0))
    //     {
    //         gpio_toggle(GPIO_PIN(PORT_A, 5));
    //         bPushed = true;
    //     } else if ((bPushed == true) && (gpio_read(GPIO_PIN(PORT_C, 13)) == 0))
    //     {
    //         bPushed = false;
    //     }
    // }

    return 0;
}
