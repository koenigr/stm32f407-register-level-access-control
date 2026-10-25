#pragma once

class PinValidator {
public:
	PinValidator();

	bool Validate(const char* pin) const;
	void SetPin(const char* newPin);
};