#pragma once

#include "ILedOutput.hpp"

class LedOutputAdapter : public ILedOutput {

public:

	LedOutputAdapter();

	void Green();
	void Red();
	void BlinkRed();
	void Off();

};
