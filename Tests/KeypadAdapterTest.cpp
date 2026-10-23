#include "KeypadAdapter.hpp"
#include <cassert>

extern "C" {
	void Keypad_Stub_Reset(void);
	extern int stub_init_order;
	extern int stub_enable_order;
	extern int stub_read_calls;
	extern char stub_read_value;
}

void RunKeypadAdapterInitOrderTest() {
	Keypad_Stub_Reset();

	KeypadAdapter adapter;

	assert(stub_init_order == 1);
	assert(stub_enable_order == 2);
}

void RunKeypadAdapterScanTest() {
	Keypad_Stub_Reset();

	KeypadAdapter adapter;

	// No event
	stub_read_value = '\0';
	assert(adapter.Scan() == '\0');

	// Key event
	stub_read_value = '5';
	assert(adapter.Scan() == '5');

	assert(stub_read_calls == 2);
}