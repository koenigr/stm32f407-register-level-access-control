#include "AccessController.hpp"


AccessController::AccessController(IKeypad& keypad, ILedOutput& leds, IClock& clock)
	: keypad_(keypad),
	  leds_(leds),
	  clock_(clock)
	  {}


AccessController::KeyEvent AccessController::ProcessKey(char key) {
	if (key == '\0')
		return KeyEvent::None;

	if (key >= '0' && key <= '9') {
		if (!AppendDigit(key)) {
			ClearPin();
			return KeyEvent::Invalid;
		}
		return KeyEvent::Digit;
	}

	if (key == '#')
		return KeyEvent::Submit;

	ClearPin();
	return KeyEvent::Invalid;
}

void AccessController::Update() {

	switch(state_) {
	case LockState::Locked: {
		const KeyEvent event = ProcessKey(keypad_.Scan());
		if (event == KeyEvent::Submit) {
			ValidatePin();
		} else if (event == KeyEvent::Invalid) {
			leds_.Red();
		}
		break;
	}

	case LockState::Unlocked: {
		if (ProcessKey(keypad_.Scan()) == KeyEvent::Submit) {
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
		if ((clock_.NowMs() - lockoutStartMs_) >= kLockoutMs) {
			attempts_.Reset();
			ClearPin();
			state_ = LockState::Locked;
			leds_.Red();
		}
		break;
	}
}

bool AccessController::AppendDigit(char digit) {
	if (length_ >= 4)
		return false;
	pin_[length_++] = digit;
	return true;
	
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
			lockoutStartMs_ = clock_.NowMs();
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