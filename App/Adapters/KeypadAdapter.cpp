#include "KeypadAdapter.hpp"
#include "keypad.h"

KeypadAdapter::KeypadAdapter() {
	Keypad_Init();
}

char KeypadAdapter::Scan() {
	return Keypad_Scan();
}