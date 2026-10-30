# STM32F407 Register-Level Access Control System - Agent Notes

## Project Status

This project is a demonstrable bare-metal embedded system for the STM32F407G-DISC1.

The firmware has been implemented, tested on real hardware and documented. The keypad input is interrupt-driven (EXTI), PIN input validation is unified, a time-based lockout (SysTick clock) is implemented, and a HardFault handler with SWO crash dump is available.

The project also contains host-based GoogleTest tests and a separate Unity-based board-test executable for testing the keypad/EXTI hardware chain.

The goal is to demonstrate understanding of embedded systems from MCU startup to application logic, including register-level programming, hardware debugging and testing.

---

## Important Constraints

Do not introduce:

- STM32 HAL
- STM32CubeMX-generated code
- Unnecessary embedded frameworks

Maintain:

- Register-level hardware access
- Clear separation between hardware and application logic
- Simple, explicit embedded C/C++ code
- Testable application architecture
- Explicit source-file lists in CMake

Do not replace the custom startup code, register definitions or peripheral access with generated code or a framework.

---

## Current Architecture

The project uses a layered architecture.

### Platform Layer

Responsible for direct hardware representation.

Contains:

- STM32 register definitions (`Platform/stm32f407xx.h`)
- Peripheral addresses
- RCC definitions, including `APB2ENR` for SYSCFG
- GPIO register structures
- SYSCFG (`EXTICR1-4`), EXTI (`IMR`, `RTSR`, `FTSR`, `SWIER`, `PR`) and NVIC (`ISER`) register definitions
- EXTI helper functions (`Platform/stm32f407xx_exti.c` / `.h`)
- Debug/trace register definitions (ITM, TPIU, DEMCR, DBGMCU) in `Platform/debug_regs.h`
- System Control Block fault status registers (`CFSR`, `HFSR`, `MMFAR`, `BFAR`), used by the HardFault handler

The platform layer represents the actual MCU hardware.

**Register provenance:** ITM and TPIU register layouts are not STM32-specific; they belong to the ARM Cortex-M4 core (CoreSight). `DBGMCU_CR` and the DEMCR/DHCSR core-debug registers are documented separately (RM0090 for DBGMCU, the ARMv7-M Architecture Reference Manual for DEMCR/DHCSR, and the Cortex-M4 Technical Reference Manual for ITM/TPIU). The same applies to SysTick and the SCB fault registers (ARMv7-M ARM).

This separation is intentional. Do not simplify it by pulling in a CMSIS core header. The register definitions remain explicit in the project.

### Driver Layer

Contains hardware drivers.

Current drivers:

- GPIO driver
- Keypad driver (interrupt-driven)
- SWO driver (`Drivers/swo.h` / `Drivers/swo.cpp`)
- Fault handler (`Drivers/fault.cpp`, HardFault crash dump over SWO)

Responsibilities:

- GPIO configuration
- Input reading
- Output control
- Keypad matrix scanning, triggered by column interrupts (EXTI8-11)
- SWO trace initialization and character output (`swoInit()`, `ITM_SendChar()`, `swoPuts()`)
- Fault diagnostics (stacked register frame and fault status registers)

Drivers use the platform layer but do not contain application logic.

#### Keypad Driver (Interrupt-Driven)

- Rows PD0-PD3 are outputs.
- Columns PD8-PD11 are inputs with pull-up.
- In idle, all rows are driven LOW (`Keypad_SetIdle()`, called at the end of `Keypad_Init()`). A key press therefore pulls a column LOW and triggers a falling-edge interrupt.
- Columns are routed to EXTI8-11 via `SYSCFG->EXTICR3` (port D).
- Falling-edge detection is configured via `FTSR`; interrupts are unmasked via `IMR`.
- NVIC configuration:
  - `EXTI9_5_IRQHandler`: IRQ 23, `ISER0` bit 23.
  - `EXTI15_10_IRQHandler`: IRQ 40, `ISER1` bit 8.
