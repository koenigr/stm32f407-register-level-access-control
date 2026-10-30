.PHONY: help all firmware tests board-tests \
        flash flash-board-tests \
        debug clean clean-all \
        configure-firmware configure-tests configure-board-tests \
        rebuild-firmware rebuild-tests rebuild-board-tests \
        gdb openocd size disasm symbols sections elfinfo swo misra tidy \
        reset reboot keypad-test board-tests-run

# ------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------

FIRMWARE_BUILD = build-firmware
TEST_BUILD     = build-tests
BOARD_TEST_BUILD = build_board-tests
PROJECT        = stm32f407_access_control
ELF            = $(FIRMWARE_BUILD)/$(PROJECT)
BOARD_TEST_ELF = $(BOARD_TEST_BUILD)/board_tests
GCC_VER := $(shell arm-none-eabi-gcc -dumpfullversion)

OPENOCD        = openocd
INTERFACE_CFG  = /usr/share/openocd/scripts/interface/stlink.cfg
TARGET_CFG     = /usr/share/openocd/scripts/target/stm32f4x.cfg

# ------------------------------------------------------------------
# Default target
# ------------------------------------------------------------------

all: firmware

# ------------------------------------------------------------------
# Help
# ------------------------------------------------------------------

help:
	@echo "Available targets:"
	@echo ""
	@echo "Build:"
	@echo "  all                 Build firmware (default)"
	@echo "  firmware            Build STM32 firmware"
	@echo "  tests               Build and run unit tests"
	@echo ""
	@echo "Configuration:"
	@echo "  configure-firmware  Configure firmware build"
	@echo "  configure-tests     Configure host test build"
	@echo ""
	@echo "Rebuild:"
	@echo "  rebuild-firmware    Clean + configure + build firmware"
	@echo "  rebuild-tests       Clean + configure + build tests"
	@echo ""
	@echo "Debugging:"
	@echo "  flash               Flash firmware to STM32"
	@echo "  openocd             Start OpenOCD debug server"
	@echo "  gdb                 Start GDB and connect to OpenOCD"
	@echo ""
	@echo "Inspection:"
	@echo "  size                Show firmware size"
	@echo "  disasm              Generate disassembly"
	@echo "  symbols             Show ELF symbols"
	@echo "  sections            Show ELF sections"
	@echo "  elfinfo             Show ELF header information"
	@echo ""
	@echo "Cleanup:"
	@echo "  clean               Remove build artifacts"
	@echo "  clean-all           Remove all build directories"
	@echo ""
	@rcho "BoardTests:"
	@echo "  board-tests         Build Unity tests for STM32 hardware"
	@echo "  flash-board-tests   Flash Unity tests to STM32"
	@echo "  board-tests-run     Run board tests and capture SWO output"

# ------------------------------------------------------------------
# Configure
# ------------------------------------------------------------------

configure-firmware:
	cmake -S . -B $(FIRMWARE_BUILD) \
		-DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake

configure-tests:
	cmake -S . -B $(TEST_BUILD) \
		-DBUILD_TESTS=ON

configure-board-tests:
	cmake -S BoardTests -B $(BOARD_TEST_BUILD) \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/cmake/arm-gcc-toolchain.cmake


# ------------------------------------------------------------------
# Build
# ------------------------------------------------------------------

firmware: configure-firmware
	cmake --build $(FIRMWARE_BUILD)

tests: configure-tests
	cmake --build $(TEST_BUILD)
	ctest --test-dir $(TEST_BUILD) --output-on-failure

board-tests: configure-board-tests
	cmake --build $(BOARD_TEST_BUILD)

# ------------------------------------------------------------------
# Flash
# ------------------------------------------------------------------

flash:
	$(OPENOCD) \
		-f $(INTERFACE_CFG) \
		-f $(TARGET_CFG) \
		-c "program $(ELF) verify reset exit"

flash-board-tests:
	$(OPENOCD) \
		-f $(INTERFACE_CFG) \
		-f $(TARGET_CFG) \
		-c "program $(BOARD_TEST_ELF) verify reset exit"

