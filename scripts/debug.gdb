# Connect to OpenOCD
target remote localhost:3333

# Load firmware
load

# Reset MCU and halt immediately
monitor reset halt

# Set breakoints
break main
break HardFault_Handler

# list breakpoints
info breakpoints

# Start execution
continue

define crash_report
	echo \n=== CRASH DUMP ===\n
	info registers
	backtrace full
	echo \n==================\n
end