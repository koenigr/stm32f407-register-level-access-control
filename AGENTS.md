# STM32F407 Register-Level Access Control System - Agent Notes

## Project Status

This project is a completed demonstrable bare-metal embedded system for the STM32F407G-DISC1.

The firmware has been implemented, tested on real hardware and documented. Since the first version, the keypad input was reworked from polling to interrupt-driven (EXTI), PIN input validation was unified, a time-based lockout (SysTick clock) was added, and a HardFault handler with SWO crash dump was introduced. See the sections below.

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

- STM32 register definitions (`Platform/stm32f407xx.h`)
- peripheral addresses
- RCC definitions (including `APB2ENR` for SYSCFG)
- GPIO register structures
- SYSCFG (`EXTICR1-4`), EXTI (`IMR`, `RTSR`, `FTSR`, `SWIER`, `PR`) and NVIC (`ISER`) register definitions
- EXTI helper functions (`Platform/stm32f407xx_exti.c` / `.h`)
- Debug/trace register definitions (ITM, TPIU, DEMCR, DBGMCU) in `Platform/debug_regs.h`
- System Control Block fault status registers (`CFSR`, `HFSR`, `MMFAR`, `BFAR`), used by the HardFault handler

The platform layer represents the actual MCU hardware.

Note on provenance: the ITM and TPIU register layouts are not STM32-specific, they belong to the ARM Cortex-M4 core (CoreSight). `DBGMCU_CR` and the DEMCR/DHCSR core-debug registers are documented separately (RM0090 for DBGMCU, the ARMv7-M Architecture Reference Manual for DEMCR/DHCSR, the Cortex-M4 Technical Reference Manual for ITM/TPIU). The same applies to SysTick and the SCB fault registers (ARMv7-M ARM). This split is intentional and should not be "simplified" by pulling in a CMSIS core header; the register definitions stay explicit in one place.

---

## Driver Layer

Contains hardware drivers.

Current drivers:

- GPIO driver
- keypad driver (interrupt-driven)
- SWO driver (`Drivers/swo.h` / `Drivers/swo.cpp`)
- fault handler (`Drivers/fault.cpp`, HardFault crash dump over SWO)

Responsibilities:

- GPIO configuration
- input reading
- output control
- keypad matrix scanning, triggered by column interrupts (EXTI8-11)
- SWO trace initialization and character output (`swoInit()`, `ITM_SendChar()`, `swoPuts()`)
- fault diagnostics (stacked register frame and fault status registers)

Drivers use the platform layer but do not contain application logic.

### Keypad driver (interrupt-driven)

- Rows PD0-PD3 are outputs, columns PD8-PD11 are inputs with pull-up.
- In idle all rows are driven LOW (`Keypad_SetIdle()`, called at the end of `Keypad_Init()`), so any key press pulls a column LOW and triggers a falling-edge interrupt.
- Columns are routed to EXTI8-11 via `SYSCFG->EXTICR3` (port D), falling edge via `FTSR`, unmasked via `IMR`.
- NVIC: `EXTI9_5_IRQHandler` (IRQ 23, `ISER0` bit 23) and `EXTI15_10_IRQHandler` (IRQ 40, `ISER1` bit 8).
- The ISRs only clear `EXTI->PR` and set a flag. Debouncing and the matrix scan run in the main loop, with `IMR` masked during the scan.
- `main()` sleeps with `wfi` and processes key events via `Keypad_ReadEvent()`. `Keypad_IrqPending()` exposes the flag.
- The application layer reaches the driver only through `KeypadAdapter::Scan`.

Note: with SysTick running at 1 kHz, `wfi` returns every millisecond. The main loop therefore runs `AccessController::Update()` at least once per ms, which the lockout timeout relies on (see below). `Update()` with no key pressed must stay cheap.

The SWO driver is excluded from host-based unit tests, since it writes directly to hardware addresses that do not exist on the host (same rationale as the GPIO/keypad drivers). If `ILogger`/`SwoLoggerAdapter`/`MockLogger` are introduced later so the application layer can log through an interface, the adapter goes in `App/Adapters`, not here.

### Fault handling (HardFault)

`Drivers/fault.cpp` overrides the weak `HardFault_Handler` alias from the vector table. It consists of two parts:

- `HardFault_Handler` is a naked trampoline. It checks bit 2 of `LR` (EXC_RETURN) to find out whether the faulting code used MSP or PSP, and passes the pointer to the stacked exception frame in `r0` to the C function.
- `HardFault_Handler_C(uint32_t* frame)` prints the stacked `R0-R3`, `R12`, `LR`, `PC`, `xPSR` and the fault status registers `CFSR`, `HFSR`, `MMFAR`, `BFAR` over SWO, then loops forever.

