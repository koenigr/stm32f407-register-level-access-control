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

extern "C" {
	void HardFault_Handler(void) {
		// Software-Breakpoint. GDB can read the stacktrace
		__asm volatile("bkpt #0");

		while(1);
	}
}