#pragma once

#include "sha256.h"

#include <array>
#include <cstdint>
#include <string_view>

class PinValidator {
public:
	PinValidator();

	bool Validate(std::string_view pin) const;
	bool SetPin(std::string_view newPin);

private:
	uint8_t pinHash_[SHA256_BLOCK_SIZE]{};
};