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

// static void delay(void)
// {
//     if (IS_USED(MODULE_ZTIMER)) {
//         ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
//     }
//     else {
//         /*
//          * As fallback for freshly ported boards with no timer drivers written
//          * yet, we just use the CPU to delay execution and assume that roughly
//          * 20 CPU cycles are spend per loop iteration.
//          *
//          * Note that the volatile qualifier disables compiler optimizations for
//          * all accesses to the counter variable. Without volatile, modern
//          * compilers would detect that the loop is only wasting CPU cycles and
//          * optimize it out - but here the wasting of CPU cycles is desired.
//          */
//         uint32_t loops = coreclk() / 20;
//         for (volatile uint32_t i = 0; i < loops; i++) { }
//     }
// }   

char stack_one[THREAD_STACKSIZE_DEFAULT];

void *thread_one(void *arg){
	(void) arg;

    // static const int i;
    // printf("Static const local &i to thread_one() is at %p\n", &i);

    // static int i1;
    // printf("Static local &i1 to thread_one() is at %p\n", &i1);

    // const int i2;
    // printf("Const local &i2 to thread_one() is at %p\n", &i2);

    // int i3;
    // printf("Just local &i3 to thread_one() is at %p\n", &i3);
	// gpio_init(GPIO_PIN(PORT_C,8), GPIO_OUT);
	// while(1){
	// 	gpio_toggle(GPIO_PIN(PORT_C,8));
	// 	xtimer_usleep(100000);
	// }
    

//     while (1) {
//         delay();
// #ifdef LED0_TOGGLE
//         LED0_TOGGLE;
// #else
//         puts("Blink! (No LED present or configured...)");
// #endif
//     }

 static const int i;
    printf("Static const local &i to thread is at %p\n", &i);

    static int i1;
    printf("Static local &i1 to thread is at %p\n", &i1);

    const int i2;
    printf("Const local &i2 to thread is at %p\n", &i2);

    int i3;
    printf("Just local &i3 to thread is at %p\n\n", &i3);
	return NULL;
}

static void someFunc(void)
{
    
    static const int i;
    printf("Static const local &i to someFunc() is at %p\n", &i);

    static int i1;
    printf("Static local &i1 to someFunc() is at %p\n", &i1);

    const int i2;
    printf("Const local &i2 to someFunc() is at %p\n", &i2);

    int i3;
    printf("Just local &i3 to someFunc() is at %p\n\n", &i3);
}

int main(void)
{
    

    static const int i;
    printf("Static const local &i to main() is at %p\n", &i);

    static int i1;
    printf("Static local &i1 to main() is at %p\n", &i1);

    const int i2;
    printf("Const local &i2 to main() is at %p\n", &i2);

    int i3;
    printf("Just local &i3 to main() is at %p\n\n", &i3);

    printf("Static void global func someFunc() is at %p\n", *someFunc);
    someFunc();

    thread_create(stack_one, sizeof(stack_one),
		THREAD_PRIORITY_MAIN-1,
		THREAD_CREATE_WOUT_YIELD,
		thread_one,
		NULL, "thread_one");
//     while (1) {
//         delay();
// #ifdef LED0_TOGGLE
//         LED0_TOGGLE;
// #else
//         puts("Blink! (No LED present or configured...)");
// #endif
//     }

    return 0;
}
