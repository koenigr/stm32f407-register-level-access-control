#ifndef STM32F407XX_GPIO_H_
#define STM32F407XX_GPIO_H_

#include <stdint.h>
#include "stm32f407xx.h"

typedef struct {
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
} GPIO_RegDef_t;

#define GPIOA ((GPIO_RegDef_t *)GPIOA_BASE)
#define GPIOB ((GPIO_RegDef_t *)GPIOB_BASE)
#define GPIOC ((GPIO_RegDef_t *)GPIOC_BASE)
#define GPIOD ((GPIO_RegDef_t *)GPIOD_BASE)
#define GPIOE ((GPIO_RegDef_t *)GPIOE_BASE)
#define GPIOF ((GPIO_RegDef_t *)GPIOF_BASE)
#define GPIOG ((GPIO_RegDef_t *)GPIOG_BASE)
#define GPIOH ((GPIO_RegDef_t *)GPIOH_BASE)
#define GPIOI ((GPIO_RegDef_t *)GPIOI_BASE)
#define GPIOJ ((GPIO_RegDef_t *)GPIOJ_BASE)
#define GPIOK ((GPIO_RegDef_t *)GPIOK_BASE)

void GPIO_PeriClockControl(GPIO_RegDef_t *GPIOx, uint8_t Enable);
void GPIO_Init(GPIO_RegDef_t *GPIOx, uint8_t PinNumber);
uint8_t GPIO_ReadPin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber);
void GPIO_WritePin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_TogglePin(GPIO_RegDef_t *GPIOx, uint8_t PinNumber);

#endif /* STM32F407XX_GPIO_H_ */