#include "PinValidator.hpp"
#include "sha256.h"
#include <cstring>

const uint8_t CORRECT_PIN_HASH[SHA256_BLOCK_SIZE] = {
	0x03, 0xac, 0x67, 0x42, 0x16, 0xf3, 0xe1, 0x5c,
	0x76, 0x1e, 0xe1, 0xa5, 0xe2, 0x55, 0xf0, 0x67,
	0x95, 0x36, 0x23, 0xc8, 0xb3, 0x88, 0xb4, 0x45,
	0x9e, 0x13, 0xf9, 0x78, 0xd7, 0xc8, 0x46, 0xf4
};

const char* PIN_PEPPER = "STM32F4_Geheimnis!";

bool PinValidator::Validate(const char* pin) const {

	uint8_t computed_hash[SHA256_BLOCK_SIZE];

	SHA256_CTX ctx;
	sha256_init(&ctx);

	sha256_update(&ctx, reinterpret_cast<const uint8_t*>(pin), std::strlen(pin));

	sha256_final(&ctx, computed_hash);

	return std::memcmp(computed_hash, CORRECT_PIN_HASH, SHA256_BLOCK_SIZE) == 0;
}