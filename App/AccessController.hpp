#pragma once

#include "Interfaces/IKeypad.hpp"
#include "Interfaces/ILedOutput.hpp"
#include "PinValidator.hpp"
#include "AttemptCounter.hpp"
#include "LockState.hpp"

class AccessController {
public:
	AccessController(IKeypad& keypad, ILedOutput& leds);

	void Update();

private:
	void AppendDigit(char digit);
	void ValidatePin();
	void ClearPin();

	IKeypad& keypad_;
	ILedOutput& leds_;

	PinValidator validator_;
	AttemptCounter attempts_;

	char pin_[5];
	unsigned int length_;

	LockState state_;
};