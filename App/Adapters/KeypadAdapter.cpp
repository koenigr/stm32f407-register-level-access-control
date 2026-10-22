#include "KeypadAdapter.hpp"
#include "keypad.h"

KeypadAdapter::KeypadAdapter() {
	Keypad_Init();
	Keypad_EnableInterrupt();
}

char KeypadAdapter::Scan() {
	return Keypad_ReadEvent();
}