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
- Debug/trace register definitions (ITM, TPIU, DEMCR, DBGMCU) in `Platform/debug_regs.h`

The platform layer represents the actual MCU hardware.

Note on provenance: the ITM and TPIU register layouts are not STM32-specific, they belong to the ARM Cortex-M4 core (CoreSight). `DBGMCU_CR` and the DEMCR/DHCSR core-debug registers are documented separately (RM0090 for DBGMCU, the ARMv7-M Architecture Reference Manual for DEMCR/DHCSR, the Cortex-M4 Technical Reference Manual for ITM/TPIU). This split is intentional and should not be "simplified" by pulling in a CMSIS core header; `debug_regs.h` keeps all of it explicit in one place.

---

## Driver Layer

Contains hardware drivers.

Current drivers:

- GPIO driver
- keypad driver
- SWO driver (`Drivers/swo.h` / `Drivers/swo.cpp`)

Responsibilities:

- GPIO configuration
- input reading
- output control
- keypad matrix scanning
- SWO trace initialization and character output (`swoInit()`, `ITM_SendChar()`, `swoPuts()`)

Drivers use the platform layer but do not contain application logic.

The SWO driver is excluded from host-based unit tests, since it writes directly to hardware addresses that do not exist on the host (same rationale as the GPIO/keypad drivers). If `ILogger`/`SwoLoggerAdapter`/`MockLogger` are introduced later so the application layer can log through an interface, the adapter goes in `App/Adapters`, not here.

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

`main()` currently calls `swoInit()`/`swoPuts()` directly, which is acceptable since this is one-off startup diagnostic output in `main()`, not application logic. If SWO output is ever needed from inside the application layer (e.g. logging access attempts), it must go through an `ILogger` interface and adapter, not a direct call to the driver.

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
- SWO runtime debug output (register-level ITM/TPIU configuration, no UART)

---

## Hardware Configuration

Board:

STM32F407G-DISC1

MCU:

- STM32F407VG
- ARM Cortex-M4

Clock:

- Runs on HSI (16 MHz), no PLL configured. Any future clock change must also update `swoInit()`'s core-clock argument and the matching value in `scripts/swo.gdb`, or SWO output breaks.

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

Trace:

- SWO: PB3 (TRACESWO), routed through the onboard ST-LINK over the same USB connection used for flashing/debugging. No separate wiring, no UART.

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

The SWO driver is not part of this host-based suite (see Driver Layer).

---

## Debugging

Debug environment:

- ST-LINK
- OpenOCD
- GDB (`gdb-multiarch` on this machine; `arm-none-eabi-gdb` is not available)

Available scripts:

- scripts/debug.gdb
- scripts/inspect.gdb
- scripts/reset.gdb
- scripts/swo.gdb

Build/debug automation lives in the top-level `Makefile`, not just raw CMake calls. Relevant targets:

- `make configure-firmware` / `make firmware` — configure and build the STM32 target
- `make flash` — program and reset via OpenOCD
- `make openocd` — start the OpenOCD GDB server (keep running in its own terminal)
- `make gdb` / `make debug` / `make reset` / `make reboot` — various GDB sessions
- `make swo` — runs `gdb-multiarch -batch -x scripts/swo.gdb $(ELF)` to flash and start the firmware with SWO configured; read output separately with `tail -f swo.log`

### SWO debug workflow

Three terminals:
`
make openocd # terminal 1, stays running
make swo # terminal 2
tail -f swo.log # terminal 3
`


`scripts/swo.gdb`:

```gdb
target extended-remote :3333
monitor reset halt

monitor tpiu config internal swo.log uart off 16000000 125000
monitor itm port 0 on

load
monitor reset halt
monitor resume
```

Two non-obvious points worth keeping in mind if this script is touched again:

- `monitor resume` is used instead of plain GDB `continue`. In `-batch` mode, GDB detaches as soon as the script finishes, and detaching after `continue` tends to leave the target halted again (`gdb_detach halt` behavior in OpenOCD). `monitor resume` resumes the CPU through OpenOCD itself, so it keeps running after GDB disconnects.
- SWO baud rate is 125000, not the initially planned 2000000. The onboard ST-LINK on this board could not reliably decode SWO at 2 MHz (confirmed by garbled, inconsistent-length output in `swo.log`, no `0x05` ITM header bytes visible in `xxd`). 125 kHz is a conservative, reliable rate for this hardware. The value is duplicated in two places and must stay in sync:
  - `swoInit(16000000, 125000)` in `main()`
  - `monitor tpiu config internal swo.log uart off 16000000 125000` in `scripts/swo.gdb`

If SWO output ever looks like garbage again, check in this order: (1) is the firmware actually running past `swoInit()`/`swoPuts()`, not stuck in a fault; (2) do the core clock values match on both sides; (3) try lowering the SWO baud rate further; (4) check the `ITM->TPR` value (see below).

### Known deviation from the reference sequence

The TPIU/ITM init sequence in `swo.cpp` follows the example configuration from the STM32 reference manual (DBG chapter) and the ARM Cortex-M4 TRM, in this order: enable trace (`DEMCR.TRCENA`), configure TPIU pin/baud, unlock ITM (`LAR`), configure `TCR`, enable stimulus port 0 (`TER`), unmask privilege (`TPR`). The `ITM->TPR` value should be `1` (unmask ports 0-7 for unprivileged code, matching the reference manual's example), not `0`. With `0`, SWO still works today only because `main()` runs in privileged thread mode by default (no RTOS), so double-check this if an RTOS or unprivileged code is ever introduced.

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

Should be added once written up:

- swo.md — register configuration (ITM/TPIU/DEMCR/DBGMCU), host-side OpenOCD/GDB setup, and the baud-rate/`-batch` gotchas described above.

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
- ITM/TPIU/DEMCR/DBGMCU trace and debug registers

Do not replace register access with HAL.

Peripheral/register structs are exposed as `reinterpret_cast`-based pointers at fixed addresses (see `Platform/debug_regs.h` for the SWO-related example). This is a deliberate, explicit cast, not a C-style cast, so that raw address-to-pointer reinterpretation stays grep-able and visually distinct from other casts in the codebase.

---

## Build System

- `CMakeLists.txt` drives both the firmware build and the host test build (`BUILD_TESTS` option), kept in one file rather than split per target.
- Firmware sources, include directories (`Core`, `Platform`, `Drivers`, `App`, `App/Interfaces`, `App/Adapters`), compiler flags (`-mcpu=cortex-m4 -mthumb`, no exceptions/RTTI/unwind tables for C++), and linker flags (custom linker script, `-nostartfiles -nostdlib`) are defined there.
- New driver/platform source files must be added explicitly to the `add_executable()` file list in `CMakeLists.txt` (e.g. `Drivers/swo.cpp`); CMake does not glob sources here by design, for explicitness.
- The top-level `Makefile` wraps CMake configure/build steps and all OpenOCD/GDB workflows (flash, debug, reset, reboot, swo, inspection targets like `size`/`disasm`/`symbols`/`sections`/`elfinfo`). Prefer adding new debug/inspection workflows as Makefile targets rather than ad hoc shell commands, to keep the workflow self-documenting via `make help`.

---

## Future Work

Only add improvements if they preserve the original purpose of the project.

Planned future extensions:

- improve test coverage
- introduce `ILogger` / `SwoLoggerAdapter` / `MockLogger` if application-layer logging over SWO is needed, keeping the application layer free of direct driver calls
- `Docs/swo.md` write-up

Do not change the architecture to hide hardware details.