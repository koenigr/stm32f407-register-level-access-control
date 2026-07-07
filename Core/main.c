#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

int main(void)
{

	GPIO_PeriClockControl(GPIOD, ENABLE);

	GPIO_InitOutput(GPIOD, 12);

	while(1) {

		GPIO_TogglePin(GPIOD, 12);
		for(uint32_t i = 0; i < DELAY_COUNT; i++);

	}
}

/*
int main(void)
{_init(GPIOD, )
    Keypad_Init();

    while(1)
    {
        char key = Keypad_Scan();

        if(key)
        {
            // Debug
        }
    }
}*/