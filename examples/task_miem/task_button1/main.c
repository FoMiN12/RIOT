#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

// #include "periph_gpio_irq"
#include "thread.h"

#define BUTTON_PRESS_TIME 30
// #define BUTTON_SHORT_PRESS_TIME 100
// #define BUTTON_LONG_PRESS_TIME 1000

// #define MAX_PRESS_DURATION 1000

#define BUTTON_PIN GPIO_PIN(PORT_C, 13)
#define LED_PIN GPIO_PIN(PORT_A, 5)

// static bool clickFlag = false;
// static uint32_t pressingTime = 0;

// static const uint32_t buttonPeriod = 10;

// //обработчик кнопки
// int buttonProcessing(uint32_t timePeriod){
//   //для временного хранения кода нажатия кнопки
//   int temp = 0;

//   //если кнопка нажата
//   if (gpio_read(BUTTON_PIN) == 0) {
//     //считаем время нажатия
//     pressingTime += timePeriod;

//     //защита от переполнения
//     if (pressingTime >= MAX_PRESS_DURATION) {
//       pressingTime = MAX_PRESS_DURATION;
//     }
    
//     //проверка дребезга
//     if(pressingTime >= BUTTON_PRESS_TIME && clickFlag == false){
//       temp = 1;
//       clickFlag = true;
//     }
//   }
//   //если кнопка не нажата
//   else{
//     //проверяем, сколько времени продолжалось нажатие кнопки
//     if(pressingTime >= BUTTON_LONG_PRESS_TIME){
//       temp = 3;
//     }
//     else if(pressingTime >= BUTTON_SHORT_PRESS_TIME){
//       temp = 2;
//     }

//     //сбрасываем для следующего измерения
//     pressingTime = 0;
//     clickFlag = false;
//   }

//   //возвращаем результат обработки кнопки
//   return temp;
// }
uint32_t currentTime(void) {
    return (uint32_t)ztimer_now(ZTIMER_USEC) / 1000;
}

static bool iSneedToUnableInt = false;
static bool iSneedToggle = false;

void btn_press_handler(void *arg) {
    (void) arg;
    iSneedToUnableInt = true;
}

void btn_unpress_handler(void *arg) {
    (void) arg;
    iSneedToggle = true;
}



int main(void)
{
    gpio_init(LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN, GPIO_IN);
    gpio_init_int(BUTTON_PIN,GPIO_IN, GPIO_FALLING, btn_press_handler, NULL);
    gpio_init_int(BUTTON_PIN,GPIO_IN, GPIO_RISING, btn_unpress_handler, NULL);
    
    // int tempButtonPress = 0;
    uint32_t lastCheckTime = currentTime();
    bool isIrqEnabled = true;
    while (1)
    {
        if (iSneedToUnableInt && isIrqEnabled) {
            gpio_irq_disable(BUTTON_PIN);
            isIrqEnabled = false;
            lastCheckTime = currentTime();
            iSneedToUnableInt = false;
        } else if (!isIrqEnabled && currentTime() - lastCheckTime > BUTTON_PRESS_TIME) {
            gpio_irq_enable(BUTTON_PIN);
            isIrqEnabled = true;
        }

        if (iSneedToggle) {
            gpio_toggle(LED_PIN);
            iSneedToggle = false;
        }


        // if (currentTime() - lastCheckTime > buttonPeriod) {
        //     tempButtonPress = buttonProcessing(buttonPeriod);
        //     lastCheckTime = currentTime();
        //     // if (tempButtonPress != 0) {
        //     //     printf("%d\n", tempButtonPress);
        //     // }
            
        //     if (tempButtonPress == 2 || tempButtonPress == 3)
        //     {
        //         gpio_toggle(LED_PIN);
        //     }
        // }
        
    }

    return 0;
}
