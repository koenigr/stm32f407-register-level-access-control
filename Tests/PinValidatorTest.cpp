#include "PinValidator.hpp"
#include <gtest/gtest.h>

TEST(PinValidatorTest, AcceptsCorrectPin) {
	PinValidator validator;

	EXPECT_TRUE(validator.Validate("1234"));
}

TEST(PinValidatorTest, RejectsWrongPin) {
	PinValidator validator;

	EXPECT_FALSE(validator.Validate("0000"));
}

TEST(PinValidatorTest, RejectsTooLongPin) {
	PinValidator validator;

	EXPECT_FALSE(validator.Validate("12345"));
}

