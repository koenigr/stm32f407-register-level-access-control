#include "KeypadAdapter.hpp"
#include "keypad.h"


char KeypadAdapter::Scan() {
	return Keypad_Scan();
}