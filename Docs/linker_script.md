# Linker Script

## Overview

The linker script defines how the firmware is organized in the memory of the STM32F407VG.

It specifies:

- Flash layout
- RAM layout
- placement of code and data sections
- memory symbols used during startup

---

## Memory Layout

The STM32F407VG provides:

| Region | Address | Size |
|---|---|---|
| Flash | 0x08000000 | 1024 KB |
| RAM | 0x20000000 | 128 KB |

Flash is used for program code and constant data.

RAM is used for runtime data such as variables and the stack.

---

## Sections

The firmware is divided into several memory sections.

### .text

Contains:

- program code
- interrupt vector table
- read-only data

This section is stored in Flash.

---

### .data

Contains initialized global and static variables.

The initial values are stored in Flash and copied to RAM during startup.

Example:

int counter = 10;

The value 10 is stored in Flash initially and copied into RAM before main() starts.

---

### .bss

Contains uninitialized global and static variables.

These variables are placed in RAM and cleared to zero during startup.

Example:

int counter;

---

## Startup Interaction

The linker script provides symbols used by the startup code:

-`_estack`
- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`

These symbols allow the Reset Handler to initialize memory before the application starts.

---

## Stack

The stack is placed at the end of RAM.

The initial stack pointer is defined by:

```c
_estack = ORIGIN(RAM) + LENGTH(RAM);

```
The Cortex-M4 loads this address from the vector table after reset.

---

## Design Decision

A custom linker script is used to understand:

- MCU memory organization
- relationship between Flash and RAM
- startup initialization
- firmware image layout