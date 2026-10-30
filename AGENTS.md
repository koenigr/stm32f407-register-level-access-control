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
- Key presses are