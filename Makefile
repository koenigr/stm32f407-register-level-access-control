.PHONY: help all firmware tests flash debug clean clean-all \
        configure-firmware configure-tests rebuild-firmware rebuild-tests \
        gdb openocd size disasm symbols sections elfinfo

# ------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------

FIRMWARE_BUILD = build-firmware
TEST_BUILD     = build-tests
PROJECT        = stm32f407_access_control
ELF            = $(FIRMWARE_BUILD)/$(PROJECT)

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

# ------------------------------------------------------------------
# Configure
# ------------------------------------------------------------------

configure-firmware:
	cmake -S . -B $(FIRMWARE_BUILD) \
		-DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake

configure-tests:
	cmake -S . -B $(TEST_BUILD) \
		-DBUILD_TESTS=ON

# ------------------------------------------------------------------
# Build
# ------------------------------------------------------------------

firmware:
	cmake --build $(FIRMWARE_BUILD)

tests:
	cmake --build $(TEST_BUILD)
	./$(TEST_BUILD)/Tests/unit_tests

# ------------------------------------------------------------------
# Flash
# ------------------------------------------------------------------

flash:
	$(OPENOCD) \
		-f $(INTERFACE_CFG) \
		-f $(TARGET_CFG) \
		-c "program $(ELF) verify reset exit"

# ------------------------------------------------------------------
# Rebuild
# ------------------------------------------------------------------

rebuild-firmware:
	rm -rf $(FIRMWARE_BUILD)
	$(MAKE) configure-firmware
	$(MAKE) firmware

rebuild-tests:
	rm -rf $(TEST_BUILD)
	$(MAKE) configure-tests
	$(MAKE) tests

# ------------------------------------------------------------------
# Debugging
# ------------------------------------------------------------------

openocd:
	$(OPENOCD) \
		-f $(INTERFACE_CFG) \
		-f $(TARGET_CFG)

gdb:
	gdb-multiarch $(ELF) \
		-ex "target remote localhost:3333" \
		-ex "monitor reset halt"

debug: openocd

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

clean-all:
	rm -rf $(FIRMWARE_BUILD)
	rm -rf $(TEST_BUILD)