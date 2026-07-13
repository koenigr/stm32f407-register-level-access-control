#include "AccessController.hpp"

AccessController::AccessController(IKeypad& keypad, ILockOutput& lock)
	: keypad_(keypad),
	  lock_(lock),
	  pin_{},
	  length_(0) {}


void AccessController::Update() {

	if (attempts_.IsLockedOut()) return;
	
	char key = keypad_.Scan();

	if (key == '\0')
		return;

	if (key >= '0' && key <= '9')
	{
		AppendDigit(key);
		return;
	}

	if (key == '#')
	{
		ValidatePin();
	}

}

void AccessController::AppendDigit(char digit) {
	if (length_ < 4) {
		pin_[length_] = digit;
		++length_;
	}
}

void AccessController::ValidatePin() {
	pin_[length_] = '\0';

	if (validator_.Validate(pin_)) {
		lock_.Unlock();
		attempts_.Reset();
	} else {
		attempts_.Increment();
	}

	ClearPin();
}

void AccessController::ClearPin() {
	length_ = 0;

	for (char& c : pin_) {
		c = '\0';
	}
}