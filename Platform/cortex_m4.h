#ifndef CORTEX_M4_H_
#define CORTEX_M4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---- Cortex-M4 NVIC (Core-Peripherie, PPB-Bereich) --- */
#define NVIC_BASE		0xE000E100UL

#define NVIC_ISER0		(*(volatile uint32_t *)(NVIC_BASE + 0x000UL))
#define NVIC_ICER0		(*(volatile uint32_t *)(NVIC_BASE + 0x080UL))
#define NVIC_IPR_BASE	(NVIC_BASE + 0x300UL)

#ifdef __cplusplus
}
#endif

#endif /* CORTEX_M4_H_ */