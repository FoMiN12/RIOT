#include <stdio.h>

bool /*bool*/ STM_BUTTON_PUSHED = 0;
unsigned double int /*UDINT*/ STM_TIME_PUSH = 0;
unsigned double int /*UDINT*/ STM_CURRENT_TIME = 0 /*System time*/;
bool /*bool*/ STM_BUTTON_1 = 0;
bool /*bool*/ STM_BUTTON_2 = 0;
bool /*bool*/ STM_BUTTON_3 = 0;
bool /*bool*/ STM_LED = 0;
short int STM_CLICK_COUNT = 0;
short int STM_BUTTON_SEQUENCE[4] = {0, 0, 0, 0};
short int STM_PASSWORD[4] = {1, 2, 3, 3};
//bool perem = false;


void main(void)
{

	if (STM_BUTTON_PUSHED == 0)
	{
		STM_TIME_PUSH = STM_CURRENT_TIME;
	}
	// printf("%i", (unsigned long int) (STM_TIME_PUSH - STM_CURRENT_TIME));
	if (STM_BUTTON_1 == 0 && STM_BUTTON_2 == 0 && STM_BUTTON_3 == 0) 
	{
		STM_BUTTON_PUSHED = 0;
	} 
	else if (STM_CLICK_COUNT == 4) 
	{ 
		if (STM_BUTTON_SEQUENCE[0] == STM_PASSWORD[0] && STM_BUTTON_SEQUENCE[1] == STM_PASSWORD[1]
		&& STM_BUTTON_SEQUENCE[2] == STM_PASSWORD[2] && STM_BUTTON_SEQUENCE[3] == STM_PASSWORD[3]) 
		{
            //Держу в курсе, с интом это работать не будет работать
            //НО ЕБАНЫЙ VSC НЕ ВИДИТ ТИП ДАННЫХ bool
			STM_LED = !STM_LED; 
		}
		STM_BUTTON_SEQUENCE[0] = 0;
		STM_BUTTON_SEQUENCE[1] = 0;
		STM_BUTTON_SEQUENCE[2] = 0;
		STM_BUTTON_SEQUENCE[3] = 0;
		STM_CLICK_COUNT = 0;
	//	printf("%i\n", STM_CLICK_COUNT);
	}
	else if (STM_CLICK_COUNT < 4) 
	{
		if (STM_BUTTON_1 != 0 && STM_BUTTON_PUSHED == 0) 
		{
			STM_BUTTON_PUSHED = 1;
			STM_BUTTON_SEQUENCE[STM_CLICK_COUNT] = 1;
			STM_CLICK_COUNT += 1;
			// printf("%i", STM_CLICK_COUNT);
		} 
		else if (STM_BUTTON_2 != 0 && STM_BUTTON_PUSHED == 0) 
		{
			STM_BUTTON_PUSHED = 1;
			STM_BUTTON_SEQUENCE[STM_CLICK_COUNT] = 2;
			STM_CLICK_COUNT += 1;
			// printf("%i", STM_CLICK_COUNT);
		} 
		else if (STM_BUTTON_3 != 0 && STM_BUTTON_PUSHED == 0) {
			STM_BUTTON_PUSHED = 1;
			STM_BUTTON_SEQUENCE[STM_CLICK_COUNT] = 3;
			STM_CLICK_COUNT += 1;
		}
		// printf("%i", STM_CLICK_COUNT);
	} 
}
