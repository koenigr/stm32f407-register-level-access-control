// driver layer
#include "swo.h"

// application layer
#include "AccessController.hpp"
#include "KeypadAdapter.hpp"
#include "LedOutputAdapter.hpp"

// stdlib
#include <cstdint>

#define TEST_HARDFAULT

void test_invalid_fnptr(void) {
	void (* volatile fn)(void) = (void (*)(void))0x20000000; // missing thumb bit
	fn();
}

int main() {

	swoInit(16000000, 125000);  // HSI, no PLL
	swoPuts("Hello STM32\n");

	#ifdef TEST_HARDFAULT
		test_invalid_fnptr();
	#endif

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
	__attribute__ ((naked)) void HardFault_Handler(void) {
		__asm volatile (
			"tst lr, #4			\n"
			"ite eq				\n"
			"mrseq r0, msp		\n"
			"mrsne r0, psp		\n"
			"b hardfault_c		\n"
		);
	}

	__attribute__((used)) void hardfault_c(uint32_t *frame) {
		volatile uint32_t pc = frame[6];
		volatile uint32_t lr = frame[5];
		volatile uint32_t cfsr = *(volatile uint32_t *)0xE000ED28;
		volatile uint32_t hfsr = *(volatile uint32_t *)0xE000ED2C;
		(void)pc; (void)lr; (void)cfsr; (void)hfsr;
		__asm volatile ("bkpt #0");
		for (;;) {}
	}
}