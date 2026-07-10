#include "PinValidator.hpp"

bool PinValidator::Validate(const char* pin) const {
	return 	pin[0] == '1' &&
			pin[1] == '2' &&
			pin[2] == '3' &&
			pin[3] == '4' &&
			pin[4] == '\0';
}