The handler must not use the stack-heavy or interrupt-dependent parts of the application (no keypad, no `AccessController`). It only needs the SWO driver, which is polling-based.

How to use it:

1. Run the SWO workflow (`make openocd`, `make swo`, `tail -f swo.log`).
2. On a crash the dump appears in `swo.log`.
3. Resolve the faulting address: `arm-none-eabi-addr2line -e build-firmware/stm32f407_access_control.elf <PC>` (also useful for `LR`).
4. Read `CFSR` with the ARMv7-M ARM table: bits 0-7 MemManage, 8-15 BusFault, 16-31 UsageFault (e.g. `DIVBYZERO`, `UNALIGNED`, `INVSTATE`, `PRECISERR`). `BFAR`/`MMFAR` are only valid if the matching valid bit in `CFSR` is set.

Deliberate test (remove afterwards): write to an invalid address, e.g. `*reinterpret_cast<volatile uint32_t*>(0xFFFFFFF0U) = 0U;`. With BusFault not separately enabled this escalates to HardFault. Expected: dump in `swo.log` with `PC` pointing at the store instruction.

The SWO baud/core-clock rules of the SWO section apply to the dump as well. If the output is garbage, check the same points.

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
- IClock (`uint32_t NowMs() const`, monotonic millisecond counter)

Adapters:

- KeypadAdapter
- LedOutputAdapter
- SysTickClock (firmware only, not part of the host tests)

The application layer must never directly access STM32 registers.

`SysTickClock` lives in `App/Adapters` but touches SysTick registers directly. It is the boundary between application and hardware, so this is acceptable for now. If more hardware time functions are needed, move the register access into `Drivers/systick.*` and let the adapter call that.

`main()` currently calls `swoInit()`/`swoPuts()` directly, which is acceptable since this is one-off startup diagnostic output in `main()`, not application logic. If SWO output is ever needed from inside the application layer (e.g. logging access attempts), it must go through an `ILogger` interface and adapter, not a direct call to the driver.

### AccessController behaviour

States: `Locked`, `Unlocked`, `LockedOut`.

Construction: `AccessController(IKeypad&, ILedOutput&, IClock&)`. In `main()`:

```cpp
SysTickClock::Init();          // before the main loop
SysTickClock clock;
AccessController controller{keypadAdapter, ledAdapter, clock};
```

Input handling (identical in `Locked` and `Unlocked`, implemented in `ProcessKey()`):

- digits `0-9` are appended to the PIN buffer
- `#` submits
- any other key (`A-D`, `*`, ...) aborts the current input and clears the buffer
- a fifth digit also aborts the input (no silent truncation, `12345#` is invalid)
- a PIN with fewer than four digits is rejected on `#`
- aborted, too short and too long inputs do not count as failed attempts; only a complete four-digit wrong PIN does

Locked:

- correct PIN: green LED, attempt counter reset, state `Unlocked`
- wrong PIN: red LED, counter incremented
- third wrong PIN: state `LockedOut`, red LED blinks, `lockoutStartMs_` is set

Unlocked:

- a valid four-digit input followed by `#` replaces the PIN (RAM only, lost on reset), sets state `Locked`, red LED
- invalid or incomplete input keeps the system `Unlocked`; the old PIN stays valid until a valid new one is submitted

LockedOut:

- the keypad is not scanned, key presses are ignored and not buffered
- after `kLockoutMs` (30000 ms) the counter is reset, the buffer cleared and the state returns to `Locked` with red LED
- the comparison is `NowMs() - lockoutStartMs_ >= kLockoutMs` in `uint32_t` arithmetic, which stays correct across the 32-bit overflow (after about 49.7 days)
- a second lockout restarts the timer

To change the lockout duration, change `kLockoutMs` in `AccessController.hpp`.

### SysTickClock

- `SysTickClock::Init()` sets `SYST_RVR` to `kCoreClockHz / 1000 - 1`, clears `SYST_CVR` and enables SysTick with interrupt and processor clock (`SYST_CSR = 0x7`).
- `SysTick_Handler` increments a `volatile uint32_t` millisecond counter. It overrides the weak alias in the vector table.
- `NowMs()` returns the counter. 32-bit aligned reads are atomic on Cortex-M4.
- `kCoreClockHz` is 16000000 (HSI). It must be changed together with `swoInit()` and `scripts/swo.gdb` if the clock is ever changed (see Hardware Configuration).

---

## Implemented Features

The following functionality is complete:

