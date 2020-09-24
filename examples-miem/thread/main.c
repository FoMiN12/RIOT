#include <thread.h>
#include <msg.h>
#include <xtimer.h>
#include <board.h>
#include <periph/gpio.h>

static char my_stack[THREAD_STACKSIZE_DEFAULT];

void *my_thread(void *arg) {
    (void) arg;
    msg_t m;

    while(1) {
        msg_receive(&m);
        gpio_toggle(LED0_PIN);
    }
}

int main(void) {
    msg_t m;
    m.content.value = 1;

    kernel_pid_t pid = thread_create(my_stack, sizeof(my_stack),
                                     THREAD_PRIORITY_MAIN-1,
                                     THREAD_CREATE_STACKTEST,
                                     my_thread,
                                     NULL, "My own thread");

    xtimer_ticks32_t last_wakeup = xtimer_now();

    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, 500000);
        msg_send(&m, pid);
    }

    return 0;
}
