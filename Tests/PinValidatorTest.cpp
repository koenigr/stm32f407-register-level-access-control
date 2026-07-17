#include "PinValidator.hpp"
#include <cassert>


void RunPinValidatorTests()
{
	PinValidator validator;

	assert(validator.Validate("1234"));
	assert(!validator.Validate("0000"));
	assert(!validator.Validate("12345"));
}