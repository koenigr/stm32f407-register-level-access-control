#include <stdint.h>
#include <stdio.h>
#include "keypad.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

void delay(void) {
	for(uint32_t i = 0; i < 300000; i++);
}

//peripheral register addresses
uint32_t volatile *const pGPIODModeReg = (uint32_t*)(0x40020C00);
uint32_t volatile *const pInPutDataReg = (uint32_t*)(0x40020C00+0x10);
uint32_t volatile *const pOutPutDataReg = (uint32_t*)(0x40020C00+0x14);
uint32_t volatile *const pPullupDownReg = (uint32_t*)(0x40020C00+0x0C);

static const char keypad[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'},
};

void Keypad_Init(void) {
	//1. Enable the peripheral clock of GPIOD peripheral
	GPIO_periClockControl(GPIOOD, ENABLE);

	//2. Configure PD0, PD1, PD2, PD3 as output (rows)
	GPIO_InitOutput(GPIOD, 0);
	GPIO_InitOutput(GPIOD, 1);
	GPIO_InitOutput(GPIOD, 2);
	GPIO_InitOutput(GPIOD, 3);

	//3. configure PD8, PD9, PD10, PD11 as input (columns)
	GPIO_InitInput(GPIOD, 8);
	GPIO_InitInput(GPIOD, 9);
	GPIO_InitInput(GPIOD, 10);
	GPIO_InitInput(GPIOD, 11);

	//4. Enable internal pull-up resistors for PD8, PD9, PD10, PD11
	GPIO_EnablePullUp(GPIOD, 8);
	GPIO_EnablePullUp(GPIOD, 9);
	GPIO_EnablePullUp(GPIOD, 10);
	GPIO_EnablePullUp(GPIOD, 11);
}

char Keypad_Scan(void) {
	for(;;) {
		//make all rows HIGH
		*pOutPutDataReg |= 0x0f; // -->GPIO_WritePin(...)

		//make R1 LOW (PD0)
		*pOutPutDataReg &= ~(1 << 0);

		//scan the columns
		//check C1(PD8) low or high
		if(  ! (*pInPutDataReg & ( 1<<8 ))  ) { // --> GPIO_ReadPin(...)
			// Key is pressed
			printf("1\n");
			for(volatile int i = 0; i < 100000; i++);
		}

		//check C2(PD9) low or high
		if(  ! (*pInPutDataReg & ( 1<<9 ))  ) {
			// Key is pressed
			printf("2\n");
			for(volatile int i = 0; i < 100000; i++);
		}

		//check C3(PD10) low or high
		if(  ! (*pInPutDataReg & ( 1<<10 ))  ) {
			// Key is pressed
			printf("3\n");
			for(volatile int i = 0; i < 100000; i++);
		}

		//check C4(PD11) low or high
		if(  ! (*pInPutDataReg & ( 1<<11 ))  ) {
			// Key is pressed
			printf("A\n");
			for(volatile int i = 0; i < 100000; i++);
		}

		// ===============================================================================
		//make all rows HIGH
		*pOutPutDataReg |= 0x0f;

		//make R2 LOW (PD1)
		*pOutPutDataReg &= ~(1 << 1);


		//scan the columns
		//check C1(PD8) low or high
		if(  ! (*pInPutDataReg & ( 1<<8 ))  ) {
			// Key is pressed
			delay();
			printf("4\n");
		}

		//check C2(PD9) low or high
		if(  ! (*pInPutDataReg & ( 1<<9 ))  ) {
			// Key is pressed
			delay();
			printf("5\n");
		}

		//check C3(PD10) low or high
		if(  ! (*pInPutDataReg & ( 1<<10 ))  ) {
			// Key is pressed
			delay();
			printf("6\n");
		}

		//check C4(PD11) low or high
		if(  ! (*pInPutDataReg & ( 1<<11 ))  ) {
			// Key is pressed
			delay();
			printf("B\n");
		}

		// =============================================================
		//make all rows HIGH
		*pOutPutDataReg |= 0x0f;

		//make R3 LOW (PD2)
		*pOutPutDataReg &= ~(1 << 2);

		//scan the columns
		//check C1(PD8) low or high
		if(  ! (*pInPutDataReg & ( 1<<8 ))  ) {
			// Key is pressed
			delay();
			printf("7\n");
		}

		//check C2(PD9) low or high
		if(  ! (*pInPutDataReg & ( 1<<9 ))  ) {
			// Key is pressed
			delay();
			printf("8\n");
		}

		//check C3(PD10) low or high
		if(  ! (*pInPutDataReg & ( 1<<10 ))  ) {
			// Key is pressed
			delay();
			printf("9\n");
		}

		//check C4(PD11) low or high
		if(  ! (*pInPutDataReg & ( 1<<11 ))  ) {
			// Key is pressed
			delay();
			printf("C\n");
		}

		// =================================================================
		//make all rows HIGH
		*pOutPutDataReg |= 0x0f;

		//make R4 LOW (PD3)
		*pOutPutDataReg &= ~(1 << 3);

		//scan the columns
		//check C1(PD8) low or high
		if(  ! (*pInPutDataReg & ( 1<<8 ))  ) {
			// Key is pressed
			delay();
			printf("*\n");
		}

		//check C2(PD9) low or high
		if(  ! (*pInPutDataReg & ( 1<<9 ))  ) {
			// Key is pressed
			delay();
			printf("0\n");
		}

		//check C3(PD10) low or high
		if(  ! (*pInPutDataReg & ( 1<<10 ))  ) {
			// Key is pressed
			delay();
			printf("#\n");
		}

		//check C4(PD11) low or high
		if(  ! (*pInPutDataReg & ( 1<<11 ))  ) {
			// Key is pressed
			delay();
			printf("D\n");
		}
	}
}
