#pragma once
#include "IClock.hpp"

struct MockClock : IClock {
	uint32_t now = 0;
	uint32_t NowMs() const override {return now;}
};