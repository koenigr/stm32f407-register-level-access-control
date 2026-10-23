#ifndef STM32F407XX_EXTI_H_
#define STM32F407XX_EXTI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// map EXTI-Line on port
void EXTI_MapPort(uint8_t line, uint8_t port);
// Set falling edge
void EXTI_SetFallingEdge(uint8_t line);
// Unmask interrupt-line
void EXTI_Enable(uint8_t line);
// activate NVIC
void EXTI_NVIC_Enable(uint8_t irq);

#ifdef __cplusplus
}
#endif

#endif /* STM32F407XX_EXTI_H_ */