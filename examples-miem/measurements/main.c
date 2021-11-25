#include <board.h>
#include <xtimer.h>
#include <periph/gpio.h>
#include "periph_conf.h"

static int main_pid;

static char stack3[THREAD_STACKSIZE_DEFAULT];
static char stack4[THREAD_STACKSIZE_DEFAULT];
static char stack5[THREAD_STACKSIZE_DEFAULT];
static char stack6[THREAD_STACKSIZE_DEFAULT];
static char stack7[THREAD_STACKSIZE_DEFAULT];
static char stack8[THREAD_STACKSIZE_DEFAULT];

void interrupt_handler(void *arg){
    (void) arg;
    static msg_t m;
    GPIOA->BSRR = GPIO_BSRR_BR_5; /* turn off LED using CMSIS; do not call HAL functions */
    gpio_irq_disable(BTN0_PIN);
    
    msg_send(&m, main_pid);
}

/* Same threading function for all the threads */
void* thread_func(void *arg){
    static msg_t m;
    kernel_pid_t next_pid = (int) arg;
    
    while(1){
        msg_receive(&m);
        GPIOA->BSRR = GPIO_BSRR_BR_5; /* turn off LED using CMSIS; do not call HAL functions */
        
        xtimer_sleep(1);
        msg_send(&m, next_pid);
    }
}

int main(void) {
    main_pid = thread_getpid();
    int thread3_pid = thread_create(stack3, sizeof(stack3),
                                              THREAD_PRIORITY_MAIN-1,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) main_pid,
                                              "Thread 3");
    int thread4_pid = thread_create(stack4, sizeof(stack4),
                                              THREAD_PRIORITY_MAIN-2,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) thread3_pid,
                                              "Thread 4");
    int thread5_pid = thread_create(stack5, sizeof(stack5),
                                              THREAD_PRIORITY_MAIN-3,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) thread4_pid,
                                              "Thread 5");
    int thread6_pid = thread_create(stack6, sizeof(stack6),
                                              THREAD_PRIORITY_MAIN-4,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) thread5_pid,
                                              "Thread 6");
    int thread7_pid = thread_create(stack7, sizeof(stack7),
                                              THREAD_PRIORITY_MAIN-5,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) thread6_pid,
                                              "Thread 7");
    int thread8_pid = thread_create(stack8, sizeof(stack8),
                                              THREAD_PRIORITY_MAIN-6,
                                              THREAD_CREATE_STACKTEST|THREAD_CREATE_WOUT_YIELD,
                                              thread_func,
                                              (void*) thread7_pid,
                                              "Thread 8");
    msg_t m;
    gpio_init(LED0_PIN, GPIO_OUT);
    
    gpio_init(GPIO_PIN(PORT_B, 13), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 14), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 15), GPIO_OUT);
    
    /* Test 1 - interrupt reaction time */
#if 0
    gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_FALLING, interrupt_handler, (void*) 0);
    
    msg_receive(&m);
    GPIOA->BSRR = GPIO_BSRR_BR_5; /* turn off LED using CMSIS; do not call HAL functions */
#endif

    /* Test 2 - context switch timing */
#if 0
    GPIOA->BSRR = GPIO_BSRR_BS_5; /* turn on led using CMSIS */
    msg_send(&m, thread3_pid);
    msg_receive(&m); /* receive a message from thread 3 */
#endif

    /* Test 3 - threads switching */
    msg_send(&m, thread8_pid); /* start rolling */
    while(1){
        msg_receive(&m);
        xtimer_sleep(1);
        msg_send(&m, thread8_pid);
    }

    return 0;
}
