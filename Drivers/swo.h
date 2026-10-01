#pragma once
#include <stdint.h>

void swoInit(uint32_t coreClockHz, uint32_t swoBaud);
void ITM_SendChar(char c);
void swoPuts(const char* s);