- custom startup code
- vector table with weak aliases for unused handlers
- reset handler
- custom linker script
- register-level GPIO driver
- 4x4 matrix keypad input, interrupt-driven (EXTI + NVIC, `wfi` in main loop)
- PIN authentication with unified input validation
- PIN change while unlocked
- failed attempt counter
- lockout handling with 30 s timeout (SysTick-based `IClock`)
- LED feedback
- HardFault handler with SWO crash dump
- host-based unit and driver tests (GoogleTest)
- ST-LINK/OpenOCD/GDB debugging
- hardware validation on STM32F407G-DISC1
- SWO runtime debug output (register-level ITM/TPIU configuration, no UART)
- CI (GitHub Actions): firmware build and host unit tests

---

## Hardware Configuration

Board:

STM32F407G-DISC1

MCU:

- STM32F407VG
- ARM Cortex-M4

Clock:

- Runs on HSI (16 MHz), no PLL configured. Any future clock change must also update:
  - `swoInit()`'s core-clock argument
  - the matching value in `scripts/swo.gdb`
  - `kCoreClockHz` in `App/Adapters/SysTickClock.cpp`

  Otherwise SWO output breaks and the lockout timeout no longer lasts 30 s.

LEDs:

- Green LED: PD12
- Red LED: PD14

Keypad:

4x4 matrix keypad.

Connections:

Rows (outputs, idle LOW):

- R1: PD0
- R2: PD1
- R3: PD2
- R4: PD3

Columns (inputs, pull-up, EXTI8-11, falling edge):

- C1: PD8
- C2: PD9
- C3: PD10
- C4: PD11

Trace:

- SWO: PB3 (TRACESWO), routed through the onboard ST-LINK over the same USB connection used for flashing/debugging. No separate wiring, no UART.

---

## Testing

Host-based tests use GoogleTest (fetched via CMake `FetchContent`, v1.15.2) and run through `ctest`.

Two executables:

- `unit_tests` — application logic (AccessController, PinValidator, AttemptCounter, LockState) and `KeypadAdapterTest`, using `MockKeypad`, `MockLedOutput`, `MockClock` and `Tests/Stubs/keypad_stub.c`
- `driver_tests` — `keypad.c` compiled with hardware fakes, covering `Keypad_ReadEvent()`, both ISRs and `Keypad_IrqPending()`

Tests cover:

- correct PIN handling
- wrong PIN handling
- failed attempts
- lockout behaviour, including timeout (before, at and after `kLockoutMs`), counter reset after timeout, ignored input during lockout, restart of the timer on a second lockout and `uint32_t` wraparound of the clock
- invalid keys, too short and too long PINs in `Locked` and `Unlocked`; none of them counts as a failed attempt
- PIN change: valid change, old PIN rejected afterwards, invalid input keeps the system unlocked
- keypad event handling and interrupt flag logic

Using `MockClock`: set `clock.now` to the desired time in ms and call `controller.Update()` once without a key press (`keypad.nextKey = '\0'`) so that the `LockedOut` branch runs. The test file defines a small `TICK()` helper for this.

Run with `make tests`. `App/Interfaces` must be in the include paths of `Tests/CMakeLists.txt`. `MockClock.hpp` lives in `Tests/Mocks/`.

The purpose is to test application logic and driver logic without hardware dependency. The SWO driver, `SysTickClock` and the fault handler are not part of this host-based suite.

Manual hardware checklist (after flashing):

- start: red LED
- `1234#`: green LED
- wrong PIN: red LED stays, no unlock
- three wrong PINs: red LED blinks
- `1234#` during lockout: no reaction
- after 30 s: red LED, then `1234#` unlocks
- unlocked, `5678#`: back to red; `5678#` unlocks, `1234#` does not
- `12*34#` and `12345#`: input aborted, no unlock
- SWO: every key press arrives exactly once (debounce, no double trigger)

Hardware tests on the board (Unity with SWO output, Renode, GDB scripts with `EXTI->SWIER`, optionally hardware-in-the-loop) are tracked in a separate issue.

---

## Continuous Integration

Workflow: `.github/workflows/` (STM32 CI), two jobs:

- `firmware-build`: installs the ARM toolchain and builds with `cmake/arm-gcc-toolchain.cmake`
- `unit-tests`: configures with `-DBUILD_TESTS=ON`, builds and runs `ctest`

The firmware job needs these packages, installed with plain `apt-get`:

```yaml
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
  gcc-arm-none-eabi \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib
```

`libstdc++-arm-none-eabi-newlib` provides the C++ headers (`<cstdint>`), `libnewlib-arm-none-eabi` the C headers (`stdlib.h`). The `cache-apt-pkgs-action` did not restore these headers correctly (errors like `stdlib.h: No such file or directory`), which is why it was replaced. If it is reintroduced, bump its `version` input whenever the package list changes.

---

## Static Analysis

