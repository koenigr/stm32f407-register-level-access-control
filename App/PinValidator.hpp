#pragma once

#include <cstdint>
#include "sha256.h"

class PinValidator {
public:
	PinValidator();

	bool Validate(const char* pin) const;
	bool SetPin(const char* newPin);

private:
	uint8_t pinHash_[SHA256_BLOCK_SIZE];
};