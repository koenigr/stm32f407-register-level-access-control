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