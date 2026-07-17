#include "AccessController.hpp"

#include "Mocks/MockKeypad.hpp"
#include "Mocks/MockLedOutput.hpp"

#include <cassert>

struct TestContext {
	MockKeypad keypad;
	MockLedOutput leds;
	AccessController controller;

	TestContext()
		: controller(keypad, leds)
		{}

	void EnterKeys(const char* keys) {
		while (*keys) {
			keypad.nextKey = *keys++;
			controller.Update();
		}
	}
};

void RunAccessControllerTests() {
	TestContext ctx;

	ctx.EnterKeys("1234#");

	assert(ctx.leds.green);
	assert(!ctx.leds.red);
	assert(!ctx.leds.blinkRed);
}

void RunAccessControllerLockoutTest() {
	TestContext ctx;

	// wrong pin
	ctx.EnterKeys("9999#9999#9999#");

	// correct PIN
	ctx.EnterKeys("1234#");

	// should stay locked
	assert(ctx.leds.blinkRed);
}

void RunAccessControllerShortPinTest() {
	TestContext ctx;

	ctx.EnterKeys("12#");

	assert(!ctx.leds.green);
	assert(!ctx.leds.blinkRed);
}

void RunAccessControllerLongPinTest() {
	TestContext ctx;

	ctx.EnterKeys("12345#");

	// the last number shall be ignored and only 1234 gets validated
	assert(ctx.leds.green);
	assert(!ctx.leds.red);
	assert(!ctx.leds.blinkRed);
}

void RunAccessControllerClearPinTest() {
	TestContext ctx;

	ctx.EnterKeys("9999#");

	assert(!ctx.leds.green);
	assert(ctx.leds.red);
	assert(!ctx.leds.blinkRed);

	// The Pin-buffer shall get cleared

	ctx.EnterKeys("1234#");

	assert(ctx.leds.green);
	assert(!ctx.leds.red);
	assert(!ctx.leds.blinkRed);
}