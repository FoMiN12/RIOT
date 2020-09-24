#include <board.h>
#include <periph/gpio.h>

void btn_handler(void *arg) {
    (void)arg;
    gpio_toggle(LED0_PIN);
}

int main(void) {
    gpio_init_int(UNWD_CONNECT_BTN, GPIO_IN_PU, GPIO_RISING, btn_handler, NULL);

    while(1) {}

    return 0;
}
