#include <board.h>
#include <periph/gpio.h>
#include <thread.h>
#include <msg.h>
#include <lptimer.h>

enum button_state {
    MSG_BUTTON_UP,
    MSG_BUTTON_DOWN
};

struct antibounce {
    gpio_t pin;
    lptimer_t timer;
    int state;
    int interval;
    kernel_pid_t receiver;
};

struct antibounce ab4, ab5, ab6, ab7;

lptimer_t antibounce_timer;
void antibounce_reset(void* arg);
void antibounce_btn_handler(void *arg);
void antibounce_init(struct antibounce *ab, gpio_t btn, gpio_mode_t mode, int interval);

void antibounce_btn_handler(void *arg) {
    struct antibounce *ab = (struct antibounce*) arg;
    
    /* Ignore interrupts for some time */
    gpio_irq_disable(ab->pin);
    
    msg_t m;
    m.content.ptr = arg;

    if ( gpio_read(ab->pin) == 0 ){
        /* кнопка нажата */
        ab->state = 0;
        m.type = MSG_BUTTON_DOWN;
    } else {
        /* кнопка отпущена */
        ab->state = 1;
        m.type = MSG_BUTTON_UP;
    }
    msg_send(&m, ab->receiver);

    /* And re-enable interrupts in the future */
    ab->timer.callback = antibounce_reset;
    ab->timer.arg = arg;
    lptimer_set(&(ab->timer), ab->interval);
}

void antibounce_reset(void* arg){
    struct antibounce *ab = (struct antibounce*) arg;
    msg_t m;
    m.content.ptr = arg;
    
    if( gpio_read(ab->pin) == 0 ){
        if( ab-> state == 1 ){
            /* send a btn down message, the button was pressed during an antibounce timer interval */
            m.type = MSG_BUTTON_DOWN;
            msg_send(&m, ab->receiver);
        }
        
        ab->state = 0;
    } else {
        if( ab-> state == 0 ){
            /* send a btn up message, the button was released during an antibounce time interval */
            m.type = MSG_BUTTON_UP;
            msg_send(&m, ab->receiver);
        }
        
        ab->state = 1;
    }
    
    gpio_irq_enable(ab->pin);
}

void antibounce_init(struct antibounce *ab, gpio_t pin, gpio_mode_t mode, int interval){
    ab->pin = pin;
    ab->state = gpio_read(pin);
    ab->interval = interval;
    ab->receiver = thread_getpid();
    gpio_init_int(pin, mode, GPIO_BOTH, antibounce_btn_handler, (void*) ab);
}

int main(void) {
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);
    
    gpio_init(UNWD_GPIO_16, GPIO_OUT);
    gpio_init(UNWD_GPIO_17, GPIO_OUT);
    
    antibounce_init(&ab4, UNWD_GPIO_4, GPIO_IN_PU, 300);
    antibounce_init(&ab5, UNWD_GPIO_5, GPIO_IN_PU, 300);
    antibounce_init(&ab6, UNWD_GPIO_6, GPIO_IN_PU, 300);
    antibounce_init(&ab7, UNWD_GPIO_7, GPIO_IN_PU, 300);
    
    while(1) {
        msg_t m;
        msg_receive(&m);
        
        int n = 0;
        if( m.content.ptr == &ab4 ){
            n = 4;
        } else if( m.content.ptr == &ab5 ){
            n = 5;
        } else if( m.content.ptr == &ab6 ){
            n = 6;
        } else if( m.content.ptr == &ab7 ){
            n = 7;
        }
        
        printf("%i %s\n", n, (m.type == MSG_BUTTON_UP) ? "up" : "down");
        
        if( m.type == MSG_BUTTON_DOWN ){
            if( m.content.ptr == &ab4 ){
                /* left relay break */
                gpio_clear(UNWD_GPIO_16);
            } else if( m.content.ptr == &ab6 ){
                /* left relay make */
                gpio_set(UNWD_GPIO_16);
            } else if( m.content.ptr == &ab5 ){
                /* right relay break */
                gpio_clear(UNWD_GPIO_17);
            } else if( m.content.ptr == &ab7 ){
                /* right relay make */
                gpio_set(UNWD_GPIO_17);
            }
        }
    }

    return 0;
}

