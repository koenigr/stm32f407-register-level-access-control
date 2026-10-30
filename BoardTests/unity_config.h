#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include <stdint.h>
#include "swo.h"

#define UNITY_OUTPUT_CHAR(a) ITM_SendChar((uint32_t)(a))

#endif