#include "AccessController.hpp"

#include "KeypadAdapter.hpp"
#include "LockOutputAdapter.hpp"

int main() {

	KeypadAdapter keypad;
	LockOutputAdapter lock;

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