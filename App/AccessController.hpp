#pragma once

#include "Interfaces/IKeypad.hpp"
#include "Interfaces/ILockOutput.hpp"
#include "PinValidator.hpp"
#include "AttemptCounter.hpp"
#include "LockState.hpp"

class AccessController {
public:
	AccessController(IKeypad& keypad, ILockOutput& lock);

	void Update();

private:
	void AppendDigit(char digit);
	void ValidatePin();
	void ClearPin();

	IKeypad& keypad_;
	ILockOutput& lock_;

	PinValidator validator_;
	AttemptCounter attempts_;

	char pin_[5];
	unsigned int length_;

	LockState state_;
};