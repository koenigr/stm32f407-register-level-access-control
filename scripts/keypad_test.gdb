# Start: arm-none-eabi-gdb build/firmware.elf -x keypad_test.gdb
# OpenOCD läuft vorher (Port 3333)
target extended-remote :3333

monitor tpiu config internal swo.log uart off 16000000 125000
monitor itm port 0 on

monitor reset halt
load

set $n_9_5 = 0
set $n_15_10 = 0

# EXTI-Register (Base 0x40013C00): IMR +0x00, FTSR +0x0C, SWIER +0x10, PR +0x14
define pr
  printf "PR=0x%08x IMR=0x%08x\n", *(unsigned int*)0x40013C14, *(unsigned int*)0x40013C00
end

# Tastendruck simulieren: sw 8 .. sw 11 (setzt SWIER-Bit)
define sw
  set *(unsigned int*)0x40013C10 = (1u << $arg0)
end

define counts
  printf "EXTI9_5: %d, EXTI15_10: %d\n", $n_9_5, $n_15_10
end

break EXTI9_5_IRQHandler
commands
  silent
  set $n_9_5 = $n_9_5 + 1
  printf "EXTI9_5 hit, "
  pr
  continue
end

break EXTI15_10_IRQHandler
commands
  silent
  set $n_15_10 = $n_15_10 + 1
  printf "EXTI15_10 hit, "
  pr
  continue
end

set $events = 0

break keypad.c:136 if key != 0
commands
  silent
  set $events = $events + 1
  printf "EVENT #%d\n", $events
  continue
end

define reset_counts
  set $n_9_5 = 0
  set $n_15_10 = 0
  set $events = 0
end

define report
  printf "ISR: %d (9_5) + %d (15_10) = %d | Events: %d\n", $n_9_5, $n_15_10, $n_9_5 + $n_15_10, $events
end

continue