- The ISRs clear `EXTI->PR` and set a flag.
- Debouncing and matrix scanning run in the main loop, with `IMR` masked during the scan.
- `main()` sleeps with `wfi` and processes key events via `Keypad_ReadEvent()`.
- `Keypad_IrqPending()` exposes the interrupt flag.
- The application layer reaches the driver only through `KeypadAdapter::Scan`.

With SysTick running at 1 kHz, `wfi` returns every millisecond. The main loop therefore runs `AccessController::Update()` at least once per millisecond, which the lockout timeout relies on. `Update()` with no key pressed must remain inexpensive.

The SWO driver is excluded from host-based unit tests because it writes directly to hardware addresses that do not exist on the host.

If `ILogger`, `SwoLoggerAdapter` or `MockLogger` are introduced later so the application layer can log through an interface, the adapter belongs in `App/Adapters`, not in the driver layer.

#### Fault Handling (HardFault)

`Drivers/fault.cpp` overrides the weak `HardFault_Handler` alias from the vector table.

It consists of two parts:

- `HardFault_Handler` is a naked trampoline. It checks bit 2 of `LR` (EXC_RETURN) to determine whether the faulting code used MSP or PSP, then passes the pointer to the stacked exception frame in `r0` to the C function.
- `HardFault_Handler_C(uint32_t* frame)` prints the stacked `R0-R3`, `R12`, `LR`, `PC`, `xPSR` and the fault status registers `CFSR`, `HFSR`, `MMFAR`, `BFAR` over SWO, then loops forever.

The handler must not use stack-heavy or interrupt-dependent application components. Do not use the keypad or `AccessController` from the fault handler. It only needs the polling-based SWO driver.

How to use it:

1. Start the SWO workflow (`make openocd`, `make swo`, `tail -f swo.log`).
2. When a crash occurs, the dump appears in `swo.log`.
3. Resolve the faulting address with:

   ```bash
   arm-none-eabi-addr2line -e build-firmware/stm32f407_access_control.elf <PC>
   ```

   This is also useful for `LR`.

4. Interpret `CFSR` using the ARMv7-M ARM:
   - Bits 0-7: MemManage
   - Bits 8-15: BusFault
   - Bits 16-31: UsageFault

   Examples include `DIVBYZERO`, `UNALIGNED`, `INVSTATE` and `PRECISERR`.

`BFAR` and `MMFAR` are only valid if the corresponding valid bit in `CFSR` is set.

Deliberate fault test (remove afterwards):

```cpp
*reinterpret_cast<volatile uint32_t*>(0xFFFFFFF0U) = 0U;
```

With BusFault not separately enabled, this escalates to HardFault. The expected dump appears in `swo.log`, with `PC` pointing to the store instruction.

The SWO baud-rate and core-clock rules described below also apply to the HardFault dump.

---

## Application Layer

The application layer contains hardware-independent application logic:

- `AccessController`
- `PinValidator`
- `AttemptCounter`
- `LockState`

Interfaces:

- `IKeypad`
- `ILedOutput`
- `IClock` (`uint32_t NowMs() const`, monotonic millisecond counter)

Adapters:

- `KeypadAdapter`
- `LedOutputAdapter`
- `SysTickClock` (firmware only, not part of the host tests)

The application layer must never directly access STM32 registers.

`SysTickClock` lives in `App/Adapters` but accesses SysTick registers directly. This is the hardware boundary for the clock adapter and is acceptable for now. If more hardware time functions are needed, move register access into `Drivers/systick.*` and let the adapter call that driver.

`main()` currently calls `swoInit()` and `swoPuts()` directly. This is acceptable for one-off startup diagnostic output. If SWO output is needed from inside the application layer, it must go through an `ILogger` interface and adapter, not through a direct driver call.

### AccessController Behaviour

States:

- `Locked`
- `Unlocked`
- `LockedOut`

