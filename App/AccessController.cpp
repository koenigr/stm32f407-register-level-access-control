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

	switch(state_) {
	case LockState::Locked: {
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
		break;
	}

	case LockState::Unlocked: {
		char key = keypad_.Scan();

		if (key == '\0')
			return;

		if (key >= '0' && key <= '9') {
			AppendDigit(key);
			return;
		}

		if (key != '#')
		{
			ClearPin();
			return;
		} else if (key == '#') {
			if (length_ == 4) {
				pin_[length_] = '\0';
				validator_.SetPin(pin_);
				state_ = LockState::Locked;
				leds_.Red();
			}

			ClearPin();
		}
		break;
	}

	case LockState::LockedOut:
		// check timout
		break;
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