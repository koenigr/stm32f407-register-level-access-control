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
- GNU Make

## Build

The project provides a Makefile as the main entry point.

Show available commands:
```bash
make help
```

### Build Firmware
```bash
make firmware
```

### Build and run host tests
```bash
make tests
```

### Rebuild firmware
```bash
make rebuild-firmware
```

### Rebuild tests
```bash
make rebuild-tests
```

### Clean build artifact
```bash
make clean
```

### Remove all build directories
```bash
make clean-all
```

## Flash and Debug

### Flash firmware
```bash
make flash
```

### Start OpenOCD
```bash
make openocd
```

### Start GDB
```bash
make gdb
```

## Features
- Register-level GPIO programming
- Custom GPIO driver
- Matrix keypad driver
- PIN authentication
- Lockout protection
- LED Feedback
  - Green LED: valid PIN
  - Red LED: invalid PIN
  - Red blinking LED: lockout state
- Host-based unit tests for application logic

## Architecture

The project separates application logic from hardware-specific code.

### Application layer
- AccessController
- PinValidator
- AttemptCounter

### Hardware interfaces
- IKeypad
- ILedOutput

### Hardware adapters
- KeypadAdapter
- LedOutputAdapter

The application layer does not directly access STM32 registers.

A detailed description of the architecture and design decisions can be found here:
[Architecture Documentation](Docs/archictecture.md)

## Concepts Demonstrated
- Memory-Mapped I/O
- Pointer Casting
- Bitwise Operations
- Struct-Based Register Mapping
- Volatile Qualifier
- Embedded Driver Architecture
- State Machine Design
- Layered Software Architecture
- Hardware Abstraction
- Dependency Inversion
- Unit Testing with Mocks

## Project Status

- [x] Bare-metal project setup
- [x] Custom startup code
- [x] Linker script
- [x] CMake build system
- [x] Makefile build workflow
- [x] Flash and debug with ST-LINK/OpenOCD
- [x] Register-level GPIO driver
- [x] Matrix keypad driver
- [x] Access control application
- [x] Hardware validation on STM32F407

## Roadmap
- [ ] Extend unit test coverage
- [ ] Create GDB debug scripts
- [ ] Implement SWO debug output
- [ ] Prepare v1.0 release