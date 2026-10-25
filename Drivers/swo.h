#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void swoInit(uint32_t coreClockHz, uint32_t swoBaud);
void ITM_SendChar(char c);
void swoPuts(const char* s);

#ifdef __cplusplus
}
#endif