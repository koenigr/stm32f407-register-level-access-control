#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_exti.h"

EXTI_TypeDef test_exti;

#define COL_MASK (0x0FU << 8)

uint32_t fake_odr;
int fake_key_active;
int fake_key_row;
int fake_key_col;
int fake_zero_reads;
int fake_release_after;
int fake_read_calls;
int fake_imr_masked_during_read = 1;

void Fake_Reset(void) {
	test_exti.IMR = COL_MASK;
	test_exti.PR = 0;
	fake_odr = 0;
	fake_key_active = 0;
	fake_zero_reads = 0;
	fake_release_after = 3;
	fake_read_calls = 0;
	fake_imr_masked_during_read = 1;
}