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

void Fake_PressKey(int row, int col) {
	fake_key_active = 1;
	fake_key_row = row;
	fake_key_col = col;
	fake_zero_reads = 0;
}

void GPIO_PeriClockControl(GPIO_RegDef_t *GPIOx, uint8_t Enable) { (void)GPIOx; (void)Enable; }
void GPIO_InitOutput(GPIO_RegDef_t *GPIOx, uint8_t pin) { (void)GPIOx; (void)pin; }
void GPIO_InitInput(GPIO_RegDef_t *GPIOx, uint8_t pin) { (void)GPIOx; (void)pin; }
void GPIO_EnablePullUp(GPIO_RegDef_t *GPIOx, uint8_t pin) { (void)GPIOx; (void)pin; }

void GPIO_WritePin(GPIO_RegDef_t *GPIOx, uint8_t pin, uint8_t value) {
	(void)GPIOx;
	if (value) fake_odr |= (1U << pin);
	else       fake_odr &= ~(1U << pin);
}

uint8_t GPIO_ReadPin(GPIO_RegDef_t *GPIOx, uint8_t pin) {
	(void)GPIOx;
	fake_read_calls++;

	if (test_exti.IMR & COL_MASK) {
		fake_imr_masked_during_read = 0;
	}

	if (fake_key_active
		&& pin == (uint8_t)(fake_key_col + 8)
		&& ((fake_odr >> fake_key_row) & 1U) == 0) {
		if (fake_zero_reads < fake_release_after) {
			fake_zero_reads++;
			return 0;
		}
		fake_key_active = 0;
	}
	return 1;
}

void EXTI_MapPort(uint8_t line, uint8_t port) { (void)line; (void)port; }
void EXTI_SetFallingEdge(uint8_t line) { (void)line; }
void EXTI_Enable(uint8_t line) { (void)line; }
void EXTI_NVIC_Enable(uint8_t irq) { (void)irq; }

void ITM_SendChar(char c) { (void)c; }