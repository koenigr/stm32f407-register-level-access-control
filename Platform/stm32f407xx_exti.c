#include "stm32f407xx_exti.h"
#include "stm32f407xx.h"
#include "cortex_m4.h"

void EXTI_MapPort(uint8_t line, uint8_t port) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;			// SYSCGF-Takt
	SYSCFG->EXTICR[line / 4U] &= ~(0xFU << (((uint32_t)line % 4U) * 4U));
	SYSCFG->EXTICR[line / 4U] |= ((uint32_t)port << (((uint32_t)line % 4U) * 4U));
}

void EXTI_SetFallingEdge(uint8_t line) {
	EXTI->RTSR &= ~(1U << line);
	EXTI->FTSR |= (1U << line);
}

void EXTI_Enable(uint8_t line) {
	EXTI->IMR |= (1U << line);
}

void EXTI_NVIC_Enable(uint8_t irq) {
	if (irq < 32U) {
		NVIC_ISER0 = (uint32_t)(1UL << irq);
	} else {
		NVIC_ISER1 = (uint32_t)(1UL << (uint8_t)(irq - 32U));
	}
}