Construction:

```cpp
AccessController(IKeypad&, ILedOutput&, IClock&)
```

In `main()`:

```cpp
SysTickClock::Init();          // before the main loop
SysTickClock clock;
AccessController controller{keypadAdapter, ledAdapter, clock};
```

#### Input Handling

Input handling is identical in `Locked` and `Unlocked`, implemented in `ProcessKey()`:

- Digits `0-9` are appended to the PIN buffer.
- `#` submits the input.
- Any other key (`A-D`, `*`, ...) aborts the current input and clears the buffer.
- A fifth digit also aborts the input. There is no silent truncation; `12345#` is invalid.
- A PIN with fewer than four digits is rejected on `#`.
- Aborted, too-short and too-long inputs do not count as failed attempts.
- Only a complete four-digit wrong PIN counts as a failed attempt.

#### Locked

- Correct PIN: green LED, attempt counter reset, state changes to `Unlocked`.
- Wrong PIN: red LED, attempt counter incremented.
- Third wrong PIN: state changes to `LockedOut`, red LED blinks and `lockoutStartMs_` is set.

#### Unlocked

- A valid four-digit input followed by `#` replaces the PIN (RAM only; lost on reset), changes the state to `Locked` and turns on the red LED.
- Invalid or incomplete input keeps the system `Unlocked`.
- The old PIN stays valid until a valid new PIN is submitted.

#### LockedOut

- The keypad is not scanned.
- Key presses are ignored and are not buffered.
- After `kLockoutMs` (30000 ms), the counter is reset, the buffer is cleared and the state returns to `Locked` with the red LED.
- The timeout comparison is:

  ```cpp
  NowMs() - lockoutStartMs_ >= kLockoutMs
  ```

  This uses `uint32_t` arithmetic and remains correct across the 32-bit counter overflow (approximately 49.7 days).

- A second lockout restarts the timer.

To change the lockout duration, change `kLockoutMs` in `AccessController.hpp`.

### SysTickClock

- `SysTickClock::Init()` sets `SYST_RVR` to `kCoreClockHz / 1000 - 1`, clears `SYST_CVR` and enables SysTick with the interrupt and processor clock (`SYST_CSR = 0x7`).
- `SysTick_Handler` increments a `volatile uint32_t` millisecond counter.
- It overrides the weak alias in the vector table.
- `NowMs()` returns the counter. Aligned 32-bit reads are atomic on Cortex-M4.
- `kCoreClockHz` is 16000000 (HSI).

If the clock configuration changes, update `kCoreClockHz`, the `swoInit()` core-clock argument and the matching value in `scripts/swo.gdb`.

---

## Implemented Features

The following functionality is implemented:

- Custom startup code
- Vector table with weak aliases for unused handlers
- Reset handler
- Custom linker script
- Register-level GPIO driver
- 4x4 matrix keypad input, interrupt-driven (EXTI + NVIC, `wfi` in main loop)
- PIN authentication with unified input validation
- PIN change while unlocked
- Failed-attempt counter
- Lockout handling with 30-second SysTick-based timeout through `IClock`
- LED feedback
- HardFault handler with SWO crash dump
- Host-based unit and driver tests using GoogleTest
- Separate Unity-based board-test executable
- Register configuration tests for keypad GPIO, EXTI and NVIC
- Software-triggered EXTI interrupt tests using `EXTI->SWIER`
- SWO output configuration for Unity test results
- ST-LINK/OpenOCD/GDB debugging
- SWO runtime debug output (register-level ITM/TPIU configuration, no UART)
- CI (GitHub Actions) for firmware build and host-based tests

Renode-based keypad simulation and its CI integration are not yet implemented.

---

## Hardware Configuration

Board:

- STM32F407G-DISC1

MCU:

- STM32F407VG
- ARM Cortex-M4

### Clock

The firmware runs on HSI (16 MHz), with no PLL configured.

Any future clock change must also update:

- `swoInit()`'s core-clock argument
- The matching value in `scripts/swo.gdb`
- `kCoreClockHz` in `App/Adapters/SysTickClock.cpp`

Otherwise SWO output may break and the lockout timeout will no longer last 30 seconds.

### LEDs

- Green LED: PD12
- Red LED: PD14

### Keypad

4x4 matrix keypad.

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

### Trace

- SWO: PB3 (TRACESWO)
- Routed through the onboard ST-LINK over the same USB connection used for flashing/debugging.
- No separate wiring and no UART are required.

---

## Testing

The project has three testing levels: host-based tests, on-target Unity tests and manual hardware validation.

### Host-Based Tests (GoogleTest)

Host-based tests use GoogleTest, fetched through CMake `FetchContent` (v1.15.2), and run through `ctest`.

The test executables are:

- `unit_tests` — application logic (`AccessController`, `PinValidator`, `AttemptCounter`, `LockState`) and `KeypadAdapterTest`, using `MockKeypad`, `MockLedOutput`, `MockClock` and `Tests/Stubs/keypad_stub.c`.
- `driver_tests` — `keypad.c` compiled with hardware fakes, covering `Keypad_ReadEvent()`, both ISRs and `Keypad_IrqPending()`.

Run the host tests with:

```bash
make tests
```

Tests cover:

- Correct PIN handling
- Wrong PIN handling
- Failed attempts
- Lockout behaviour, including timeout before, at and after `kLockoutMs`
- Counter reset after timeout
- Ignored input during lockout
- Restart of the timer on a second lockout
- `uint32_t` clock wraparound
- Invalid keys, too-short and too-long PINs in `Locked` and `Unlocked`
- PIN change, rejection of the old PIN afterwards and invalid input while unlocked
- Keypad event handling and interrupt flag logic

Using `MockClock`, set `clock.now` to the desired time in milliseconds and call `controller.Update()` once without a key press (`keypad.nextKey = '\0'`) so that the `LockedOut` branch runs. The test file defines a small `TICK()` helper for this.

`App/Interfaces` must be in the include paths of `Tests/CMakeLists.txt`. `MockClock.hpp` lives in `Tests/Mocks/`.

The purpose of these tests is to verify application and driver logic without hardware dependency.

The following are not part of the host-based suite because they access hardware registers directly:

- SWO driver
- `SysTickClock`
- HardFault handler

### On-Target Unity Tests

The `BoardTests/` directory contains a separate Unity-based executable named `board_tests`.

It has its own `main()` and is built independently from the normal firmware and host tests.

Unity is fetched through CMake `FetchContent`. Its output is redirected to SWO through `UNITY_OUTPUT_CHAR` in `BoardTests/unity_config.h`.

The board tests cover the hardware-facing keypad/EXTI initialization and interrupt chain:

- GPIO register configuration after `Keypad_Init()`
- EXTI configuration after interrupt initialization
- NVIC enable bits in `NVIC->ISER`
- Software-triggered interrupt requests using `EXTI->SWIER`
- Interrupt handling and pending-flag behaviour through `Keypad_IrqPending()`

Build the board-test executable with:

```bash
make board-tests
```

The Makefile also provides:

```bash
make flash-board-tests
make board-tests-run
```

`flash-board-tests` flashes the board-test executable using OpenOCD. `board-tests-run` invokes the board-test GDB script and captures the test output through SWO.

The Unity configuration is in:

- `BoardTests/CMakeLists.txt`
- `BoardTests/board_test_main.cpp`
- `BoardTests/unity_config.h`

The board-test GDB workflow is in:

- `scripts/board-tests-swo.gdb`

The board-test executable is separate from the production firmware. Its `main()` must not be linked into the normal firmware target.

Keep the board-test workflow independent from the normal SWO runtime workflow. In particular, do not change the normal `scripts/swo.gdb` behaviour when modifying the board-test GDB script.

