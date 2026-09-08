define crash_report
	echo \n=== CRASH DUMP ===\n
	info registers
	backtrace full
	echo \n==================\n
end

define gpiod_dump
	echo \n=== GPIOD ===\n
	printf "MODER: 0x%08x\n", *(unsigned int*)0x40020C00
	printf "ODR:   0x%08x\n", *(unsigned int*)0x40020C14
	printf "IDR:   0x%08x\n", *(unsigned int*)0x40020C10
	echo ==============\n
end

define rcc_dump
	printf "AHB1ENR: 0x%08x\n", *(unsigned int*)0x40023830
end