#include "PinValidator.hpp"

#include "sha256.h"

#include <cstddef>
#include <cstring>
#include <string_view>

namespace {

constexpr std::size_t PIN_LENGTH = 4;

const uint8_t CORRECT_PIN_HASH[SHA256_BLOCK_SIZE] = {
	0xd6, 0x03, 0xfa, 0xcf, 0x7e, 0xeb, 0x1a, 0xca,
	0x2a, 0xdf, 0x49, 0x05, 0xbe, 0x57, 0x66, 0x85,
	0x2e, 0xb1, 0x00, 0x53, 0xb2, 0x22, 0x39, 0x84,
	0x3f, 0xe1, 0xa7, 0x12, 0xda, 0xbb, 0x1c, 0x65
};

const char* const PIN_PEPPER = "STM32F4_Geheimnis!";

bool IsValidPin(std::string_view pin) {
	if (pin.size() != PIN_LENGTH) {
		return false;
	}
	for (const char c : pin) {
		if (c < '0' || c > '9') {
			return false;
		}
	}
	return true;
}

void HashPin(std::string_view pin, uint8_t (&out)[SHA256_BLOCK_SIZE]) {
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(pin.data()), pin.size());
	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(PIN_PEPPER),
				  std::strlen(PIN_PEPPER));
	sha256_final(&ctx, out);
}

} // namespace

PinValidator::PinValidator() {
	std::memcpy(pinHash_, CORRECT_PIN_HASH, SHA256_BLOCK_SIZE);
}

bool PinValidator::Validate(std::string_view pin) const {

	if (!IsValidPin(pin)) {
		return false;
	}

	uint8_t computed_hash[SHA256_BLOCK_SIZE];

	HashPin(pin, computed_hash);

	return std::memcmp(computed_hash, pinHash_, SHA256_BLOCK_SIZE) == 0;
}

bool PinValidator::SetPin(std::string_view newPin) {

	if (!IsValidPin(newPin)) {
		return false;
	}

	HashPin(newPin, pinHash_);

	return true;
}