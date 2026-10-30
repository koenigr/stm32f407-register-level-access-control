#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "cortex_m4.h"
#include "swo.h"

#include <cstdint>

static uint32_t passed = 0;
static uint32_t failed = 0;

static void EnableGlobalInterrupts() {
	__asm volatile ("cpsie i"::: "memory");
}

static void TestExtiInterrupts() {
	swoPuts("\n--- EXTI interrupt tests ---\n");

	// Enable EXTI and NVIC configuration
	Keypad_EnableInterrupt();

	// Enable CPU-level interrupts
	EnableGlobalInterrupts();

	// Test EXTI line 8 -> IRQ 23
	EXTI->PR = (1U << 8);
	EXTI->SWIER = (1U << 8);

	Check("EXTI line 8 interrupt dispatched",
		  Keypad_IrqPending() == 1U);

	Check("EXTI line 8 flag consumed",
		  Keypad_IrqPending() == 0U);

	Check("EXTI line 8 pending bit cleared",
		  (EXTI->PR & (1U << 8)) == 0U);

	// Test EXTI line 10 -> IRQ 40
	EXTI->PR = (1U << 10);
	EXTI->SWIER = (1U << 10);

	Check("EXTI line 10 interrupt dispatched",
		  Keypad_IrqPending() == 1U);

	Check("EXTI line 10 flag consumed",
		  Keypad_IrqPending() == 0U);

	Check("EXTI line 10 pending bit cleared",
		  (EXTI->PR & (1U << 10)) == 0U);
}

static void Check(const char* name, bool condition)
{
	if (condition) {
			swoPuts("[PASS] ");
			++passed;
	} else {
			swoPuts("[FAIL] ");
			++failed;
	}

	swoPuts(name);
	swoPuts("\n");
}

int main()
{
	swoInit(16000000U, 125000U);
	swoPuts("\n--- STM32 Board Tests ---\n");

	Keypad_Init();

	Check("GPIOD clock enabled",
				(RCC->AHB1ENR & (1U << 3)) != 0U);

	Check("PD0-PD3 configured as outputs",
				(GPIOD->MODER & 0xFFU) == 0x55U);

	Check("PD8-PD11 configured as inputs",
				(GPIOD->MODER & (0xFFU << 16)) == 0U);

	Check("PD8-PD11 pull-ups enabled",
				(GPIOD->PUPDR & (0xFFU << 16)) == (0x55U << 16));

	Keypad_EnableInterrupt();

	Check("SYSCFG clock enabled",
				(RCC->APB2ENR & (1U << 14)) != 0U);

	Check("EXTI lines 8-11 mapped to port D",
				(SYSCFG->EXTICR[2] & 0xFFFFU) == 0x3333U);

	Check("EXTI falling edges configured",
				(EXTI->FTSR & (0xFU << 8)) == (0xFU << 8));

	Check("EXTI lines 8-11 unmasked",
				(EXTI->IMR & (0xFU << 8)) == (0xFU << 8));

	Check("NVIC IRQ 23 enabled",
				(NVIC_ISER0 & (1U << 23)) != 0U);

	Check("NVIC IRQ 40 enabled",
				(NVIC_ISER1 & (1U << 8)) != 0U);

	TestExtiInterrupts();

	swoPuts("\n--- Results ---\n");
	swoPuts("Passed: ");
	// Numeric result formatting can be added in the next iteration.
	swoPuts("See individual checks above.\n");

	if (failed == 0U) {
			swoPuts("BOARD TESTS PASSED\n");
	} else {
			swoPuts("BOARD TESTS FAILED\n");
	}

	while (true) {
	}
}