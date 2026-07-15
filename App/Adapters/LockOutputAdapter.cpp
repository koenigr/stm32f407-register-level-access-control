#include "LockOutputAdapter.hpp"

#include "stm32f407xx_gpio.h"

LockOutputAdapter::LockOutputAdapter() {
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_InitOutput(GPIOA, 12);
}

void LockOutputAdapter::Lock() {
	GPIO_WritePin(GPIOA, 12, 0);
}

void LockOutputAdapter::Unlock() {
	GPIO_WritePin(GPIOA, 12, 1);
}