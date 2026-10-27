#include "AccessController.hpp"

#include "Mocks/MockKeypad.hpp"
#include "Mocks/MockLedOutput.hpp"

#include "Mocks/MockClock.hpp"

#include <gtest/gtest.h>

class AccessControllerTest : public ::testing::Test {
protected:
	MockKeypad keypad;
	MockLedOutput leds;
	MockClock clock;
	AccessController controller{keypad, leds, clock};

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

TEST_F(AccessControllerTest, LongPinIsRefected) {
	// the last digit is ignored, only 1234 gets validated
	EnterKeys("12345#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
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

TEST_F(AccessControllerTest, InvalidKeyBeforePinIsHarmless) {
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

// ---- Locked: validate input ----

TEST_F(AccessControllerTest, InvalidKeyInMiddleAbortsInput) {
	EnterKeys("12a34#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
}

TEST_F(AccessControllerTest, InvalidKeyAtEndAbortsInput) {
	EnterKeys("1234a#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
}

TEST_F(AccessControllerTest, StarAbortsInput) {
	EnterKeys("12*34#");

	EXPECT_FALSE(leds.green);
}

TEST_F(AccessControllerTest, ValidPinWorksAfterAbortedInput) {
	EnterKeys("12a34#");
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, OverflowDoesNotCountAsAttempt) {
	EnterKeys("9999#9999#");
	EnterKeys("12345#");
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, InvalidKeyDoesNotCountAsAttempt) {
	EnterKeys("9999#9999#");
	EnterKeys("9a99#");
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, ShortPinDoesNotCountAsAttempt) {
	EnterKeys("9999#9999#");
	EnterKeys("12#");
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.blinkRed);
}

// ---- Unlocked: PIN-change ----

TEST_F(AccessControllerTest, ChangePinRejectsInvalidKeyInMiddle) {
	EnterKeys("1234#");
	EnterKeys("56a78#");

	// bleibt Unlocked
	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, ChangePinRejectsOverflow) {
	EnterKeys("1234#");
	EnterKeys("56789#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, ChangePinRejectsShortPin) {
	EnterKeys("1234#");
	EnterKeys("56#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, ChangePinWorksAfterAbortedInput) {
	EnterKeys("1234#");
	EnterKeys("56a78#");    // aborted
	EnterKeys("5678#");     // valid -> Locked

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);

	EnterKeys("5678#");

	EXPECT_TRUE(leds.green);
}

TEST_F(AccessControllerTest, OldPinRejectedAfterChange) {
	EnterKeys("1234#");
	EnterKeys("5678#");     // -> Locked
	EnterKeys("1234#");

	EXPECT_FALSE(leds.green);
	EXPECT_TRUE(leds.red);
}

TEST_F(AccessControllerTest, ChangePinWithHashOnlyKeepsState) {
	EnterKeys("1234#");
	EnterKeys("#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

static constexpr uint32_t kLockoutMs = 30000U;

// Update() einmal ohne Taste, damit der LockedOut-Zweig läuft
#define TICK() do { keypad.nextKey = '\0'; controller.Update(); } while (0)

TEST_F(AccessControllerTest, StaysLockedOutBeforeTimeout) {
	EnterKeys("9999#9999#9999#");

	clock.now = kLockoutMs - 1;
	TICK();
	EnterKeys("1234#");

	EXPECT_TRUE(leds.blinkRed);
	EXPECT_FALSE(leds.green);
}

TEST_F(AccessControllerTest, LockoutEndsAfterTimeout) {
	EnterKeys("9999#9999#9999#");

	clock.now = kLockoutMs;
	TICK();

	EXPECT_TRUE(leds.red);
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, CorrectPinWorksAfterTimeout) {
	EnterKeys("9999#9999#9999#");

	clock.now = kLockoutMs;
	TICK();
	EnterKeys("1234#");

	EXPECT_TRUE(leds.green);
	EXPECT_FALSE(leds.red);
}

TEST_F(AccessControllerTest, AttemptsResetAfterTimeout) {
	EnterKeys("9999#9999#9999#");

	clock.now = kLockoutMs;
	TICK();

	EnterKeys("9999#9999#");
	EXPECT_FALSE(leds.blinkRed);     // 2 Fehlversuche reichen nicht

	EnterKeys("9999#");
	EXPECT_TRUE(leds.blinkRed);      // 3. löst Lockout aus
}

TEST_F(AccessControllerTest, KeysDuringLockoutAreNotBuffered) {
	EnterKeys("9999#9999#9999#");

	EnterKeys("12");                 // während Lockout
	clock.now = kLockoutMs;
	TICK();
	EnterKeys("34#");                // Puffer muss leer gewesen sein

	EXPECT_FALSE(leds.green);
}

TEST_F(AccessControllerTest, LockoutTimeoutSurvivesCounterWraparound) {
	clock.now = 0xFFFFFF00U;
	EnterKeys("9999#9999#9999#");

	clock.now = 0x00000100U;         // erst 512 ms vergangen
	TICK();
	EXPECT_TRUE(leds.blinkRed);

	clock.now = 0xFFFFFF00U + kLockoutMs;   // wrap-around, 30 s vergangen
	TICK();
	EXPECT_FALSE(leds.blinkRed);
}

TEST_F(AccessControllerTest, SecondLockoutRestartsTimer) {
	EnterKeys("9999#9999#9999#");
	clock.now = kLockoutMs;
	TICK();

	EnterKeys("9999#9999#9999#");    // zweiter Lockout bei now = 30000
	clock.now = kLockoutMs + (kLockoutMs - 1);
	TICK();

	EXPECT_TRUE(leds.blinkRed);
}