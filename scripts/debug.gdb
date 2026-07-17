# Connect to OpenOCD
target remote localhost:3333

# Load firmware
load

# Reset MCU and halt immediately
monitor reset halt

# Break at application entry
break main

# Start execution
continue