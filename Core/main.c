#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "keypad.h"


int main(void) {

	GPIO_PeriClockControl(GPIOD, ENABLE);

	GPIO_InitOutput(GPIOD, 12); // LED

	Keypad_Init();

	while(1) {
		char key = Keypad_Scan();

		if(key) {
			volatile char pressed_key = key;
			GPIO_TogglePin(GPIOD, 12);
		}
	}
}