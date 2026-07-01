#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

int main(void)
{
	RCC_AHB1ENR_t volatile 	*const pClkCtrlReg   = ADDR_REG_AHB1ENR;
	GPIOx_MODE_t  volatile 	*const pPortDModeReg = ADDR_REG_GPIOD_MODE;
	GPIOx_ODR_t   volatile  *const pPortDOutReg  = ADDR_REG_GPIOD_OD;

	//1. enable the clock for GPIOD peripheral in the AHB1ENR
	pClkCtrlReg->gpiod_en = CLOCK_ENABLE; // for GPIO D

	// Configure the mode of the PD12 as output
	// a. clear the 24th and 25th bit positions (CLEAR)
	pPortDModeReg->mode_pin12 = MODE_CONF_OUTPUT;

	while(1) {
		pPortDOutReg->od_pin12 = PIN_STATE_HIGH;

		for(uint32_t i = 0; i < DELAY_COUNT; i++);

		pPortDOutReg->od_pin12 = PIN_STATE_LOW;

		for(uint32_t i = 0; i < DELAY_COUNT; i++);
	}
}
