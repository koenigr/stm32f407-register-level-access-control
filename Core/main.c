#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

int main(void)
{

	// Todo define RCC->AHB1ENR |= (1 << 3);
	RCC_AHB1ENR_t volatile *const pClkCtrlReg = ADDR_REG_AHB1ENR;
	pClkCtrlReg->gpiod_en = CLOCK_ENABLE; // for GPIO D

	GPIOD->MODER &= ~(0x3 << (12 * 2));
	GPIOD->MODER |=  (0x1 << (12 * 2));

	while(1) {
		GPIOD->ODR |= (1 << 12);

		for(uint32_t i = 0; i < DELAY_COUNT; i++);

		GPIOD->ODR &= ~(1 << 12);

		for(uint32_t i = 0; i < DELAY_COUNT; i++);
	}
}
