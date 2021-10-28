#include <board.h>
#include <periph/gpio.h>
#include "periph_conf.h"

void delay_ms(const uint16_t ms) {
    uint32_t i = ms * 4000; /* approx. value */
    while (i-- > 0) {
      __asm("nop");
    }
}

int main(void) {
    /* Example 1 - direct access to registers */
#if 0
    periph_clk_en(AHB, RCC_AHBENR_GPIOAEN);
    *((uint32_t*) 0x40020000) = (1 << (5*2));
    while(1) {
        delay_ms(500);
        *((uint32_t*) 0x40020018) = (1 << 5);
        delay_ms(500);
        *((uint32_t*) 0x40020018) = (1 << (5 + 16));
    }
#endif

    /* Example 2 - access to registers with CMSIS */
#if 0
    periph_clk_en(AHB, RCC_AHBENR_GPIOAEN);
    uint32_t tmpreg;
    tmpreg = GPIOA->MODER;
    tmpreg &= ~GPIO_MODER_MODER5;
    tmpreg |= GPIO_MODER_MODER5_0;
    GPIOA->MODER = tmpreg;
    while(1) {
        delay_ms(500);
        GPIOA->BSRR = GPIO_BSRR_BS_5;
        delay_ms(500);
        GPIOA->BSRR = GPIO_BSRR_BR_5;
    }
#endif

    /* Example 3 - RIOT's hardware abstraction */
#if 1
    gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);
    while(1) {
        delay_ms(500);
        gpio_set(GPIO_PIN(PORT_A, 5));
        delay_ms(500);
        gpio_clear(GPIO_PIN(PORT_A, 5));
    }
#endif

    return 0;
}
