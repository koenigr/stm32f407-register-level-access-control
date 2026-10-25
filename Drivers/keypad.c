#include <stdint.h>
#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_exti.h"

#define ROW_MASK	0x000FU		// PD0 - PD3
#define COL_SHIFT	8U
#define COL_MASK	(0x000FU << COL_SHIFT)	// PD8 - PD11

static volatile uint8_t keypad_irq_flag = 0;

static void delay(void) {
	for(uint32_t i = 0; i < 300000; i++);
}

static const char keypad[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

static void Keypad_SetIdle(void) {
	for (int row = 0; row < 4; row++) {
		GPIO_WritePin(GPIOD, row, 0);
	}
}

void Keypad_Init(void) {
	//1. Enable the peripheral clock of GPIOD peripheral
	GPIO_PeriClockControl(GPIOD, ENABLE);

	//2. Configure PD0, PD1, PD2, PD3 as output (rows)
	for (int pin = 0; pin < 4; pin++) {
		GPIO_InitOutput(GPIOD, pin);
	}

	//3. configure PD8, PD9, PD10, PD11 as input (columns)
	for (int pin = 8; pin < 12; pin++) {
		GPIO_InitInput(GPIOD, pin);
	}

	//4. Enable internal pull-up resistors for PD8, PD9, PD10, PD11
	for (int pin = 8; pin < 12; pin++) {
		GPIO_EnablePullUp(GPIOD, pin);
	}

	Keypad_SetIdle();
}

void Keypad_EnableInterrupt(void) {
	for (int pin = 8; pin < 12; pin++) {
		EXTI_MapPort(pin, 3);
		EXTI_SetFallingEdge(pin);
		EXTI_Enable(pin);
	}
	EXTI_NVIC_Enable(23);
	EXTI_NVIC_Enable(40);
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
				return keypad[row][column];
			}
		}
	}

	return 0;
}

void EXTI9_5_IRQHandler(void) {
	// Lines 8 and 9
	if (EXTI->PR & ((1U << 8) | (1U << 9))) {
		EXTI->PR = (1U << 8) | (1U << 9); // (Write-1-to-clear)
		keypad_irq_flag = 1;
	}
}

void EXTI15_10_IRQHandler(void) {
	// Lines 10 and 11
	if (EXTI->PR & ((1U << 10) | (1U << 11))) {
		EXTI->PR = (1U << 10) | (1U << 11);
		keypad_irq_flag = 1;
	}
}

uint8_t Keypad_IrqPending(void) {
	if (keypad_irq_flag) {
		keypad_irq_flag = 0;
		return 1;
	}

	return 0;
}

char Keypad_ReadEvent(void) {
	char key = 0;

	if (!Keypad_IrqPending()) {
		return 0;
	}

	// 1. mask column interrupts
	EXTI->IMR &= ~COL_MASK;

	// 2. scan rows (wait for release)
	key = Keypad_Scan();

	// 3. restore idle state (all rows LOW)
	Keypad_SetIdle();

	// 4. debounce release
	delay();

	// 5. clear pending bits (write-1-to-clear), then unmask
	EXTI->PR = COL_MASK;
	EXTI->IMR |= COL_MASK;

	return key;
}