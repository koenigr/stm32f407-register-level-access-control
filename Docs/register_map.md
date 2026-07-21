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

| Peripheral | Purpose |
|---|---|
| RCC | Enable peripheral clocks |
| GPIO | Configure and control GPIO pins |

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

---

## Volatile Access

Hardware registers are declared as volatile.

This prevents the compiler from optimizing away register accesses.

Hardware values can change independently from the executing code.

Example:

- input pins can change because of external signals
- output registers must be written directly to hardware

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
- hardware abstraction
- embedded driver design

The project intentionally avoids STM32 HAL and CubeMX generated peripheral code.
