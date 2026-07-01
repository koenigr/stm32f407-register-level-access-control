#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

int main(void)
{

	volatile uint32_t moder = GPIOA->MODER;

	GPIO_RegDef_t *gpio = GPIOA;

	return 0;
}
