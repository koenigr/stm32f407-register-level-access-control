#pragma once

#include "Interfaces/ILockOutput.hpp"

class MockLockOutput : public ILockOutput {
public:
	bool unlocked = false;

	void Lock() override {
		unlocked = false;
	}

	void Unlock() override {
		unlocked = true;
	}
};