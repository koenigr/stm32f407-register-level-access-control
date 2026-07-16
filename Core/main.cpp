#include "AccessController.hpp"

#include "KeypadAdapter.hpp"
#include "LedOutputAdapter.hpp"

int main() {

	KeypadAdapter keypad;
	LedOutputAdapter lock;

	AccessController controller(
		keypad,
		lock
	);

	lock.Unlock();

	while(true) {
		controller.Update();
	}

	return 0;
}