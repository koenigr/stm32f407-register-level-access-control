#include "AttemptCounter.hpp"
#include <cassert>

void RunAttemptCounterTests() {
	AttemptCounter counter;

	// Not locked out
	assert(!counter.IsLockedOut());

	// first fail
	counter.Increment();
	assert(!counter.IsLockedOut());

	// second fail
	counter.Increment();
	assert(!counter.IsLockedOut());

	// third fail
	counter.Increment();
	assert(counter.IsLockedOut());

	// Reset
	counter.Reset();
	assert(!counter.IsLockedOut());
}
