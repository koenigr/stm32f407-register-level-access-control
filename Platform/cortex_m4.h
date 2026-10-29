#ifndef CORTEX_M4_H_
#define CORTEX_M4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---- Cortex-M4 NVIC (Core-Peripherie, PPB-Bereich) --- */
#define NVIC_BASE		0xE000E100UL

#ifdef UNIT_TEST
extern volatile uint32_t test_nvic_iser[2];
#define NVIC_ISER0 (test_nvic_iser[0])
#define NVIC_ISER1 (test_nvic_iser[1])
#else
#define NVIC_ISER0 (*(volatile uint32_t *)(NVIC_BASE + 0x000UL))
#define NVIC_ISER1 (*(volatile uint32_t *)(NVIC_BASE + 0x004UL))
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORTEX_M4_H_ */