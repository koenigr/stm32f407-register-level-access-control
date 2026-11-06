# HardFault Handling

`HardFault_Handler` (in `main.cpp`) overrides the weak handler of the vector table. It is split in two parts:

- `HardFault_Handler` is a naked trampoline. It tests bit 2 of LR (EXC_RETURN) to select MSP or PSP and passes the pointer to the stacked exception frame in r0.
- `hardfault_c(uint32_t* frame)` reads the stacked PC (`frame[6]`) and LR (`frame[5]`) and the registers CFSR and HFSR into local variables, then executes `bkpt #0` and loops forever.

Nothing is printed. The values are read in the debugger (locals) at the breakpoint. The handler uses no keypad, interrupts or application layer, since the system state is unknown after a fault.

## Analysis

1. Resolve the faulting instruction (PC faulted, LR points into the caller):

   ```
   arm-none-eabi-addr2line -f -C -e build-firmware/stm32f407_access_control.elf <PC>
   ```

2. Decode CFSR:

   | Bits  | Group      | Examples                                     |
   |-------|------------|----------------------------------------------|
   | 0-7   | MemManage  | IACCVIOL, DACCVIOL, MMARVALID (bit 7)        |
   | 8-15  | BusFault   | IBUSERR, PRECISERR (bit 9), BFARVALID (bit 15) |
   | 16-31 | UsageFault | UNDEFINSTR, INVSTATE (bit 17), UNALIGNED, DIVBYZERO |

3. HFSR bit 30 (FORCED): a configurable fault escalated to a HardFault. The cause is in CFSR.

## Deliberate test

Enable `#define TEST_HARDFAULT` in `main.cpp`. `test_invalid_fnptr()` calls a function pointer without the Thumb bit (`0x20000000`). Expected in the debugger: CFSR = `0x00020000` (INVSTATE), HFSR = `0x40000000` (FORCED). Remove the define afterwards.