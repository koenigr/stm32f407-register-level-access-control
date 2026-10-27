# SWO Debugging

## Overview

SWO (Serial Wire Output) is a one-way trace channel of the Cortex-M4 CoreSight debug system. The firmware writes characters to an ITM stimulus port, the TPIU serializes them onto the `TRACESWO` pin (PB3), and the on-board ST-LINK forwards them to the host over the same USB connection that is used for flashing and debugging.

No UART and no extra wiring are needed. The firmware output appears in `swo.log` on the host.

```text
swoPuts() / ITM_SendChar()
→ ITM stimulus port 0
→ TPIU (async NRZ)
→ PB3 / TRACESWO
→ ST-LINK
→ OpenOCD
→ swo.log
```

SWO is used for startup messages in `main()` and for the HardFault crash dump. The SWO driver (`Drivers/swo.h`, `Drivers/swo.cpp`) is not part of the host-based unit tests, since it writes to addresses that do not exist on the host.

---

## Registers

ITM, TPIU and DEMCR belong to the Cortex-M4 core (ARMv7-M, CoreSight), `DBGMCU_CR` to the STM32 (RM0090). All definitions are explicit in `Platform/debug_regs.h`; no CMSIS core header is used.

| Register | Address | Purpose |
|---|---|---|
| DEMCR | 0xE000EDFC | Bit 24 `TRCENA` enables the trace blocks |
| DBGMCU_CR | 0xE0042004 | `TRACE_IOEN` (bit 5) and `TRACE_MODE` (bits 7:6) route the trace pin |
| TPIU_ACPR | 0xE0040010 | Prescaler for the SWO baud rate |
| TPIU_SPPR | 0xE00400F0 | Pin protocol (asynchronous NRZ) |
| TPIU_FFCR | 0xE0040304 | Formatter control |
| ITM_STIM0 | 0xE0000000 | Stimulus port 0, write a character to send it |
| ITM_TER | 0xE0000E00 | Enable stimulus ports |
| ITM_TPR | 0xE0000E40 | Unmask ports for unprivileged code |
| ITM_TCR | 0xE0000E80 | ITM control |
| ITM_LAR | 0xE0000FB0 | Lock access register, unlocked with `0xC5ACCE55` |

The SWO pin PB3 uses alternate function 0.

---

## Initialization Sequence

`swoInit(coreClockHz, baudRate)` follows the example configuration from the STM32 reference manual (DBG chapter) and the Cortex-M4 TRM, in this order:

1. enable trace in `DEMCR.TRCENA`
2. enable the trace pin in `DBGMCU_CR` (asynchronous mode)
3. configure the TPIU: pin protocol and baud prescaler
4. unlock the ITM via `LAR`
5. configure `TCR`
6. enable stimulus port 0 in `TER`
7. unmask the privilege in `TPR`

The TPIU prescaler is derived from the clock and the baud rate:

```text
ACPR = coreClockHz / baudRate - 1
     = 16000000 / 125000 - 1 = 127
```

`ITM->TPR` should be set to `1` (unmask ports 0-7 for unprivileged code), matching the reference manual's example, not `0`. With `0`, SWO still works today only because `main()` runs in privileged thread mode (no RTOS). Check this if an RTOS or unprivileged code is ever introduced.

---

## Usage

Three terminals:

```bash
make openocd      # terminal 1, stays running
make swo          # terminal 2, flashes and starts the firmware with SWO configured
tail -f swo.log   # terminal 3, shows the output
```

`make swo` runs `gdb-multiarch -batch -x scripts/swo.gdb $(ELF)`.

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

The `TPIU config` line makes OpenOCD decode the SWO stream on the host side. Core clock and baud rate must match the values given to `swoInit()`.

### Why `monitor resume`

In `-batch` mode GDB detaches as soon as the script finishes. Detaching after a plain `continue` tends to leave the target halted again (`gdb_detach halt` behaviour in OpenOCD). `monitor resume` resumes the CPU through OpenOCD itself, so it keeps running after GDB disconnects.

