#include "LedOutputAdapter.hpp"

#include "stm32f407xx_gpio.h"

LedOutputAdapter::LedOutputAdapter() {
	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_InitOutput(GPIOD, 12); // green
	GPIO_InitOutput(GPIOD, 14); // red
}

void LedOutputAdapter::Red() {
	GPIO_WritePin(GPIOD, 14, 1);
	GPIO_WritePin(GPIOD, 12, 0);
}

void LedOutputAdapter::Green() {
	GPIO_WritePin(GPIOD, 12, 1);
	GPIO_WritePin(GPIOD, 14, 0);
}

void LedOutputAdapter::BlinkRed() {
	// todo
}

void LedOutputAdapter::Off() {
	GPIO_WritePin(GPIOD, 12, 0);
	GPIO_WritePin(GPIOD, 14, 0);
}