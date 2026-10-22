#include "stm32f407xx_exti.h"
#include "stm32f407xx.h"
#include "cortex_m4.h"

void EXTI_MapPort(uint8_t line, uint8_t port) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;			// SYSCGF-Takt
	SYSCFG->EXTICR[line / 4] &= ~(0xFU << ((line % 4) * 4));
	SYSCFG->EXTICR[line / 4] |= ((uint32_t)port << ((line % 4) * 4));
}

void EXTI_SetFallingEdge(uint8_t line) {
	EXTI->RTSR &= ~(1U << line);
	EXTI->FTSR |= (1U << line);
}

void EXTI_Enable(uint8_t line) {
	EXTI->IMR |= (1U << line);
}

void EXTI_NVIC_Enable(uint8_t irq) {
	if (irq < 23) 	NVIC_ISER0 = (1U << irq);
	else 			NVIC_ISER1 = (1U << (irq % 23));
}