#include "KeypadAdapter.hpp"
#include <gtest/gtest.h>

extern "C" {
	void Keypad_Stub_Reset(void);
	extern int stub_init_order;
	extern int stub_enable_order;
	extern int stub_read_calls;
	extern char stub_read_value;
}

class KeypadAdapterTest : public ::testing::Test {
protected:
	void SetUp() override { Keypad_Stub_Reset(); }
};

TEST_F(KeypadAdapterTest, ConstructorInitializesBeforeEnablingInterrupt) {
	KeypadAdapter adapter;

	EXPECT_EQ(stub_init_order, 1);
	EXPECT_EQ(stub_enable_order, 2);
}

TEST_F(KeypadAdapterTest, ScanForwardsReadEvent) {
	KeypadAdapter adapter;

	stub_read_value = '\0';
	EXPECT_EQ(adapter.Scan(), '\0');

	stub_read_value = '5';
	EXPECT_EQ(adapter.Scan(), '5');

	EXPECT_EQ(stub_read_calls, 2);
}
