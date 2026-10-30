#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "cortex_m4.h"
#include "swo.h"

#include "unity.h"

#include <cstdint>


static void EnableGlobalInterrupts() {
	__asm volatile ("cpsie i"::: "memory");
}

static void TestExtiInterrupts(void)
{
	EnableGlobalInterrupts();

	EXTI->PR = (1U << 8);
	EXTI->SWIER = (1U << 8);

	TEST_ASSERT_EQUAL_UINT8(1U, Keypad_IrqPending());
	TEST_ASSERT_EQUAL_UINT8(0U, Keypad_IrqPending());
	TEST_ASSERT_BITS(1U << 8, 0U, EXTI->PR);

	EXTI->PR = (1U << 10);
	EXTI->SWIER = (1U << 10);

	TEST_ASSERT_EQUAL_UINT8(1U, Keypad_IrqPending());
	TEST_ASSERT_EQUAL_UINT8(0U, Keypad_IrqPending());
	TEST_ASSERT_BITS(1U << 10, 0U, EXTI->PR);
}

void setUp(void) {}
void tearDown(void) {}

void TestKeypadConfiguration(void) {
	Keypad_Init();

	TEST_ASSERT_BITS(
		1U << 3,
		1U << 3,
		RCC->AHB1ENR
	);

	TEST_ASSERT_EQUAL_HEX32(
		0x55U,
		GPIOD->MODER & 0xFFU
	);

	TEST_ASSERT_EQUAL_HEX32(
		0x55U << 16,
		GPIOD->PUPDR & (0xFFU << 16)
	);

	Keypad_EnableInterrupt();

	TEST_ASSERT_BITS(
		1U << 14,
		1U << 14,
		RCC->APB2ENR
	);

	TEST_ASSERT_EQUAL_HEX32(
		0x3333U,
		SYSCFG->EXTICR[2] & 0xFFFFU
	);

	TEST_ASSERT_BITS(
		0xFU << 8,
		0xFU << 8,
		EXTI->FTSR
	);

	TEST_ASSERT_BITS(
		0xFU << 8,
		0xFU << 8,
		EXTI->IMR
	);

	TEST_ASSERT_BITS(1U << 23, 1U << 23, NVIC_ISER0);

	TEST_ASSERT_BITS(1U << 8, 1U << 8, NVIC_ISER1);
}

int main()
{
	swoInit(16000000U, 125000U);

	UNITY_BEGIN();

	RUN_TEST(TestKeypadConfiguration);

	RUN_TEST(TestExtiInterrupts);

	UNITY_END();

	while (true) {
	}
}