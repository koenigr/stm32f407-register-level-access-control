#include "keypad.h"

int stub_init_order;
int stub_enable_order;
int stub_read_calls;
char stub_read_value;

static int seq;

void Keypad_Stub_Reset(void) {
	seq = 0;
	stub_init_order = 0;
	stub_enable_order = 0;
	stub_read_calls = 0;
	stub_read_value = '\0';
}

void Keypad_Init(void) { stub_init_order = ++seq; }
void Keypad_EnableInterrupt(void) { stub_enable_order = ++seq; }
char Keypad_ReadEvent(void) { stub_read_calls++; return stub_read_value; }