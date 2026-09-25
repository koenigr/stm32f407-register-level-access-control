target extended-remote :3333
monitor reset halt
monitor tpiu config internal swo.log uart off 16000000 200000
monitor itm port 0 on

load monitor reset halt
continue