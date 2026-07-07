# AGENTS.md

## Project Overview

This project is a bare-metal access control system for the STM32F407G-DISC1 development board.

It is implemented completely without:
- STM32 HAL
- STM32CubeMX
- CMSIS drivers (except startup/linker basics)

The goal is to build a register-level embedded system from scratch, including
- startup code
- GPIO drivers
- keypad input handling
- access control application logic

---

## Current State (IMPORTANT)

The project is fully operational at bare-metal level.

The low-level hardware abstraction is stable, and the project has moved from GPIO development into driver and application-layer development.

Current working features:
- MCU startup
- GPIO abstraction
- LED output test
- GPIO input handling
- 4x4 matrix keypad scanning

### Toolchain & Build
- CMake-based build system (arm-none-eabi-gcc)
- Out-of-source builds (build/ directory)
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

## Architecture

The project is structured in layers:

### 1. Startup Layer
- Reset handler
- Vector table
- Linker script
- Memory initialization


### 2. Platform Layer (Low-Level Hardware Access)
Responsible for direct hardware mapping.
- GPIO register definitions (GPIO_RegDef_t)
- RCC register definitions (in progress / minimal usage)
- Memory-mapped base address macros
- Direct register-to-struct mapping

This layer deifnes the hardware reality and must stay minimal and hardware-accurate

### 3. Driver Layer

Current GPIO driver API:
- GPIO_periClockControl()_
- GPIO_WritePin()
- GPIO_ReadPin()
- GPIO_TogglePin()
- GPIO_InitOutput()
- GPIO_InitInput()
- GPIO_EnablePullUp()

### 4. Keypad Driver
Current Interface:
- void Keypad_Init(void)
- char Keypad_Scan(void)

Features:
- 4x4 matrix scanning
- Row output control
- Column input reading
- Pull-up configuration
- Simple debounce handling
- Key mapping:
```text
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
```


### 5. Application Layer (Future)
- Access control logic
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
- [x] First GPIO driver functions implemented

---

## Next Step (CURRENT FOCUS)
### Keypad Driver Integration (Application Layer Start)

GPIO driver layer is considered functionally complete for basic output/input use cases.

Next milestone focuses on migrating from LED test to real input handling:

- Port CubeIDE keypad scanning code into current architecture
- Implement 4x4 keypad scanning using GPIO_ReadPin / GPIO_WritePin
- Define keypad matrix mapping (rows / columns)
- Add basic debounce handling (simple delay-based approach)
- Validate correct key detection via GDB / simple output

### GPIO Layer Status
GPIO driver is stable for:
- Clock control
  - Basic output (LED control)
  - Basic input read
- No further GPIO abstraction changes planned at this stage

## Preparation for Application Layer
- Remove remaining raw register access from keypad-related logic (future step)
- Replace CubeIDE-style scanning with GPIO driver API
- Define keypad module interface (Keypad_Init / Keypad_Scan)
- Prepare structure for access control logic (PIN entry system)

## Design Focus (Shifted)
- GPIO layer is now treated as stable platform abstraction
- Focus shifts to input handling + system logic
- Keep driver API unchanged unless hardware requirement forces it

---

## Build Workflow Reminder

Build only:
```bash
cmake --build build
```

Reconfigure (if CMakeLists changed):
```bash
cmake -S . -B build
cmake --build build
```

Flash without GDB:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
    -c "program build/stm32f407_access_control.elf verify reset exit"
```

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