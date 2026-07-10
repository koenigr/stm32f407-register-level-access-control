#include "AttemptCounter.hpp"

void AttemptCounter::Reset() {
	attempts = 0;
}

void AttemptCounter::Increment() {
	attempts++;
}

bool AttemptCounter::IsLockedOut() const {
	return attempts >= MaxAttempts;
}