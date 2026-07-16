#include "AccessController.hpp"

#include "Mocks/MockKeypad.hpp"
#include "Mocks/MockLedOutput.hpp"

#include <cassert>

void RunAccessControllerTests() {
	MockKeypad keypad;
	MockLedOutput leds;

	AccessController controller(keypad, leds);

	keypad.nextKey = '1';
	controller.Update();

	keypad.nextKey = '2';
	controller.Update();

	keypad.nextKey = '3';
	controller.Update();

	keypad.nextKey = '4';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();

	assert(leds.green);
	assert(!leds.red);
	assert(!leds.blinkRed);
}

void RunAccessControllerLockoutTest() {
	MockKeypad keypad;
	MockLedOutput leds;

	AccessController controller(keypad, leds);

	//first wrong PIN 9999
	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();

	//second wrong PIN 9999
	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();

	//third wrong PIN 9999
	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '9';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();


	// correct PIN
	keypad.nextKey = '1';
	controller.Update();

	keypad.nextKey = '2';
	controller.Update();

	keypad.nextKey = '3';
	controller.Update();

	keypad.nextKey = '4';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();


	// should stay locked
	assert(leds.blinkRed);
}

void RunAccessControllerShortPinTest() {
	MockKeypad keypad;
	MockLedOutput leds;

	AccessController controller(keypad, leds);


	keypad.nextKey = '1';
	controller.Update();

	keypad.nextKey = '2';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();


	assert(!leds.green);
	assert(!leds.blinkRed);
}