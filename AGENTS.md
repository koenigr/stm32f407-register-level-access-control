# AGENTS.md

## Project Overview

This project is a bare-metal access control system for the STM32F407G-DISC1 development board.

It is implemented completely without:
- STM32 HAL
- STM32CubeMX
- CMSIS drivers (except startup/linker basics)

The goal is to build a register-level embedded system from scratch, including GPIO drivers, keypad input, and simple access control logic.

---

## Current State (IMPORTANT)

The project is already fully set up for bare-metal execution:

### Toolchain & Build
- CMake-based build system (arm-none-eabi-gcc)
- Compiles successfully to ELF
- GitHub Actions CI builds successfully

### Low-Level Startup
- Custom startup file implemented (`Reset_Handler`)
- Vector table defined manually
- `.data` copy and `.bss` zero-initialization implemented
- Program successfully reaches `main()`

### Debugging
- OpenOCD + ST-LINK working
- GDB (gdb-multiarch) connected successfully
- Breakpoints in `main()` work
- Firmware is flashed and runs on hardware

---

## Hardware Target

- STM32F407G-DISC1
- Cortex-M4 (ARMv7E-M)
- Memory-mapped peripheral access

---

## Architecture (Planned Structure)

The project is structured in layers:

### 1. Startup Layer
- Reset handler
- Vector table
- Linker script

### 2. Platform Layer (Low-Level Hardware Access)
- GPIO register definitions
- Memory-mapped peripheral structs
- Base address macros

### 3. Driver Layer
- GPIO driver (planned next)
- Keypad driver (4x4 matrix keypad)

### 4. Application Layer
- Access control logic
- PIN validation
- Lockout mechanism

---

## Constraints

- No HAL usage
- No CubeMX-generated code
- Direct register access only
- ARM GCC toolchain only
- CMake build system required
- Must run bare-metal on MCU

---

## Completed Milestones

- [x] CMake toolchain setup
- [x] ARM cross-compilation working
- [x] Linker script implemented
- [x] Startup assembly implemented
- [x] Reset handler with runtime init
- [x] Firmware flashes to MCU
- [x] Debugging via OpenOCD + GDB
- [x] Execution reaches `main()`

---

## Next Step (CURRENT FOCUS)

Implement GPIO register definitions:

- Define GPIO base addresses (GPIOA, GPIOB, ...)
- Define `GPIO_RegDef_t` structure
- Map registers using volatile pointers
- Ensure correctness against STM32F407 reference manual
- Prepare foundation for GPIO driver layer

---

## Design Principles

- Prefer register-level clarity over abstraction
- Keep drivers minimal and explicit
- Avoid hidden initialization magic
- Debug via GDB/register inspection, not prints (initially)
- Each layer must be testable independently

---

## Debug Workflow Reminder

- OpenOCD runs ST-LINK bridge
- GDB connects via localhost:3333
- Firmware loaded via `load`
- Execution starts at `Reset_Handler`

Typical commands:

```gdb
target remote localhost:3333
load
break main
monitor reset halt
continue
```

---

## Important Note

This project is intentionally built "from scratch" to understand:
- how microcontrollers boot
- how memory maps work
- how peripherals are accessed
- how a minimal embedded system is structured without frameworks