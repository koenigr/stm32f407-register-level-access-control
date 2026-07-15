#pragma once

#include "ILockOutput.hpp"

class LockOutputAdapter : public ILockOutput {

public:

	LockOutputAdapter();

	void Lock() override;
	void Unlock() override;

};
