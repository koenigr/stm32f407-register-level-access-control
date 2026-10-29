#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Keypad_Init(void);
void Keypad_EnableInterrupt(void);
char Keypad_Scan(void);
char Keypad_ReadEvent(void);
uint8_t Keypad_IrqPending(void);

void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif