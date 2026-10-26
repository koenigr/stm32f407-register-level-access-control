#pragma once
#include "IClock.hpp"

class SysTickClock : public IClock {
public:
	static void Init();
	uint32_t NowMs() const override;
}