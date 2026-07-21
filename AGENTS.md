# STM32F407 Register-Level Access Control System - Agent Notes

## Project Status

This project is a completed demonstrable bare-metal embedded system for the STM32F407G-DISC1.

The firmware has been implemented, tested on real hardware and documented.

The goal is to demonstrate understanding of embedded systems from MCU startup to application logic.

---

## Important Constraints

Do not introduce:

- STM32 HAL
- STM32CubeMX generated code
- unnecessary embedded frameworks

Maintain:

- register-level hardware access
- clear separation between hardware and application logic
- simple explicit embedded C/C++ code
- testable application architecture

---

## Current Architecture

The project uses a layered architecture.

## Platform Layer

Responsible for direct hardware representation.

Contains:

- STM32 register definitions
- peripheral addresses
- RCC definitions
- GPIO register structures

The platform layer represents the actual MCU hardware.

---

## Driver Layer

Contains hardware drivers.

Current drivers:

- GPIO driver
- keypad driver

Responsibilities:

- GPIO configuration
- input reading
- output control
- keypad matrix scanning

Drivers use the platform layer but do not contain application logic.

---

## Application Layer

Hardware-independent application logic:

- AccessController
- PinValidator
- AttemptCounter
- LockState

Interfaces:

- IKeypad
- ILedOutput

Adapters:

- KeypadAdapter
- LedOutputAdapter

The application layer must never directly access STM32 registers.

---

## Implemented Features

The following functionality is complete:

- custom startup code
- vector table
- reset handler
- custom linker script
- register-level GPIO driver
- 4x4 matrix keypad input
- PIN authentication
- failed attempt counter
- lockout handling
- LED feedback
- host-based unit tests
- ST-LINK/OpenOCD/GDB debugging
- hardware validation on STM32F407G-DISC1

---

## Hardware Configuration

Board:

STM32F407G-DISC1

MCU:

- STM32F407VG
- ARM Cortex-M4

LEDs:

- Green LED: PD12
- Red LED: PD14

Keypad:

4x4 matrix keypad.

Connections:

Rows:

- R1: PD0
- R2: PD1
- R3: PD2
- R4: PD3

Columns:

- C1: PD8
- C2: PD9
- C3: PD10
- C4: PD11

---

## Testing

Host-based tests exist using mocks:

- MockKeypad
- MockLedOutput

Tests cover:

- correct PIN handling
- wrong PIN handling
- failed attempts
- lockout behaviour
- invalid PIN lengths

The purpose is to test application logic without hardware dependency.

---

## Debugging

Debug environment:

- ST-LINK
- OpenOCD
- GDB

Available scripts:

- scripts/debug.gdb
- scripts/inspect.gdb
- scripts/reset.gdb

---

## Documentation Status

Documentation is located in:

Docs/

Current documents:

- architecture.md
- hardware.md
- keypad_connection.md
- startup.md
- linker-script.md
- register_map.md

The README links to these documents.

Documentation style:

- written as completed technical documentation
- avoid excessive bullet-only descriptions
- focus on explaining design decisions and architecture

---

## Startup Implementation

The project uses a custom startup implementation.

Important concepts:

- Vector Table
- Reset Handler
- .data initialization
- .bss initialization
- linker script interaction

The startup code currently performs:

1. Load stack pointer
2. Execute Reset Handler
3. Copy initialized data from Flash to RAM
4. Clear BSS section
5. Start main()

No full C++ runtime is used.

cpp_runtime.cpp only provides required embedded C++ symbols.

---

## Memory Layout

The linker script defines:

Flash:

- start: 0x08000000
- size: 1024 KB

RAM:

- start: 0x20000000
- size: 128 KB

Important linker symbols:

- _estack
- _sidata
- _sdata
- _edata
- _sbss
- _ebss

---

## Register-Level Implementation

The project uses memory-mapped peripheral access.

Important concepts:

- peripheral base addresses
- register structures
- volatile hardware access
- GPIO configuration registers
- RCC clock control

Do not replace register access with HAL.

---

## Future Work

Only add improvements if they preserve the original purpose of the project.

planned future extensions:

- improve test coverage
- add SWO debugging

Do not change the architecture to hide hardware details.