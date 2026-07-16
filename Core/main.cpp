#include "AccessController.hpp"

#include "KeypadAdapter.hpp"
#include "LedOutputAdapter.hpp"

int main() {

	KeypadAdapter keypad;
	LedOutputAdapter leds;

	AccessController controller(
		keypad,
		leds
	);

	leds.Red();

	while(true) {
		controller.Update();
		leds.Update();
	}

	return 0;
}