#include "AttemptCounter.hpp"
#include <gtest/gtest.h>

TEST(AttemptCounterTest, InitiallyNotLockedOut) {
	AttemptCounter counter;

	EXPECT_FALSE(counter.IsLockedOut());
}

TEST(AttemptCounterTest, NotLockedOutAfterTwoFailures) {
	AttemptCounter counter;

	counter.Increment();
	EXPECT_FALSE(counter.IsLockedOut());

	counter.Increment();
	EXPECT_FALSE(counter.IsLockedOut());
}

TEST(AttemptCounterTest, LockedOutAfterThreeFailures) {
	AttemptCounter counter;

	counter.Increment();
	counter.Increment();
	counter.Increment();
	EXPECT_TRUE(counter.IsLockedOut());
}

TEST(AttemptCounterTest, ResetClearsLockout) {
	AttemptCounter counter;

	counter.Increment();
	counter.Increment();
	counter.Increment();
	counter.Reset();

	EXPECT_FALSE(counter.IsLockedOut());
}