The Unity board-test infrastructure is implemented. Do not describe a successful build as proof that the tests have been executed on physical hardware. Record build, flash, execution and SWO results separately when reporting test status.

### Renode Tests

Renode-based keypad simulation is future work.

Planned work:

- Configure a Renode platform for the STM32F407 target.
- Create Robot Framework tests that simulate keypad input.
- Verify the interrupt-driven keypad behaviour in simulation.
- Integrate the Renode tests into GitHub Actions.

Do not claim Renode tests or CI integration exist until the platform, Robot tests and workflow have actually been implemented and run.

### Manual Hardware Checklist

After flashing the normal firmware, verify:

- Startup: red LED
- `1234#`: green LED
- Wrong PIN: red LED stays on; no unlock
- Three wrong PINs: red LED blinks
- `1234#` during lockout: no reaction
- After 30 seconds: red LED; then `1234#` unlocks
- While unlocked, `5678#`: returns to red; `5678#` unlocks and `1234#` does not
- `12*34#` and `12345#`: input is aborted; no unlock
- SWO: every key press arrives exactly once (no debounce-related double trigger)

---

## Continuous Integration

Workflow files are located in:

```text
.github/workflows/
```

The existing STM32 CI covers:

- `firmware-build`: installs the ARM toolchain and builds the firmware with `cmake/arm-gcc-toolchain.cmake`.
- `unit-tests`: configures with `-DBUILD_TESTS=ON`, builds and runs `ctest`.

The firmware job needs these packages, installed with plain `apt-get`:

```yaml
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
  gcc-arm-none-eabi \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib
```

`libstdc++-arm-none-eabi-newlib` provides the C++ headers (such as `<cstdint>`), and `libnewlib-arm-none-eabi` provides the C headers (such as `stdlib.h`).

The `cache-apt-pkgs-action` did not restore these headers correctly, causing errors such as `stdlib.h: No such file or directory`. This is why it was replaced.

If the action is reintroduced, bump its `version` input whenever the package list changes.

The Unity board tests require the STM32 hardware and are not part of the existing host-based CI workflow.

Renode CI integration is future work.

---

## Static Analysis

- `make tidy` runs clang-tidy on `App` and `Core`.
- `make misra` runs the configured MISRA-related checks.

Known warnings worth keeping clean:

- `-Wreturn-type`: a missing return in a non-void function can compile to an illegal instruction on GCC and crash host tests.
- `-Wmisleading-indentation` in `Drivers/keypad.c`.
- `-Wunused-parameter` in `cpp_runtime.cpp`; use unnamed parameters where appropriate.

Do not suppress warnings globally to hide real issues.

---

## Debugging

### Debug Environment

- ST-LINK
- OpenOCD
- GDB (`gdb-multiarch` on this machine; `arm-none-eabi-gdb` is not available)

Available scripts include:

- `scripts/debug.gdb`
- `scripts/inspect.gdb`
- `scripts/reset.gdb`
- `scripts/swo.gdb`
- `scripts/board-tests-swo.gdb`

The top-level Makefile wraps the CMake build and OpenOCD/GDB workflows. Prefer adding new debug and inspection workflows as Makefile targets rather than relying on undocumented ad hoc shell commands.

Relevant Makefile targets include:

- `make configure-firmware` / `make firmware` — configure and build the STM32 target
- `make tests` — build and run host tests
- `make tidy` — run clang-tidy
- `make misra` — run configured MISRA-related checks
- `make flash` — program and reset via OpenOCD
- `make openocd` — start the OpenOCD GDB server; keep it running in its own terminal
- `make gdb` / `make debug` / `make reset` / `make reboot` — GDB and board-control workflows
- `make swo` — runs `gdb-multiarch -batch -x scripts/swo.gdb $(ELF)` to flash and start the firmware with SWO configured
- `make board-tests` — build the Unity board-test executable
- `make flash-board-tests` — flash the Unity board-test executable
- `make board-tests-run` — run the board-test GDB/SWO workflow
- Inspection targets such as `size`, `disasm`, `symbols`, `sections` and `elfinfo`

