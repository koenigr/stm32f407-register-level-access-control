# Startup Code

## Overview

The project uses a custom startup implementation instead of generated startup code.

The startup code is responsible for preparing the STM32F407 before the C++ application starts.

The startup process includes:

- vector table initialization
- reset handling
- RAM initialization
- C++ runtime preparation
- starting the application

---

## Startup Flow

The boot sequence follows this flow:

```text
Reset

↓

Vector Table

↓

Reset Handler

↓

Initialize .data section

↓

Initialize .bss section

↓

Start main()
```
---

## Vector Table

The vector table is placed at the beginning of the firmware image.

It contains:

- initial stack pointer address
- reset handler address
- exception handlers

Currently, unused exception handlers are mapped to a default handler.

---

## Reset Handler

After reset, the MCU starts execution in the Reset Handler.

The Reset Handler performs the required memory initialization before entering the application.

### Data Section Initialization

The `.data` section contains initialized global and static variables.

Because the initial values are stored in Flash, they are copied into RAM during startup.

The process is:

```text
Flash

↓

RAM (.data)
```
---

### BSS Initialization

The `.bss` section contains uninitialized global and static variables.

The startup code clears this memory region before the application starts.

All values are initialized to zero.

---

## Memory Layout

The linker script defines the memory regions:

| Region | Address | Size |
|---|---|---|
| Flash | 0x08000000 | 1024 KB |
| RAM | 0x20000000 | 128 KB |

The stack is placed at the end of RAM.

---

## C++ Runtime

The project uses C++ for the application layer.

Because no full embedded C++ runtime is used, a minimal runtime implementation is provided.

The runtime handles required C++ symbols such as:

- pure virtual function handling
- unused delete operators

---

## Application Startup

After memory initialization, the startup code calls:

```c
main()
```

The application then initializes:

- keypad adapter
- LED adapter
- access controller

The firmware enters the main loop and continuously processes:

- keypad input
- authentication logic
- LED updates

---

## Design Decision

A custom startup implementation was chosen to understand:

- MCU boot process
- vector table handling
- memory initialization
- linker script interaction
- transition from hardware startup to C++ application code