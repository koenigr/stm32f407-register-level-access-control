# HardFault Handling

`HardFault_Handler` (in `main.cpp`) overrides the weak handler of the vector table. It is split in two parts:

- `HardFault_Handler` is a naked trampoline. It tests bit 2 of LR (EXC_RETURN) to select MSP or PSP and passes the pointer to the stacked exception frame in r0.
- `hardfault_c(uint32_t* frame)` reads the stacked PC (`frame[6]`) and LR (`frame[5]`) and the registers CFSR and HFSR into local variables, then executes `bkpt #0` and loops forever.

Nothing is printed. The values are read in the debugger (locals) at the breakpoint. The handler uses no keypad, interrupts or application layer, since the system state is unknown after a fault.

## Analysis

When the handler is entered, the debugger stops at `bkpt #0` in `hardfault_c`.

1. Read the values: `pc`, `lr`, `cfsr`, `hfsr` in the locals, or dump the whole stacked frame in GDB (R0-R3, R12, LR, PC, xPSR):

   ```
   (gdb) x/8wx frame
   ```

2. Resolve the faulting instruction (PC faulted, LR points into the caller):

   ```
   arm-none-eabi-addr2line -f -C -e build-firmware/stm32f407_access_control.elf <PC>
   ```

   Or in GDB: `info line *<PC>` / `list *<PC>`.

   For INVSTATE, PC is the invalid jump target (e.g. `0x20000000`) and cannot be resolved. Resolve LR instead, it points to the code that made the jump. For imprecise bus errors (IMPRECISERR) PC may be a few instructions after the faulting access.

3. Decode CFSR:

   | Bits  | Group      | Examples                                     |
   |-------|------------|----------------------------------------------|
   | 0-7   | MemManage  | IACCVIOL, DACCVIOL, MMARVALID (bit 7)        |
   | 8-15  | BusFault   | IBUSERR, PRECISERR (bit 9), BFARVALID (bit 15) |
   | 16-31 | UsageFault | UNDEFINSTR, INVSTATE (bit 17), UNALIGNED, DIVBYZERO |

4. HFSR bit 30 (FORCED): a configurable fault escalated to a HardFault. The cause is in CFSR.

5. If CFSR bit 15 (BFARVALID) is set, read the faulting address from BFAR, if bit 7 (MMARVALID) is set from MMFAR. Both are only valid with their bit set:

   ```
   (gdb) x/wx 0xE000ED38    # BFAR
   (gdb) x/wx 0xE000ED34    # MMFAR
   ```

6. Get the call stack of the faulting code by restoring the stacked registers in GDB:

   ```
   (gdb) set $pc = frame[6]
   (gdb) set $lr = frame[5]
   (gdb) set $sp = (uint32_t)frame + 32
   (gdb) bt
   ```

## Deliberate test

Enable `#define TEST_HARDFAULT` in `main.cpp`. `test_invalid_fnptr()` calls a function pointer without the Thumb bit (`0x20000000`). Expected in the debugger: CFSR = `0x00020000` (INVSTATE), HFSR = `0x40000000` (FORCED). Remove the define afterwards.