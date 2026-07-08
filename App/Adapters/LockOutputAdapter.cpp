#include "LockOutputAdapter.hpp"

#include "stm32f407xx_gpio.h"

void LockOutputAdapter::Lock() {
	GPIO_WritePin(GPIOA, 12, 0);
}

void LockOutputAdapter::Unlock() {
	GPIO_WritePin(GPIOA, 12, 1);
}