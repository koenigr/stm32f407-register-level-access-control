#include "stm32f407xx_gpio.h"

void GPIO_PeriClockControl(GPIO_RegDef_t *GPIOx, uint8_t Enable) {
	if (Enable) {
		if (GPIOx == GPIOA) RCC->AHB1ENR |= (1U << 0);
		else if (GPIOx == GPIOB) RCC->AHB1ENR |= (1U << 1);
		else if (GPIOx == GPIOC) RCC->AHB1ENR |= (1U << 2);
		else if (GPIOx == GPIOD) RCC->AHB1ENR |= (1U << 3);
		else if (GPIOx == GPIOE) RCC->AHB1ENR |= (1U << 4);
	} else {
		if (GPIOx == GPIOA) RCC->AHB1ENR &= ~(1U << 0);
		else if (GPIOx == GPIOB) RCC->AHB1ENR &= ~(1U << 1);
		else if (GPIOx == GPIOC) RCC->AHB1ENR &= ~(1U << 2);
		else if (GPIOx == GPIOD) RCC->AHB1ENR &= ~(1U << 3);
		else if (GPIOx == GPIOE) RCC->AHB1ENR &= ~(1U << 4);
	}

}

void GPIO_InitOutput(GPIO_RegDef_t *GPIOx, uint8_t PinNumber) {
	GPIOx->MODER &= ~(3U << (PinNumber * 2));
	GPIOx->MODER |=  (1U << (PinNumber * 2));
}

void GPIO_InitInput(GPIO_RegDef_t *GPIOx, uint8_t PinNumber) {

}

void GPIO_EnablePullUp(GPIO_RegDef_t *GPIOx, uint8_t PinNumber) {
	
}

uint8_t GPIO_ReadPin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber) {
	return (uint8_t)((GPIOx->IDR >> PinNumber) & 1U);
}

void GPIO_WritePin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber, uint8_t Value) {
	if (Value)
		GPIOx->ODR |= (1U << PinNumber);
	else
		GPIOx->ODR &= ~(1U << PinNumber);
}

void GPIO_TogglePin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber) {
	GPIOx->ODR ^= (1U << PinNumber);
}
