#include "KeypadAdapter.hpp"

extern "C" {
	#include "keypad.h"


char KeypadAdapter::Scan() {
	return Keypad_Scan();
}