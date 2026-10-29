#pragma once
#include <cstdint>

struct IClock {
public:
	IClock(const IClock&) = delete;
	IClock& operator=(const IClock&) = delete;
	IClock(IClock&&) = delete;
	IClock& operator=(IClock&&) = delete;

	virtual uint32_t NowMs() const = 0;

protected:
	IClock() = default;
	~IClock() = default;
};