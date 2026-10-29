#pragma once

#include <stdint.h>

#include "ILedOutput.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor) -- final, never deleted via base pointer (no heap)
class LedOutputAdapter final: public ILedOutput {

public:

	LedOutputAdapter();

	void Green() override;
	void Red() override;
	void BlinkRed() override;
	void Off() override;

	void Update();

private:
	bool blinking_ = false;
	bool ledState_ = false;
	uint32_t counter_ = 0;

};