# ------------------------------------------------------------------
# Rebuild
# ------------------------------------------------------------------

rebuild-firmware:
	rm -rf $(FIRMWARE_BUILD)
	$(MAKE) firmware

rebuild-tests:
	rm -rf $(TEST_BUILD)
	$(MAKE) tests

rebuild-board-tests:
	rm -rf $(BOARD_TEST_BUILD)
	$(MAKE) board-tests

# --------------------------------------------------
# Debugging infrastructure
# --------------------------------------------------

openocd:
	$(OPENOCD) \
		-f $(INTERFACE_CFG) \
		-f $(TARGET_CFG)

gdb:
	gdb-multiarch $(ELF) \
		-ex "target remote localhost:3333" \
		-ex "monitor reset halt"

swo:
	gdb-multiarch -batch -x scripts/swo.gdb $(ELF)

debug:
	gdb-multiarch -x scripts/debug.gdb $(ELF)

# --------------------------------------------------
# Debugging and test scenarios
# --------------------------------------------------

reset:
	gdb-multiarch -batch -x scripts/reset.gdb $(ELF)

reboot:
	gdb-multiarch -batch -x scripts/reboot.gdb $(ELF)

keypad-test:
	gdb-multiarch -x scripts/keypad_test.gdb $(ELF)

board-tests-run:
	gdb-multiarch -batch -x scripts/board-tests-swo.gdb $(BOARD_TEST_ELF)

# ------------------------------------------------------------------
# Inspection
# ------------------------------------------------------------------

size:
	arm-none-eabi-size $(ELF)

disasm:
	arm-none-eabi-objdump -d $(ELF) > disassembly.asm

symbols:
	arm-none-eabi-nm $(ELF)

sections:
	arm-none-eabi-readelf -S $(ELF)

elfinfo:
	arm-none-eabi-readelf -h $(ELF)

# ------------------------------------------------------------------
# Clean
# ------------------------------------------------------------------

clean:
	cmake --build $(FIRMWARE_BUILD) --target clean || true
	cmake --build $(TEST_BUILD) --target clean || true
	cmake --build $(BOARD_TEST_BUILD) --target clean || true

clean-all:
	rm -rf $(FIRMWARE_BUILD)
	rm -rf $(TEST_BUILD)
	rm -rf $(BOARD_TEST_BUILD)


# ------------------------------------------------------------------
# Static Analysis
# ------------------------------------------------------------------

misra:
	find App Drivers Platform -type f -name '*.c' ! -name 'sha256.c' -print0 | xargs -0 cppcheck --addon=misra --enable=warning,portability --inline-suppr --suppressions-list=misra_suppressions.txt --suppress=missingIncludeSystem --suppress='*:*/stm32f407xx.h' -DSTM32F407xx -I App -I Platform -I Drivers > misra.log 2>&1
	grep -oE 'misra-c[0-9]+-[0-9]+\.[0-9]+' misra.log | sort | uniq -c | sort -rn


ARM_INC := /usr/lib/gcc/arm-none-eabi/$(GCC_VER)/../../../arm-none-eabi/include

build-firmware/compile_commands.json: CMakeLists.txt
	$(MAKE) configure-firmware

tidy: build-firmware/compile_commands.json
	clang-tidy-18 -p build-firmware \
	  --extra-arg=--target=arm-none-eabi \
	  --extra-arg=-Wno-unknown-warning-option \
	  --extra-arg=-isystem$(ARM_INC)/c++/$(GCC_VER) \
	  --extra-arg=-isystem$(ARM_INC)/c++/$(GCC_VER)/arm-none-eabi \
	  --extra-arg=-isystem$(ARM_INC)/c++/$(GCC_VER)/backward \
	  --extra-arg=-isystem/usr/lib/gcc/arm-none-eabi/$(GCC_VER)/include \
	  --extra-arg=-isystem/usr/lib/gcc/arm-none-eabi/$(GCC_VER)/include-fixed \
	  --extra-arg=-isystem$(ARM_INC) \
	  $(shell find App Core -name '*.cpp')