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

TEST(PinValidatorTest, SetsNewPin) {
	PinValidator validator;

	validator.SetPin("5678");

	EXPECT_TRUE(validator.Validate("5678"));
	EXPECT_FALSE(validator.Validate("1234"));
}

TEST(PinValidatorTest, RejectsTooShortPin) {
	PinValidator validator;

	EXPECT_FALSE(validator.Validate("123"));
}

TEST(PinValidatorTest, RejectsNonDigitPin) {
	PinValidator validator;

	EXPECT_FALSE(validator.Validate("12a4"));
}

TEST(PinValidatorTest, RejectsInvalidNewPin) {
	PinValidator validator;

	validator.SetPin("12a4");

	EXPECT_TRUE(validator.Validate("1234"));
	EXPECT_FALSE(validator.Validate("12a4"));
}