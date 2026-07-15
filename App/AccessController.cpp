#include "AccessController.hpp"


AccessController::AccessController(IKeypad& keypad, ILockOutput& lock)
	: keypad_(keypad),
	  lock_(lock),
	  pin_{},
	  length_(0),
	  state_(LockState::Locked)
	  {
	  	lock_.Lock();
	  }


void AccessController::Update() {

	if (state_ == LockState::LockedOut) return;

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

	if (length_ != 4) {
		ClearPin();
		return;
	}

	pin_[length_] = '\0';

	if (validator_.Validate(pin_)) {
		lock_.Unlock();
		attempts_.Reset();
		state_ = LockState::Unlocked;
	} else {
		attempts_.Increment();

		if (attempts_.IsLockedOut()) {
			state_ = LockState::LockedOut;
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