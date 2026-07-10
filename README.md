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
- gdb-multiarch (Ubuntu) or arm-none-eabi-gdb

## Build Firmware
```bash
cmake -B build-firmware -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
or
cmake -S . -B build-firmware

cmake --build build-firmware
```

## Build Tests
````bash
cmake -S . -B build-tests -D BUILD_TESTS=ON
cmake --build --build-tests
```

## Flash and Debug

Start OpenOCD:
```bash
openocd \
  -f /usr/share/openocd/scripts/interface/stlink.cfg \
  -f /usr/share/openocd/scripts/target/stm32f4x.cfg
```

In a second terminal, start GDB:

```bash
gdb-multiarch build/stm32f407_access_control
```

Inside GDB:

```gdb
target remote localhost:3333
load
break main
monitor reset halt
continue
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

## Project Status

- [x] Bare-metal project setup
- [x] Custom startup code
- [x] Linker script
- [x] CMake build system
- [x] Flash and debug with ST-LINK/OpenOCD
- [x] Register-level GPIO driver
- [x] Matrix keypad driver
- [ ] Access control application
