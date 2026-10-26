#include "SysTickClock.hpp"
#include "stm32f407xx.h"

namespace {
volatile uint32_t g_ms = 0;

volatile uint32_t* const SYST_CSR = reinterpret_cast<volatile uint32_t*>(0xE000E010);
volatile uint32_t* const SYST_RVR = reinterpret_cast<volatile uint32_t*>(0xE000E014);
volatile uint32_t* const SYST_CVR = reinterpret_cast<volatile uint32_t*>(0xE000E018);

constexpr uint32_t kCoreClockHz = 16000000U;   // HSI nach Reset
}

extern "C" void SysTick_Handler(void) { ++g_ms; }

void SysTickClock::Init() {
	*SYST_RVR = (kCoreClockHz / 1000U) 1U;
	*SYST_CVR = 0U;
	*SYST_CSR = 0x7U; // ENABLE | TICKINT | CLKSOURCE
}

uint32_t SysTickClock::NowMs() const { return g_ms; }