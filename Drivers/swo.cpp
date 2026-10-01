#include "swo.h"
#include "debug_regs.h"

void swoInit(uint32_t coreClockHz, uint32_t swoBaud) {
	// 1. Trace-Pin (PB3 = TRACESWO) freischalten, asynchroner Modus
	DBGMCU_CR = (DBGMCU_CR & ~(3UL << 6)) | DBGMCU_CR_TRACE_IOEN;

	// 2. Trace-Block einschalten (ITM/TPIU/DWT sind sonst nicht ansprechbar)
	DEMCR |= DEMCR_TRCENA;

	// 3. TPIU: NRZ (UART-artig), Formatter aus, Baudrate
	TPIU->SPPR = 2;								// Async SWO, NRZ
	TPIU->ACPR = (coreClockHz / swoBaud) -1;	// z.B. 168 MHz / 2 MHz -1 = 83
	TPIU->FFCR = 0x100;							// Formatter bypass

	// 4. ITM entsperren und kofigurieren
	ITM->LAR = ITM_LAR_UNLOCK;
	ITM->TCR = ITM_TCR_ITMENA | ITM_TCR_SYNCENA | ITM_TCR_TRACEBUSID(1);
	ITM->TPR = 1;
	ITM->TER = 1;
}

void ITM_SendChar(char c)
{
	if (!(DEMCR & DEMCR_TRCENA))       return;   // Trace nicht aktiv
	if (!(ITM->TCR & ITM_TCR_ITMENA))  return;   // ITM aus
	if (!(ITM->TER & 1UL))             return;   // Port 0 aus
	if (!(DHCSR & DHCSR_C_DEBUGEN))    return;   // kein Debugger -> nicht blockieren

	while (ITM->STIM[0] == 0) { }                // 0 = FIFO voll
	*reinterpret_cast<volatile uint8_t*>(&ITM->STIM[0]) = static_cast<uint8_t>(c);
}

void swoPuts(const char* s)
{
	while (*s) ITM_SendChar(*s++);
}