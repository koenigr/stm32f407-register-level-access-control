#pragma once

#include <stdint.h>

class AttemptCounter {
public:
	void Reset();

	void Increment();

	bool IsLockedOut() const;

private:
	uint8_t attempts = 0;

	static constexpr uint8_t MaxAttempts = 3;
};