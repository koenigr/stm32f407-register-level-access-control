#include "LedOutputAdapter.hpp"

#include "stm32f407xx_gpio.h"

LedOutputAdapter::LedOutputAdapter() {
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_InitOutput(GPIOD, 12);
}

void LedOutputAdapter::Lock() {
	GPIO_WritePin(GPIOD, 12, 0);
}

void LedOutputAdapter::Unlock() {
	GPIO_WritePin(GPIOD, 12, 1);
}
