# Register Map

## Overview

The STM32F407 uses memory-mapped peripherals.

Instead of accessing hardware through a high-level framework, the firmware directly accesses peripheral registers by using their memory addresses.

This project implements the required register definitions manually for the used peripherals.

---

## Memory-Mapped I/O

In a memory-mapped architecture, hardware registers are mapped into the MCU address space.

Reading or writing a specific memory address accesses a hardware register.

The general structure is:

```text
Hardware Register Address

↓

C pointer

↓

Register access from firmware
```

---

## Peripheral Base Addresses

The STM32F407 organizes peripherals using fixed base addresses.

The project defines the required peripheral addresses manually.

Used peripherals:

| Peripheral | Base address | Purpose |
|---|---|---|
| RCC | 0x40023800 | Enable peripheral clocks |
| GPIO | 0x40020000 + port offset | Configure and control GPIO pins |
| SYSCFG | 0x40013800 | Route GPIO pins to EXTI lines |
| EXTI | 0x40013C00 | External interrupt configuration |
| NVIC | 0xE000E100 (ISER) | Enable interrupts in the core |
| SysTick | 0xE000E010 | 1 ms time base |
| SCB (fault status) | 0xE000ED28 | Fault diagnostics |
| ITM / TPIU / DEMCR / DBGMCU | see [SWO Debugging](swo.md) | Trace output |

The registers of NVIC, SysTick, SCB, ITM, TPIU and DEMCR belong to the Cortex-M4 core, not to the STM32 peripheral set.

---

## Register Structures

Hardware registers are represented using C structures.

Example:

A GPIO peripheral consists of several registers:

- mode configuration
- output data
- input data
- pull-up/pull-down configuration

The structure layout matches the memory layout of the actual MCU peripheral.

This allows register access using normal C syntax.

---

## GPIO Registers

The project uses GPIO registers for:

- keypad input
- keypad row control
- LED output

Important GPIO registers:

| Register | Purpose |
|---|---|
| MODER | Configure pin mode |
| OTYPER | Configure output type |
| OSPEEDR | Configure output speed |
| PUPDR | Configure pull-up/pull-down resistors |
| IDR | Read input state |
| ODR | Write output state |

---

## RCC Clock Control

Before a peripheral can be used, its clock must be enabled.

The firmware enables GPIO clocks through the RCC peripheral.

The sequence is:

1. Enable GPIO peripheral clock
2. Configure GPIO registers
3. Access GPIO pins

The SYSCFG peripheral is clocked through `RCC->APB2ENR` (bit `SYSCFGEN`). It must be enabled before `SYSCFG->EXTICR` can be written.

---

## SYSCFG Registers

SYSCFG selects which GPIO port is connected to each EXTI line.

| Register | Purpose |
|---|---|
| EXTICR1 | Port selection for EXTI0-3 |
| EXTICR2 | Port selection for EXTI4-7 |
| EXTICR3 | Port selection for EXTI8-11 |
| EXTICR4 | Port selection for EXTI12-15 |

Each EXTI line has a 4-bit field. EXTI line *n* is always connected to pin *n* of the selected port, so the keypad columns PD8-PD11 use EXTI8-EXTI11 with port D selected in `EXTICR3`.

---

## EXTI Registers

EXTI detects edges on the selected pins and generates interrupt requests.

| Register | Purpose |
|---|---|
| IMR | Interrupt mask (1 = line enabled) |
| RTSR | Rising trigger selection |
| FTSR | Falling trigger selection |
| SWIER | Software interrupt trigger |
| PR | Pending flag, cleared by writing 1 |

The keypad uses a falling edge on lines 8-11. The interrupt handlers clear the pending flag in `PR` and set a flag for the main loop. `IMR` is masked while the keypad is scanned. `SWIER` allows triggering a line from software, which is useful for tests and debugging.

---

## NVIC Registers

NVIC is part of the Cortex-M4 core, not of the STM32 peripheral set.

| Register | Address | Purpose |
|---|---|---|
| ISER0 | 0xE000E100 | Enable IRQ 0-31 |
| ISER1 | 0xE000E104 | Enable IRQ 32-63 |

The keypad interrupts are enabled through:

| IRQ | Handler | Register and bit |
|---|---|---|
| 23 | `EXTI9_5_IRQHandler` | ISER0, bit 23 |
| 40 | `EXTI15_10_IRQHandler` | ISER1, bit 8 |

---

## SysTick Registers

SysTick is a 24-bit down-counter in the Cortex-M4 core. It generates the 1 ms time base for the lockout timeout. Its interrupt is an exception (position 15 of the vector table), not an NVIC IRQ, so no `ISER` bit is involved.

| Register | Address | Purpose |
|---|---|---|
| SYST_CSR | 0xE000E010 | Control and status |
| SYST_RVR | 0xE000E014 | Reload value |
| SYST_CVR | 0xE000E018 | Current value, writing clears it |

`SYST_CSR` bits used:

| Bit | Name | Value | Meaning |
|---|---|---|---|
| 0 | ENABLE | 1 | counter running |
| 1 | TICKINT | 1 | interrupt on reaching zero |
| 2 | CLKSOURCE | 1 | processor clock |

Configuration for a 1 ms tick at the 16 MHz HSI clock:

```text
SYST_RVR = 16000000 / 1000 - 1 = 15999
SYST_CVR = 0
SYST_CSR = 0x7
```

The reload value is clock-dependent. If the system clock is changed, `kCoreClockHz` in `SysTickClock.cpp` must be changed as well.

---

## SCB Fault Status Registers

The System Control Block holds the registers used to analyse a fault in the `HardFault_Handler`.

| Register | Address | Purpose |
|---|---|---|
| CFSR | 0xE000ED28 | Configurable fault status (MemManage bits 0-7, BusFault bits 8-15, UsageFault bits 16-31) |
| HFSR | 0xE000ED2C | HardFault status (bit 30 FORCED: escalated from another fault) |
| MMFAR | 0xE000ED34 | Address of a MemManage fault |
| BFAR | 0xE000ED38 | Address of a BusFault |

`MMFAR` and `BFAR` are only valid if the corresponding valid bit (`MMARVALID`, `BFARVALID`) in `CFSR` is set.

---

## Volatile Access

Hardware registers are declared as volatile.

This prevents the compiler from optimizing away register accesses.

Hardware values can change independently from the executing code.

Example:

- input pins can change because of external signals
- output registers must be written directly to hardware
- the EXTI pending flag is set by hardware and cleared by the interrupt handler
- the SysTick counter variable is written in an interrupt handler and read in the main loop

---

## Register-Level Driver Layer

The register definitions are separated from application logic.

The architecture is:

```text
Application

↓

Drivers

↓

Register Definitions

↓

STM32 Hardware
```

The application layer does not directly access MCU registers.

---

## Design Decision

Direct register access was chosen to understand:

- memory-mapped peripherals
- MCU register layout
- GPIO configuration
- interrupt routing from pin to EXTI to NVIC
- core timer and fault registers
- hardware abstraction
- embedded driver design

The project intentionally avoids STM32 HAL and CubeMX generated peripheral code.