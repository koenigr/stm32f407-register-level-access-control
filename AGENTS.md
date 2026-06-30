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

The project is fully operational at bare-metal level and currently transitioning from raw register access to a structured GPIO abstraction layer.

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
- Register inspection via GDB is functional

---

## Hardware Target

- STM32F407G-DISC1
- Cortex-M4 (ARMv7E-M)
- Memory-mapped peripheral access

---

## Architecture (Current + Evolving)

The project is structured in layers:

### 1. Startup Layer
- Reset handler
- Vector table
- Linker script

### 2. Platform Layer (Low-Level Hardware Access)
Responsible for direct hardware mapping.
- GPIO register definitions (GPIO_RegDef_t)
- RCC register definitions (in progress / minimal usage)
- Memory-mapped base address macros
- Direct register-to-struct mapping

Two approaches currently exist in the codebase:

- Raw pointer-based register access (legacy / CubeIDE style)
- Typed struct-based register abstraction (current direction)

### 3. Driver Layer (Planned/Starting Next)
- GPIO driver API
  - GPIO_WritePin()
  - GPIO_ReadPin()
  - GPIO_TogglePin()
- Future: GPIO_Init()

### 4. Application Layer
- Access control logic (future)
- Keypad scanning logic (to be migrated from CubeIDE code)
- PIN validation
- Lockout mechanism

---

## Constraints

- No HAL usage
- No CubeMX-generated code
- No CMSIS peripheral drivers (startup/linker exceptions allowed)
- Direct register access only via Platform layer
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
- [x] GPIO register struct abstraction introduced
- [x] GDB-based register inspecton working

---

## Next Step (CURRENT FOCUS)

### GPIO Platform Layer Completion
- Define complete GPIO register mapping (GPIOA, GPIOB, ...)
- Validate GPIO_RegDef_t against STM32F407 reference manual
- Ensure correct volatile usage for all registers
- Clean separation between:
  - raw register access
  - future GPIO driver API
### Preparation for Driver Layer
- Replace CubeIDE-style register code with structured GPIO API
- Prepare migration path for keypad driver integration

---

## Design Principles

- Prefer register-level clarity over abstraction
- Keep drivers minimal and explicit
- Avoid hidden initialization magic
- Debug via GDB/register inspection, not prints (initially)
- Each layer must be testable independently
- Platform layer defines hardware reality, drivers consume it

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