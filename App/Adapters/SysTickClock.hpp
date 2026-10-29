#pragma once
#include "IClock.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor) -- final, never deleted via base pointer (no heap)
class SysTickClock final: public IClock {
public:
	static void Init();
	uint32_t NowMs() const override;
};