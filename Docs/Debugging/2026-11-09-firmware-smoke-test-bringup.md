# Firmware build and smoke-test bring-up (2026-11-09)

**Goal:** `make smoke-test` runs the STM32F407 firmware to `main()` on the board.

## Problems and fixes

| # | Symptom | Cause | Fix |
|---|---------|-------|-----|
| 1 | `smoke_test.gdb` not found | Wrong filename in Makefile | Renamed to `scripts/smoke-test.gdb` |
| 2 | CMake: target `board_tests` not built by this project | `target_link_options`/`target_link_libraries` for `board_tests` sat in the root `CMakeLists.txt` | Moved to `BoardTests/CMakeLists.txt` |
| 3 | Linker: undefined `_write`, `_sbrk`, `_close`, `_read`, `_lseek` | Firmware had no `--specs=nosys.specs` | Added `--specs=nano.specs` and `--specs=nosys.specs` to the firmware's `target_link_options` |
| 4 | `cannot open linker script /Startup/...ld` | `${PROJECT_ROOT}` is only set in `BoardTests/` | Use `${CMAKE_SOURCE_DIR}/Startup/STM32F407VGTx_FLASH.ld` |
| 5 | GDB: could not connect (timeout) | OpenOCD not reachable | Started OpenOCD with the board connected |
| 6 | Result | n/a | `PASS: Reached main()` |

## Open points

- `nosys` stubs return errors: implement `_write` (ITM/SWO) and `_sbrk` if `printf` or heap are used.
- Smoke test only proves startup reaches `main`; breakpoint 2 (`main.cpp:53`) is never checked.
- Optional: `set confirm off` in `smoke-test.gdb`, `adapter speed 1800` in the OpenOCD config.
- Optional: add OpenOCD start/stop to the `smoke-test` Makefile target.
- Root line 22 says `add_subdirectory(Tests)`, folder is `BoardTests`: verify.