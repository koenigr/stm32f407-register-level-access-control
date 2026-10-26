#include "keypad.h"
#include "stm32f407xx.h"

#include <gtest/gtest.h>

extern "C" {
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void Fake_Reset(void);
void Fake_PressKey(int row, int col);
extern uint32_t fake_odr;
extern int fake_read_calls;
extern int fake_imr_masked_during_read;
}

#define COL_MASK (0x0FU << 8)

class KeypadDriverTest : public ::testing::Test {
protected:
	void SetUp() override {
		Fake_Reset();
		while (Keypad_IrqPending()) {}
	}
};

TEST_F(KeypadDriverTest, NoPendingBitDoesNotSetFlag) {
	test_exti.PR = 0;

	EXTI9_5_IRQHandler();
	EXTI15_10_IRQHandler();

	EXPECT_EQ(Keypad_IrqPending(), 0);
}

TEST_F(KeypadDriverTest, Exti9_5HandlerSetsFlagAndClearsLines) {
	test_exti.PR = (1U << 9);

	EXTI9_5_IRQHandler();

	EXPECT_EQ(test_exti.PR, (1U << 8) | (1U << 9));	// W1C write of both lines
	EXPECT_EQ(Keypad_IrqPending(), 1);
	EXPECT_EQ(Keypad_IrqPending(), 0);				// flag consumed
}

TEST_F(KeypadDriverTest, Exti15_10HandlerSetsFlagAndClearsLines) {
	test_exti.PR = (1U << 10);

	EXTI15_10_IRQHandler();

	EXPECT_EQ(test_exti.PR, (1U << 10) | (1U << 11));
	EXPECT_EQ(Keypad_IrqPending(), 1);
}

TEST_F(KeypadDriverTest, ReadEventWithoutIrqReturnsNothing) {
	Fake_PressKey(0, 0);

	EXPECT_EQ(Keypad_ReadEvent(), 0);
	EXPECT_EQ(fake_read_calls, 0);		// no scan without interrupt
}

TEST_F(KeypadDriverTest, ReadEventReturnsPressedKey) {
	const struct { int row; int col; char key; } cases[] = {
		{0, 0, '1'}, {0, 3, 'A'}, {1, 2, '6'}, {2, 1, '8'}, {3, 0, '*'}, {3, 3, 'D'},
	};

	for (const auto& c : cases) {
		SCOPED_TRACE(std::string("expected key ") + c.key);
		Fake_Reset();
		while (Keypad_IrqPending()) {}

		test_exti.PR = (1U << (c.col + 8));
		if (c.col < 2) EXTI9_5_IRQHandler();
		else           EXTI15_10_IRQHandler();

		Fake_PressKey(c.row, c.col);

		EXPECT_EQ(Keypad_ReadEvent(), c.key);
	}
}

TEST_F(KeypadDriverTest, ReadEventRestoresStateAfterScan) {
	test_exti.PR = (1U << 9);
	EXTI9_5_IRQHandler();
	Fake_PressKey(1, 1);

	EXPECT_EQ(Keypad_ReadEvent(), '5');

	EXPECT_TRUE(fake_imr_masked_during_read);			// IMR masked while scanning
	EXPECT_EQ(test_exti.IMR & COL_MASK, COL_MASK);		// unmasked afterwards
	EXPECT_EQ(fake_odr & 0x0FU, 0U);					// rows back to LOW
	EXPECT_EQ(test_exti.PR, COL_MASK);					// W1C of all column lines

	EXPECT_EQ(Keypad_ReadEvent(), 0);					// event consumed
}

TEST_F(KeypadDriverTest, ReadEventWithoutKeyStillRestoresState) {
	// Interrupt (e.g. bounce), but no key found
	test_exti.PR = (1U << 8);
	EXTI9_5_IRQHandler();

	EXPECT_EQ(Keypad_ReadEvent(), 0);
	EXPECT_EQ(test_exti.IMR & COL_MASK, COL_MASK);
	EXPECT_EQ(fake_odr & 0x0FU, 0U);
}