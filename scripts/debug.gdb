source scripts/reset.gdb
source scripts/inspect.gdb

break main
break HardFault_Handler
commands
	crash_report
	continue
end

info breakpoints
continue