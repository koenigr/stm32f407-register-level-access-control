#include "AccessController.hpp"

#include "Mocks/MockKeypad.hpp"
#include "Mocks/MockLockOutput.hpp"

#include <cassert>

void RunAccessControllerTests() {
	MockKeypad keypad;
	MockLockOutput lock;

	AccessController controller(keypad, lock);

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

	assert(lock.unlocked);
}

void RunAccessControllerLockoutTest() {
	MockKeypad keypad;
	MockLockOutput lock;

	AccessController controller(keypad, lock);

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
	assert(lock.unlocked == false);
}

void RunAccessControllerShortPinTest() {
	MockKeypad keypad;
	MockLockOutput lock;

	AccessController controller(keypad, lock);


	keypad.nextKey = '1';
	controller.Update();

	keypad.nextKey = '2';
	controller.Update();

	keypad.nextKey = '#';
	controller.Update();


	assert(lock.unlocked == false);
}