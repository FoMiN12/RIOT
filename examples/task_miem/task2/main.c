#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "thread.h"


char stack_one[THREAD_STACKSIZE_DEFAULT];

void *thread_one(void *arg){
	(void) arg;

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

    return 0;
}