### Why 125000 baud

The initially planned 2000000 baud was not decoded reliably by the on-board ST-LINK: the output in `swo.log` was garbled and of inconsistent length, and no `0x05` ITM header bytes were visible in `xxd`. 125 kHz is a conservative rate that works reliably on this hardware.

The value exists in two places that must stay in sync:

- `swoInit(16000000, 125000)` in `main()`
- `monitor tpiu config internal swo.log uart off 16000000 125000` in `scripts/swo.gdb`

---

## Clock Dependency

The system runs on HSI (16 MHz), no PLL. If the clock is changed, three places must be updated together:

- the core-clock argument of `swoInit()`
- the matching value in `scripts/swo.gdb`
- `kCoreClockHz` in `App/Adapters/SysTickClock.cpp` (lockout time base)

---

## HardFault Dump

`Drivers/fault.cpp` overrides the weak `HardFault_Handler` of the vector table and prints a crash dump over SWO.

The handler is split into two parts:

- `HardFault_Handler` is a naked trampoline. It tests bit 2 of `LR` (EXC_RETURN) to find out whether the faulting code used MSP or PSP and passes the pointer to the stacked exception frame in `r0`.
- `HardFault_Handler_C(uint32_t* frame)` prints the stacked `R0-R3`, `R12`, `LR`, `PC`, `xPSR` and the registers `CFSR`, `HFSR`, `MMFAR`, `BFAR`, then loops forever.

The handler only uses the polling SWO driver. It does not use the keypad, the interrupt system or the application layer, since the system state is unknown after a fault.

Example output (values will differ):

```text
*** HardFault ***
R0  =0x00000000
R1  =0x...
LR  =0x080003A1
PC  =0x08000412
xPSR=0x61000000
CFSR=0x00008200
HFSR=0x40000000
BFAR=0xFFFFFFF0
```

### Analysis

1. Resolve the faulting instruction. `PC` is the instruction that faulted, `LR` points into the caller:

   ```bash
   arm-none-eabi-addr2line -e build-firmware/stm32f407_access_control.elf <PC>
   ```

   Use `-f -C` to also print the function name.

2. Decode `CFSR`:

   | Bits | Group | Examples |
   |---|---|---|
   | 0-7 | MemManage | `IACCVIOL`, `DACCVIOL`, `MMARVALID` (bit 7) |
   | 8-15 | BusFault | `IBUSERR`, `PRECISERR` (bit 9), `BFARVALID` (bit 15) |
   | 16-31 | UsageFault | `UNDEFINSTR`, `INVSTATE`, `UNALIGNED`, `DIVBYZERO` |

   In the example, `0x8200` means a precise bus error (`PRECISERR`) with a valid `BFAR`, here the access to address `0xFFFFFFF0`.

3. `HFSR` bit 30 (`FORCED`) means that a configurable fault (MemManage, BusFault, UsageFault) escalated to a HardFault. The cause is then found in `CFSR`.

4. `BFAR` and `MMFAR` are only valid if the matching valid bit in `CFSR` is set.

### Deliberate test

To check the handler, temporarily add a write to an invalid address and remove it afterwards:

```cpp
*reinterpret_cast<volatile uint32_t*>(0xFFFFFFF0U) = 0U;
```

Since BusFault is not enabled separately, it escalates to a HardFault. The dump appears in `swo.log`, with `PC` pointing at the store instruction.

---

## Troubleshooting

If `swo.log` stays empty or looks like garbage, check in this order:

1. Is the firmware running past `swoInit()` / `swoPuts()`, or is it stuck in a fault or in `Default_Handler`?
2. Do the core-clock values in `swoInit()` and `scripts/swo.gdb` match the real clock?
3. Lower the SWO baud rate further.
4. Check the `ITM->TPR` value (see Initialization Sequence).
5. `make swo` must be started after `make openocd` is running; both use the same ST-LINK.

`make flash` alone does not configure SWO on the host side. It programs and resets the target, but `swo.log` only receives data with `make swo`.
