# STM32F407 Register-Level Access Control System

## Project Overview

This project is a bare-metal access control system for the STM32F407G-DISC1 development board.

The system is implemented without:

- STM32 HAL
- STM32CubeMX
- CMSIS peripheral drivers (except startup and linker related basics)

The goal is to build a complete embedded system from scratch while understanding:

- MCU startup
- memory mapped peripherals
- register-level hardware access
- embedded driver architecture
- hardware abstraction
- testable application design

---

## Current State (IMPORTANT)

The project is functionally complete for the first demonstrable version.

The following features are implemented and validated on real hardware:

- custom startup code
- register-level GPIO driver
- 4x4 matrix keypad input
- PIN authentication
- failed attempt counter
- lockout state
- LED feedback
- host-based application tests
- firmware flashing and debugging via ST-LINK/OpenOCD/GDB

The AccessController application is working on the STM32F407.

---

# Hardware Target

## Board

- STM32F407G-DISC1

## MCU

- STM32F407VG
- ARM Cortex-M4

## LEDs

On-board LEDs:

- Green LED: PD12
- Red LED: PD14

Behaviour:

- Startup: Red LED
- Correct PIN: Green LED
- Wrong PIN: Red LED
- Lockout: Red blinking LED

The blinking is currently implemented using software timing.

Future improvement:
- replace software timing with STM32 hardware timer based timing

---

# Keypad Hardware

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

Key mapping:

    1 2 3 A
    4 5 6 B
    7 8 9 C
    * 0 # D

---

# Toolchain

Used tools:

- arm-none-eabi-gcc
- CMake
- GNU Make
- ST-LINK
- OpenOCD
- gdb-multiarch / arm-none-eabi-gdb

The Makefile is the main entry point.

Common commands:

    make help
    make firmware
    make tests
    make flash
    make openocd
    make gdb

---

# Build System

Firmware build directory:

    build-firmware

Test build directory:

    build-tests

Firmware:

    make firmware

Tests:

    make tests

---

# Architecture

The project follows a layered architecture.

The application layer must not directly access STM32 registers.

---

# Platform Layer

Responsible for direct hardware representation.

Contains:

- GPIO register definitions
- RCC register definitions
- memory mapped addresses
- peripheral register structures

Rules:

- keep hardware definitions minimal
- no application logic
- no STM32 HAL
- no CubeMX generated code

The platform layer represents the actual MCU hardware.

---

# Driver Layer

## GPIO Driver

Current API:

    GPIO_PeriClockControl()
    GPIO_WritePin()
    GPIO_ReadPin()
    GPIO_TogglePin()
    GPIO_InitOutput()
    GPIO_InitInput()
    GPIO_EnablePullUp()

Responsibilities:

- GPIO initialization
- input reading
- output control
- pull-up configuration

---

## Keypad Driver

Interface:

    void Keypad_Init()
    char Keypad_Scan()

Responsibilities:

- matrix scanning
- row activation
- column reading
- pull-up configuration
- debounce handling

The keypad driver uses the GPIO abstraction.

It does not contain application logic.

---

# Application Layer

Hardware independent components:

- AccessController
- PinValidator
- AttemptCounter
- LockState

Interfaces:

- IKeypad
- ILedOutput

Hardware adapters:

- KeypadAdapter
- LedOutputAdapter

The application layer does not directly access STM32 registers.

---

# AccessController Behaviour

Flow:

1. User enters digits.
2. Digits are stored internally.
3. '#' submits the PIN.
4. PIN is validated.
5. LED state changes.

States:

- Locked
- Unlocked
- LockedOut

After three failed PIN attempts:

- state changes to LockedOut
- red LED blinks
- further input is ignored

---

# Testing

Host-based unit tests exist.

Mocks:

- MockKeypad
- MockLedOutput

Current tests cover:

- correct PIN
- wrong PIN
- failed attempts
- lockout behaviour
- invalid PIN length

The tests verify application behaviour without hardware dependency.

Future improvement:

- extend test coverage
- add more state transition tests

---

# Debugging

Working debug setup:

Terminal 1:

    make openocd

Terminal 2:

    make gdb

Typical GDB workflow:

    target remote localhost:3333
    load
    break main
    monitor reset halt
    continue

GDB can currently:

- stop at application code
- inspect variables
- inspect peripheral registers
- debug keypad scanning

---

# Completed Milestones

- [x] Bare-metal project setup
- [x] Custom startup code
- [x] Vector table
- [x] Reset handler
- [x] Linker script
- [x] ARM cross compilation
- [x] Firmware flashing
- [x] OpenOCD debugging
- [x] GDB debugging
- [x] Register-level GPIO driver
- [x] Matrix keypad driver
- [x] Hardware abstraction interfaces
- [x] AccessController implementation
- [x] PIN validation
- [x] Attempt counter
- [x] Lockout handling
- [x] LED feedback
- [x] Host-based unit tests
- [x] Hardware validation on STM32F407

---

# Current Roadmap

## Testing

- [ ] Complete unit tests with mocks

## Debugging

- [ ] Create GDB debug scripts
- [ ] Implement SWO debug output

## Hardware Improvements

- [ ] Replace software timing with STM32 timer based timing

## Release

- [ ] Prepare v1.0 release

---

# Important Design Decisions

Do not introduce:

- STM32 HAL
- CubeMX generated code
- unnecessary frameworks

Maintain:

- register-level understanding
- separation between hardware and application logic
- testable embedded architecture
- simple explicit code

---

# Project Philosophy

This project exists to understand embedded systems from the hardware level upwards.

Focus:

- understanding MCU startup
- understanding memory mapped peripherals
- controlling hardware through registers
- building embedded software without frameworks
- separating hardware details from application behaviour