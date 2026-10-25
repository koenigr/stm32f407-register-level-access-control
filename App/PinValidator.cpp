#include "PinValidator.hpp"
#include "sha256.h"
#include <cstring>

const uint8_t CORRECT_PIN_HASH[SHA256_BLOCK_SIZE] = {
    0xd6, 0x03, 0xfa, 0xcf, 0x7e, 0xeb, 0x1a, 0xca,
    0x2a, 0xdf, 0x49, 0x05, 0xbe, 0x57, 0x66, 0x85,
    0x2e, 0xb1, 0x00, 0x53, 0xb2, 0x22, 0x39, 0x84,
    0x3f, 0xe1, 0xa7, 0x12, 0xda, 0xbb, 0x1c, 0x65
};

const char* PIN_PEPPER = "STM32F4_Geheimnis!";

PinValidator::PinValidator() {
	std::memcpy(pinHash_, CORRECT_PIN_HASH, SHA256_BLOCK_SIZE);
}

bool PinValidator::Validate(const char* pin) const {

	if (pin == nullptr || std::strlen(pin) != 4) {
		return false;
	}

	for (int i = 0; i < 4; ++i) {
		if (pin[i] < '0' || pin[i] > '9') {
			return false;
		}
	}

	uint8_t computed_hash[SHA256_BLOCK_SIZE];

	SHA256_CTX ctx;
	sha256_init(&ctx);

	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(pin), std::strlen(pin));

	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(PIN_PEPPER),
		std::strlen(PIN_PEPPER));

	sha256_final(&ctx, computed_hash);

	return std::memcmp(computed_hash, pinHash_, SHA256_BLOCK_SIZE) == 0;
}

bool PinValidator::SetPin(const char* newPin) {

	if (newPin == nullptr || std::strlen(newPin) != 4) {
		return false;
	}

	for (int i = 0; i < 4; ++i) {
		if (newPin[i] < '0' || newPin[i] > '9') {
			return false;
		}
	}

	uint8_t computed_hash[SHA256_BLOCK_SIZE];

	SHA256_CTX ctx;
	sha256_init(&ctx);

	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(newPin), std::strlen(newPin));

	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(PIN_PEPPER),
		std::strlen(PIN_PEPPER));

	sha256_final(&ctx, computed_hash);

	std::memcpy(pinHash_, computed_hash, SHA256_BLOCK_SIZE);

}