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