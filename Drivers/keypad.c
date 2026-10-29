#include <stdint.h>
#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_exti.h"
#include "swo.h"

#define COL_SHIFT	8U
#define COL_MASK	((uint32_t)0x000FU << COL_SHIFT)	// PD8 - PD11

#define COL_MASK_8_9  ((uint32_t)0x0003U << 8U)
#define COL_MASK_10_11 ((uint32_t)0x0003U << 10U)

#define KEYPAD_LOW ((uint8_t)0U)

static volatile uint8_t keypad_irq_flag = 0;

static void delay(void) {
	for(uint32_t i = 0U; i < 300000UL; i++) {}
}

static void Keypad_SetIdle(void) {
	for (uint8_t row = 0U; row < 4U; row++) {
		GPIO_WritePin(GPIOD, row, 0);
	}
}

void Keypad_Init(void) {
	//1. Enable the peripheral clock of GPIOD peripheral
	GPIO_PeriClockControl(GPIOD, ENABLE);

	//2. Configure PD0, PD1, PD2, PD3 as output (rows)
	for (uint8_t pin = 0U; pin < 4U; pin++) {
		GPIO_InitOutput(GPIOD, pin);
	}

	//3. configure PD8, PD9, PD10, PD11 as input (columns)
	for (uint8_t pin = 8U; pin < 12U; pin++) {
		GPIO_InitInput(GPIOD, pin);
	}

	//4. Enable internal pull-up resistors for PD8, PD9, PD10, PD11
	for (uint8_t pin = 8U; pin < 12U; pin++) {
		GPIO_EnablePullUp(GPIOD, pin);
	}

	Keypad_SetIdle();
}

void Keypad_EnableInterrupt(void) {
	for (uint8_t pin = 8U; pin < 12U; pin++) {
		EXTI_MapPort(pin, 3);
		EXTI_SetFallingEdge(pin);
		EXTI_Enable(pin);
	}
	EXTI_NVIC_Enable(23);
	EXTI_NVIC_Enable(40);
}

char Keypad_Scan(void) {
	static const char keypad[4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'},
	};

	for(uint8_t row = 0U; row < 4U; row++) {
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
			// cppcheck-suppress misra-c2012-10.4
			if(GPIO_ReadPin(GPIOD, (uint8_t) (column + 8U)) == KEYPAD_LOW) {
				// Key is pressed
				delay();
				// cppcheck-suppress misra-c2012-10.4
				while (GPIO_ReadPin(GPIOD, (uint8_t) (column + 8U)) ==  KEYPAD_LOW){}
				return keypad[row][column];
			}
		}
	}

	return 0;
}

void EXTI9_5_IRQHandler(void) {
	// Lines 8 and 9
	if ((EXTI->PR & COL_MASK_8_9) != 0U) {
		EXTI->PR = COL_MASK_8_9; // (Write-1-to-clear)
		keypad_irq_flag = 1;
		ITM_SendChar('5');
	}
}

void EXTI15_10_IRQHandler(void) {
	// Lines 10 and 11
	if ((EXTI->PR & COL_MASK_10_11) != 0U) {
		EXTI->PR = COL_MASK_10_11;
		keypad_irq_flag = 1;
		ITM_SendChar('A');
	}
}

uint8_t Keypad_IrqPending(void) {
	if (keypad_irq_flag != 0U) {
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

	if (key != '\0') {
		ITM_SendChar(key);
		ITM_SendChar('\n');
	}

	return key;
}