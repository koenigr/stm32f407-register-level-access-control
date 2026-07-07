#include <stdint.h>
#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

static void delay(void) {
	for(uint32_t i = 0; i < 300000; i++);
}

static const char keypad[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

void Keypad_Init(void) {
	//1. Enable the peripheral clock of GPIOD peripheral
	GPIO_PeriClockControl(GPIOD, ENABLE);

	//2. Configure PD0, PD1, PD2, PD3 as output (rows)
	GPIO_InitOutput(GPIOD, 0);
	GPIO_InitOutput(GPIOD, 1);
	GPIO_InitOutput(GPIOD, 2);
	GPIO_InitOutput(GPIOD, 3);

	//3. configure PD8, PD9, PD10, PD11 as input (columns)
	GPIO_InitInput(GPIOD, 8);
	GPIO_InitInput(GPIOD, 9);
	GPIO_InitInput(GPIOD, 10);
	GPIO_InitInput(GPIOD, 11);

	//4. Enable internal pull-up resistors for PD8, PD9, PD10, PD11
	GPIO_EnablePullUp(GPIOD, 8);
	GPIO_EnablePullUp(GPIOD, 9);
	GPIO_EnablePullUp(GPIOD, 10);
	GPIO_EnablePullUp(GPIOD, 11);
}

char Keypad_Scan(void) {
	for(int row = 0; row < 4; row++) {
		//make all rows HIGH
		GPIO_WritePin(GPIOD, 0, 1);
		GPIO_WritePin(GPIOD, 1, 1);
		GPIO_WritePin(GPIOD, 2, 1);
		GPIO_WritePin(GPIOD, 3, 1);

		//make row LOW
		GPIO_WritePin(GPIOD, row, 0);

		//scan the columns
		for (int column = 0; column < 4; column++) {
			//check column low or high
			if(GPIO_ReadPin(GPIOD, column + 8) == 0) {
				// Key is pressed
				delay();
				while (GPIO_ReadPin(GPIOD, column + 8) == 0){}
				return keypad[column][row];
			}
		}
	}

	return 0;
}
