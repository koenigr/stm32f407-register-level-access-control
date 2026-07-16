#pragma once

#include "Interfaces/ILedOutput.hpp"

class MockLedOutput : public ILedOutput {
public:
	void Green() override {
		green = true;
		red = false;
		blinkRed = false;
	}

	void Red() override {
		green = false;
		red = true;
		blinkRed = false;
	}

	void BlinkRed() override {
		green = false;
		red = false;
		blinkRed = true;
	}

	void Off() override {
		green = false;
		red = false;
		blinkRed = false;
	}

	bool green = false;
	bool red = false;
	bool blinkRed = false;
};