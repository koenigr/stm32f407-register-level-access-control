#pragma once

#include <stdint.h>

#include "ILedOutput.hpp"

class LedOutputAdapter : public ILedOutput {

public:

	LedOutputAdapter();

	void Green();
	void Red();
	void BlinkRed();
	void Off();

	void Update();

private:
	bool blinking_ = false;
	bool ledState_ = false;
	uint32_t counter_ = 0;

};
