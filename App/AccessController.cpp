#include "AccessController.hpp"


AccessController::AccessController(IKeypad& keypad, ILedOutput& leds)
	: keypad_(keypad),
	  leds_(leds),
	  pin_{},
	  length_(0),
	  state_(LockState::Locked)
	  {
	  	leds_.Red();
	  }


void AccessController::Update() {

	if (state_ == LockState::LockedOut) return;

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

	if (length_ != 4) {
		ClearPin();
		return;
	}

	pin_[length_] = '\0';

	if (validator_.Validate(pin_)) {
		leds_.Green();
		attempts_.Reset();
		state_ = LockState::Unlocked;
	} else {
		leds_.Red();
		attempts_.Increment();

		if (attempts_.IsLockedOut()) {
			state_ = LockState::LockedOut;
			leds_.BlinkRed();
		}
	}

	ClearPin();
}

void AccessController::ClearPin() {
	length_ = 0;

	for (char& c : pin_) {
		c = '\0';
	}
}