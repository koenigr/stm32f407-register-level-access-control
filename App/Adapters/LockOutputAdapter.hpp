#pragma once

#include "ILockOutput.hpp"

class LockOutputAdapter : public ILockOutput {

public:
	void Lock() override;
	void Unlock() override;

};
