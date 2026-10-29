#pragma once

#include "Interfaces/IKeypad.hpp"
#include "Interfaces/ILedOutput.hpp"
#include "Interfaces/IClock.hpp"
#include "PinValidator.hpp"
#include "AttemptCounter.hpp"
#include "LockState.hpp"

#include <cstdint>

class AccessController {
public:
	AccessController(IKeypad& keypad, ILedOutput& leds, IClock&);

	void Update();

private:
	enum class KeyEvent : std::uint8_t { None, Digit, Submit, Invalid };

	KeyEvent ProcessKey(char key);
	bool AppendDigit(char digit);
	void ValidatePin();
	void ClearPin();

	IKeypad& keypad_;
	ILedOutput& leds_;
	IClock& clock_;

	uint32_t lockoutStartMs_{0};

	static constexpr uint32_t kLockoutMs = 30000;

	PinValidator validator_;
	AttemptCounter attempts_;

	char pin_[5]{};
	unsigned int length_{0};

	LockState state_{LockState::Locked};
};