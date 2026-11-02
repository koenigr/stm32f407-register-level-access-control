source scripts/reset.gdb
source scripts/inspect.gdb

break main
commands
    silent
    printf "\nPASS: Reached main()\n"
    quit 0
end

break HardFault_Handler
commands
    silent
    printf "\nFAIL: HardFault_Handler reached\n"
    crash_report
    quit 1
end

continue

printf "\nFAIL: Target stopped unexpectedly\n"
quit 1