- `make tidy` runs clang-tidy on `App` and `Core`.

Known warnings worth keeping clean: `-Wreturn-type` (a missing `return` in a non-void function compiles to an illegal instruction on GCC and crashed the host tests), `-Wmisleading-indentation` in `Drivers/keypad.c`, `-Wunused-parameter` in `cpp_runtime.cpp` (use unnamed parameters).

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
- `make tests` — build and run the host tests
- `make tidy` — run clang-tidy
- `make flash` — program and reset via OpenOCD
- `make openocd` — start the OpenOCD GDB server (keep running in its own terminal)
- `make gdb` / `make debug` / `make reset` / `make reboot` — various GDB sessions
- `make swo` — runs `gdb-multiarch -batch -x scripts/swo.gdb $(ELF)` to flash and start the firmware with SWO configured; read output separately with `tail -f swo.log`

### SWO debug workflow

Three terminals:

```
make openocd    # terminal 1, stays running
make swo        # terminal 2
tail -f swo.log # terminal 3
```

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

If SWO output ever looks like garbage again, check in this order: (1) is the firmware actually running past `swoInit()`/`swoPuts()`, not stuck in a fault (the HardFault dump goes through the same SWO path); (2) do the core clock values match on both sides; (3) try lowering the SWO baud rate further; (4) check the `ITM->TPR` value (see below).

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
- Update `architecture.md` and the README for `IClock`/`SysTickClock`, the lockout timeout, the input validation rules and the HardFault handler.

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

The vector table contains the exception and IRQ entries. Handlers that are not implemented are weak aliases of a default handler; the real definitions override them at link time. Currently overridden: `EXTI9_5_IRQHandler`, `EXTI15_10_IRQHandler` (`keypad.c`), `SysTick_Handler` (`SysTickClock.cpp`), `HardFault_Handler` (`fault.cpp`). The names must match exactly and C++ definitions need `extern "C"`.

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
- SYSCFG / EXTI / NVIC interrupt configuration
- SysTick timer (`SYST_CSR`, `SYST_RVR`, `SYST_CVR`)
- SCB fault status registers
- ITM/TPIU/DEMCR/DBGMCU trace and debug registers

Do not replace register access with HAL.

Peripheral/register structs are exposed as `reinterpret_cast`-based pointers at fixed addresses (see `Platform/debug_regs.h` for the SWO-related example). This is a deliberate, explicit cast, not a C-style cast, so that raw address-to-pointer reinterpretation stays grep-able and visually distinct from other casts in the codebase.

---

## Build System

- `CMakeLists.txt` drives both the firmware build and the host test build (`BUILD_TESTS` option), kept in one file rather than split per target.
- Firmware sources, include directories (`Core`, `Platform`, `Drivers`, `App`, `App/Interfaces`, `App/Adapters`), compiler flags (`-mcpu=cortex-m4 -mthumb`, no exceptions/RTTI/unwind tables for C++), and linker flags (custom linker script, `-nostartfiles -nostdlib`) are defined there.
- New driver/platform/adapter source files must be added explicitly to the `add_executable()` file list in `CMakeLists.txt` (e.g. `Drivers/swo.cpp`, `Drivers/fault.cpp`, `App/Adapters/SysTickClock.cpp`, `Platform/stm32f407xx_exti.c`); CMake does not glob sources here by design, for explicitness. `SysTickClock.cpp` and `fault.cpp` are firmware-only and must not be added to the test targets.
- Consider `-Werror=return-type` for both builds.
- Host tests are defined in `Tests/CMakeLists.txt` (`unit_tests`, `driver_tests`, GoogleTest via `FetchContent`).
- The top-level `Makefile` wraps CMake configure/build steps and all OpenOCD/GDB workflows (flash, debug, reset, reboot, swo, inspection targets like `size`/`disasm`/`symbols`/`sections`/`elfinfo`), plus `tests` and `tidy`. Prefer adding new debug/inspection workflows as Makefile targets rather than ad hoc shell commands, to keep the workflow self-documenting via `make help`.

---

## Future Work

Only add improvements if they preserve the original purpose of the project.

Planned future extensions:

- improve test coverage
- `platform_tests` for the EXTI/NVIC helpers
- hardware tests on the board (Unity via SWO, Renode, GDB with `EXTI->SWIER`, optionally HIL), separate issue
- introduce `ILogger` / `SwoLoggerAdapter` / `MockLogger` if application-layer logging over SWO is needed, keeping the application layer free of direct driver calls
- move SysTick register access from `SysTickClock` into `Drivers/systick.*`
- persist the changed PIN (flash/backup SRAM) instead of keeping it in RAM only
- `Docs/swo.md` write-up

Do not change the architecture to hide hardware details.