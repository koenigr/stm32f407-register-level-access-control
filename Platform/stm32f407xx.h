#ifndef STM32F407XX_H_
#define STM32F407XX_H_

#include <stdint.h>

#define GPIOA_BASE 0x40020000
#define GPIOB_BASE 0x40020400
#define GPIOC_BASE 0x40020800
#define GPIOD_BASE 0x40020C00
#define GPIOE_BASE 0x40021000
#define GPIOF_BASE 0x40021400
#define GPIOG_BASE 0x40021800
#define GPIOH_BASE 0x40021C00
#define GPIOI_BASE 0x40022000
#define GPIOJ_BASE 0x40022400
#define GPIOK_BASE 0x40022800

#define RCC_BASE   0x40023800
#define RCC ((RCC_RegDef_t *)RCC_BASE)

#define ENABLE 		( 1 )
#define DISABLE		( 0 )

#define DELAY_COUNT			( 300000UL )

// Reference Manual 6.3.10
// RCC AHB1 peripheral clock register (RCC_AHB1ENR)
/*typedef struct {

	uint32_t gpioa_en	:1;
	uint32_t gpiob_en	:1;
	uint32_t gpioc_en	:1;
	uint32_t gpiod_en	:1;
	uint32_t gpioe_en	:1;
	uint32_t gpiof_en	:1;
	uint32_t gpiog_en	:1;
	uint32_t gpioh_en	:1;
	uint32_t gpioi_en	:1;
	uint32_t reserved_1	:3;
	uint32_t crc_en		:1;
	uint32_t reserved_2	:3;
	uint32_t reserved_3	:2;
	uint32_t bkps_ram_en :1;
	uint32_t reserved_4	:1;
	uint32_t ccmd_at_ram_en :1;
	uint32_t dma1_en	:1;
	uint32_t dma2_en	:1;
	uint32_t reserved_5	:2;
	uint32_t eth_mac_en	:1;
	uint32_t eth_mac_tx_en :1;
	uint32_t eth_mac_rx_en :1;
	uint32_t eth_mac_ptp_en :1;
	uint32_t otghs_en	:1;
	uint32_t otghsulpi_en :1;
	uint32_t reserved_6	:1;

} RCC_AHB1ENR_t;*/

// Reference Manual 8.4.1
// GPIO port mode register (GPIOx_MODER) (x=A...i/j/K)
typedef struct {
	uint32_t mode_pin00 :2;
	uint32_t mode_pin01 :2;
	uint32_t mode_pin02 :2;
	uint32_t mode_pin03 :2;
	uint32_t mode_pin04 :2;
	uint32_t mode_pin05 :2;
	uint32_t mode_pin06 :2;
	uint32_t mode_pin07 :2;
	uint32_t mode_pin08 :2;
	uint32_t mode_pin09 :2;
	uint32_t mode_pin10 :2;
	uint32_t mode_pin11 :2;
	uint32_t mode_pin12 :2;
	uint32_t mode_pin13 :2;
	uint32_t mode_pin14 :2;
	uint32_t mode_pin15 :2;
} GPIOx_MODE_t;

typedef struct {
	uint32_t od_pin00 :1;
	uint32_t od_pin01 :1;
	uint32_t od_pin02 :1;
	uint32_t od_pin03 :1;
	uint32_t od_pin04 :1;
	uint32_t od_pin05 :1;
	uint32_t od_pin06 :1;
	uint32_t od_pin07 :1;
	uint32_t od_pin08 :1;
	uint32_t od_pin09 :1;
	uint32_t od_pin10 :1;
	uint32_t od_pin11 :1;
	uint32_t od_pin12 :1;
	uint32_t od_pin13 :1;
	uint32_t od_pin14 :1;
	uint32_t od_pin15 :1;
	uint32_t none :16;

} GPIOx_ODR_t;


// Reference Manual 6.3.26
// RCC register map
typedef struct {
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	uint32_t RESERVED0;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	uint32_t RESERVED1[2];
	volatile uint32_t AHB1ENR;
} RCC_RegDef_t;

#endif /* STM32F407XX_H_ */