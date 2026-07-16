#pragma once

#include "ILockOutput.hpp"

class LedOutputAdapter : public ILockOutput {

public:

	LedOutputAdapter();

	void Lock() override;
	void Unlock() override;

};
