#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

int main(void)
{

	RCC->AHB1ENR |= (1 << 3);

	GPIOD->MODER &= ~(0x3 << (12 * 2));
	GPIOD->MODER |=  (0x1 << (12 * 2));

	while(1) {
		GPIOD->ODR |= (1 << 12);

		for(uint32_t i = 0; i < DELAY_COUNT; i++);

		GPIOD->ODR &= ~(1 << 12);

		for(uint32_t i = 0; i < DELAY_COUNT; i++);
	}
}