Check `make help` for the current target list.

### SWO Runtime Debug Workflow

The normal firmware SWO workflow uses three terminals:

```bash
make openocd    # terminal 1; remains running
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

Two important details:

1. `monitor resume` is intentional in the normal SWO script. In `-batch` mode, GDB detaches as soon as the script finishes. Detaching after `continue` can leave the target halted again because of OpenOCD's `gdb_detach halt` behaviour. `monitor resume` resumes the CPU through OpenOCD itself, allowing it to keep running after GDB disconnects.
2. The SWO baud rate is 125000, not the initially planned 2000000. The onboard ST-LINK could not reliably decode SWO at 2 MHz. The output was garbled and inconsistent, with no `0x05` ITM header bytes visible in `xxd`. 125 kHz is a conservative, reliable rate for this hardware.

The following values must remain synchronized:

- `swoInit(16000000, 125000)` in the firmware
- `monitor tpiu config internal swo.log uart off 16000000 125000` in `scripts/swo.gdb`

If SWO output looks incorrect, check:

1. Whether the firmware is actually running past `swoInit()` and `swoPuts()`, rather than being stuck in a fault.
2. Whether the core-clock values match on both sides.
3. Whether a lower SWO baud rate is needed.
4. The `ITM->TPR` value.

### Unity Board-Test Debug Workflow

The board-test GDB workflow is separate from the normal runtime SWO workflow.

The Unity test executable has its own `main()` and writes test output through `UNITY_OUTPUT_CHAR` to SWO.

Use the board-test Makefile targets:

```bash
make board-tests
make flash-board-tests
make board-tests-run
```

The OpenOCD server must be available when the GDB workflow runs.

Do not add `monitor resume` to a board-test script that is intended to wait for a test-completion breakpoint. The board-test script must allow GDB to inspect the test result before the debugging session exits.

Keep the normal `scripts/swo.gdb` workflow unchanged unless its own behaviour needs to change.

### Known SWO Register Configuration Detail

The TPIU/ITM initialization in `swo.cpp` follows the example configuration from the STM32 reference manual (DBG chapter) and the ARM Cortex-M4 TRM:

1. Enable trace (`DEMCR.TRCENA`).
2. Configure TPIU pin and baud rate.
3. Unlock ITM (`LAR`).
4. Configure `TCR`.
5. Enable stimulus port 0 (`TER`).
6. Unmask privilege (`TPR`).

`ITM->TPR` should be `1` to unmask ports 0-7 for unprivileged code, matching the reference manual's example. With `0`, SWO currently works because `main()` runs in privileged thread mode by default (no RTOS). Recheck this if an RTOS or unprivileged code is introduced.

---

## Documentation Status

Documentation is located in:

```text
Docs/
```

Current documents:

- `architecture.md`
- `hardware.md`
- `keypad_connection.md`
- `startup.md`
- `linker-script.md`
- `register_map.md`

The README links to these documents.

Documentation style:

- Write documentation as completed technical documentation.
- Avoid excessive bullet-only descriptions.
- Explain architecture and design decisions.
- Keep hardware details and implementation claims consistent with the source code.
- Distinguish implemented, tested and planned functionality.

Documentation that should be added or updated:

- `Docs/swo.md` — register configuration (ITM/TPIU/DEMCR/DBGMCU), host-side OpenOCD/GDB setup, and SWO baud-rate and `-batch` details.
- Update `architecture.md` and the README for `IClock`/`SysTickClock`, the lockout timeout, input validation rules and the HardFault handler.
- Document the Unity board-test workflow and its relationship to the keypad/EXTI driver.

The README should link to the relevant documentation.

---

## Startup Implementation

The project uses a custom startup implementation.

Important concepts:

- Vector table
- Reset handler
- `.data` initialization
- `.bss` initialization
- Linker script interaction

The vector table contains the exception and IRQ entries. Handlers that are not implemented are weak aliases of a default handler; real definitions override them at link time.

Currently overridden handlers:

- `EXTI9_5_IRQHandler`
- `EXTI15_10_IRQHandler` (both in `keypad.c`)
- `SysTick_Handler` (`SysTickClock.cpp`)
- `HardFault_Handler` (`fault.cpp`)

The names must match exactly. C++ handler definitions need `extern "C"`.

The startup code currently performs:

1. Load stack pointer.
2. Execute Reset Handler.
3. Copy initialized data from Flash to RAM.
4. Clear the BSS section.
5. Start `main()`.

No full C++ runtime is used.

`cpp_runtime.cpp` only provides the required embedded C++ symbols.

---

## Memory Layout

The linker script defines:

### Flash

- Start: `0x08000000`
- Size: 1024 KB

### RAM

- Start: `0x20000000`
- Size: 128 KB

Important linker symbols:

- `_estack`
- `_sidata`
- `_sdata`
- `_edata`
- `_sbss`
- `_ebss`

---

## Register-Level Implementation

The project uses memory-mapped peripheral access.

Important concepts:

- Peripheral base addresses
- Register structures
- `volatile` hardware access
- GPIO configuration registers
- RCC clock control
- SYSCFG / EXTI / NVIC interrupt configuration
- SysTick timer (`SYST_CSR`, `SYST_RVR`, `SYST_CVR`)
- SCB fault status registers
- ITM/TPIU/DEMCR/DBGMCU trace and debug registers

Do not replace register access with HAL.

Peripheral/register structs are exposed as `reinterpret_cast`-based pointers at fixed addresses (see `Platform/debug_regs.h` for the SWO-related example). This is deliberate. Do not replace these with C-style casts; keeping raw address-to-pointer conversions explicit makes them easier to identify in code reviews.

---

## Build System

- The top-level `CMakeLists.txt` drives the firmware build and the host test build through the `BUILD_TESTS` option.
- The Unity board-test executable is configured separately in `BoardTests/CMakeLists.txt`.
- The firmware uses the custom linker script and Cortex-M4 compiler/linker flags.
- C++ firmware builds disable exceptions, RTTI and unwind tables.
- New driver, platform and adapter source files must be added explicitly to the relevant `add_executable()` source list. CMake does not glob sources by design.
- Firmware-only files such as `App/Adapters/SysTickClock.cpp` and `Drivers/fault.cpp` must not be added to host test targets unless they are deliberately adapted for host use.
- Host tests are defined in `Tests/CMakeLists.txt` and use GoogleTest through `FetchContent`.
- Board tests are defined in `BoardTests/CMakeLists.txt` and use Unity through `FetchContent`.
- The top-level Makefile wraps configure/build steps and OpenOCD/GDB workflows. Prefer adding new workflows as Makefile targets so that `make help` remains the entry point for available commands.

The normal firmware and host test builds must remain independent from the Unity board-test executable.

---

## Future Work

Only add improvements if they preserve the original purpose of the project.

Planned extensions:

- Renode platform configuration for STM32F407.
- Robot Framework tests that simulate keypad presses and verify the interrupt-driven keypad behaviour.
- GitHub Actions integration for Renode tests.
- Optional hardware-in-the-loop test setup in which a second board drives the keypad lines.
- Introduce `ILogger` / `SwoLoggerAdapter` / `MockLogger` if application-layer logging over SWO is needed, keeping the application layer free of direct driver calls.
- Move SysTick register access from `SysTickClock` into `Drivers/systick.*`.
- Persist the changed PIN (Flash or backup SRAM) instead of keeping it in RAM only.
- Complete `Docs/swo.md`.

Do not add unnecessary tests or frameworks. Keep future work focused on demonstrable embedded engineering value.

Do not change the architecture merely to hide hardware details.