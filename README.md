# STM32F407 Register-Level Access Control System
Bare-metal STM32F407 access control system implemented without HAL or CubeMX.

## Target Hardware
- STM32F407G-DISC1
- ARM Cortex-M4

## Toolchain
- arm-none-eabi-gcc
- CMake
- ST-LINK
- OpenOCD
- GDB

## Build
```
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
cmake --build build
```

## Features
- Register-level GPIO programming
- Custom GPIO driver
- Matrix keypad driver
- PIN authentication
- Lockout protection
- Admin reset button
- No HAL / No CubeMX

## Concepts Demonstrated
- Memory-Mapped I/O
- Pointer Casting
- Bitwise Operations
- Struct-Based Register Mapping
- Volatile Qualifier
- Embedded Driver Architecture
- State Machine Design
