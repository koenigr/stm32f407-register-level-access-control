// driver layer
#include "swo.h"

// application layer
#include "AccessController.hpp"
#include "KeypadAdapter.hpp"
#include "LedOutputAdapter.hpp"

int main() {

	swoInit(16000000, 2000000);  // HSI, no PLL
	swoPuts("Hello STM32\n");

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