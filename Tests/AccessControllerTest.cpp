#include "AccessController.hpp"

#include "Mocks/MockKeypad.hpp"
#include "Mocks/MockLedOutput.hpp"

#include <gtest/gtest.h>

class AccessControllerTest : public ::testing::Test {
protected:
	MockKeypad keypad;
	MockLedOutput leds;
	AccessController controller{keypad, leds};

	void EnterKeys(const char* keys) {
		while (*keys) {
			keypad.nextKey = *keys++;
			controller.Update();
		}
	}
};

TEST_F(AccessControllerTest, CorrectPinGrantsAccess) {
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, ThreeWrongPinsLockOut) {
	EnterKeys("9999#9999#9999#");

	// correct PIN must not unlock
	EnterKeys("1234#");

	EXPECT_TRUE(leds.blinkRed);
}

TEST_F(AccessControllerTest, ShortPinIsNotAccepted) {
	EnterKeys("12#");

	EXPECT_FALSE(leds.green);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, LongPinIgnoresExtraDigit) {
	// the last digit is ignored, only 1234 gets validated
	EnterKeys("12345#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, PinBufferIsClearedAfterAttempt) {
	EnterKeys("9999#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
	EXPECT_FALSE(leds.blinkRed);

	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, LockoutIgnoresInput) {
	EnterKeys("9999#9999#9999#");

	// try correct PIN after lockout
	EnterKeys("1234#");

	EXPECT_TRUE(leds.blinkRed);
	EXPECT_FALSE(leds.green);
}

TEST_F(AccessControllerTest, InvalidKeysAreIgnored) {
	EnterKeys("AB*1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, SetNewPinAfterUnlocked) {
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);

	EnterKeys("5678#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, UseNewPinAfterUnlocked) {
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);

	EnterKeys("5678#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
	EXPECT_FALSE(leds.blinkRed);

	EnterKeys("5678#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}