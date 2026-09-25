#pragma once
#include <stdint.h>

struct ItmRegs {
	volatile uint32_t 	STIM[256];	// 0xE0000000 Stimulus Ports
	uint32_t			_r0[640];
	volatile uint32_t	TER;		// 0xE0000E00 Trace Enable
	uint32_t			_r1[15];
	volatile uint32_t	TPR;		// 0xE0000E40 Trace Privilege
	uint32_t			_r2[15];
	volatile uint32_t	TCR;		// 0xE0000E80 Trace Control
	uint32_t			_r3[75];
	volatile uint32_t	LAR;		// 0xE0000FB0 Lock Access
};

struct TpiuRegs {
	volatile uint32_t SSPSR;       // 0xE0040000
	volatile uint32_t CSPSR;       // 0x004
	uint32_t          _r0[2];
	volatile uint32_t ACPR;        // 0x010  Async Clock Prescaler
	uint32_t          _r1[55];
	volatile uint32_t SPPR;        // 0x0F0  Pin Protocol (2 = NRZ/UART)
	uint32_t          _r2[131];
	volatile uint32_t FFSR;        // 0x300
	volatile uint32_t FFCR;        // 0x304  Formatter Control
};

#define ITM     (reinterpret_cast<ItmRegs*>(0xE0000000UL))
#define TPIU    (reinterpret_cast<TpiuRegs*>(0xE0040000UL))

#define DEMCR   (*reinterpret_cast<volatile uint32_t*>(0xE000EDFCUL))
#define DHCSR   (*reinterpret_cast<volatile uint32_t*>(0xE000EDF0UL))
#define DBGMCU_CR (*reinterpret_cast<volatile uint32_t*>(0xE0042004UL))

#define DEMCR_TRCENA        (1UL << 24)
#define DHCSR_C_DEBUGEN     (1UL << 0)
#define DBGMCU_CR_TRACE_IOEN (1UL << 5)     // TRACE_MODE[7:6] = 00 -> asynchron (SWO)
#define ITM_LAR_UNLOCK      0xC5ACCE55UL
#define ITM_TCR_ITMENA      (1UL << 0)
#define ITM_TCR_SYNCENA     (1UL << 2)
#define ITM_TCR_TRACEBUSID(x) ((uint32_t)(x) << 16)