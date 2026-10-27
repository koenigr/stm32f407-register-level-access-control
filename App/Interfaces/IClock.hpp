#pragma once
#include <cstdint>

struct IClock {
	virtual ~IClock() = default;
	virtual uint32_t NowMs() const = 0;
};