#include "stm32f407xx.h"
#include "stm32f407xx_exti.h"
#include "cortex_m4.h"

#include <gtest/gtest.h>

extern "C" {
RCC_RegDef_t test_rcc;
SYSCFG_TypeDef test_syscfg;
EXTI_TypeDef test_exti;
volatile uint32_t test_nvic_iser[2];
}

class ExtiTest : public ::testing::Test {
protected:
	void SetUp() override {
		test_rcc = RCC_RegDef_t{};
		test_syscfg = SYSCFG_TypeDef{};
		test_exti = EXTI_TypeDef{};
		test_nvic_iser[0] = 0;
		test_nvic_iser[1] = 0;
	}
};

TEST_F(ExtiTest, NvicEnableIrq23SetsIser0Bit23) {
	EXTI_NVIC_Enable(23);

	EXPECT_EQ(test_nvic_iser[0], 1U << 23);
	EXPECT_EQ(test_nvic_iser[1], 0U);
}

TEST_F(ExtiTest, NvicEnableIrq40SetsIser1Bit8) {
	EXTI_NVIC_Enable(40);

	EXPECT_EQ(test_nvic_iser[0], 0U);
	EXPECT_EQ(test_nvic_iser[1], 1U << 8);
}

TEST_F(ExtiTest, MapPortSetsFieldInExticr3) {
	test_syscfg.EXTICR[2] = 0xFFFFFFFFU;

	EXTI_MapPort(9, 3);

	// line 9 = bits 4..7 of EXTICR3, other fields untouched
	EXPECT_EQ(test_syscfg.EXTICR[2], 0xFFFFFF3FU);
}

TEST_F(ExtiTest, MapPortUsesCorrectFieldForLines8To11) {
	EXTI_MapPort(8, 3);
	EXTI_MapPort(9, 3);
	EXTI_MapPort(10, 3);
	EXTI_MapPort(11, 3);

	EXPECT_EQ(test_syscfg.EXTICR[2], 0x3333U);
}

TEST_F(ExtiTest, MapPortEnablesSyscfgClock) {
	EXTI_MapPort(8, 3);

	EXPECT_EQ(test_rcc.APB2ENR & (1U << 14), 1U << 14);
}

TEST_F(ExtiTest, SetFallingEdgeSetsFtsrAndClearsRtsr) {
	test_exti.RTSR = (1U << 8);

	EXTI_SetFallingEdge(8);

	EXPECT_EQ(test_exti.FTSR, 1U << 8);
	EXPECT_EQ(test_exti.RTSR, 0U);
}

TEST_F(ExtiTest, EnableUnmasksLine) {
	EXTI_Enable(10);

	EXPECT_EQ(test_exti.IMR, 1U << 10);
}