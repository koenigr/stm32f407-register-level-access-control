#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void Keypad_Init(void);
char Keypad_Scan(void);

uint8_t Keypad_IrqPending(void);

#ifdef __cplusplus
}
#endif

#endif