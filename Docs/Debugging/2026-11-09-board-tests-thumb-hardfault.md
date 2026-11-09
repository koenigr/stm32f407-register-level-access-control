# Debugging Protocol: STM32F407 Board Tests (Unity + SWO)

Date: 2026-11-09
Target: STM32F407, OpenOCD + `gdb-multiarch`, Unity v2.6.1 via CMake `FetchContent`
Goal: `make board-tests-run` runs Unity tests on hardware and reports the result via GDB/SWO.

## Result

Both tests pass (`Unity.TestFailures = 0`, `board_test_result = 0`, `BOARD TESTS PASSED`).

## Timeline

| # | Symptom | Cause | Fix |
|---|---------|-------|-----|
| 1 | `No source file named BoardTests.` | GDB script had `break BoardTests`, parsed as a file name | `break BoardTests_Done` |
| 2 | `Cannot configure TPIU/SWO; stm32f4x.tpiu is enabled!` | TPIU still enabled from an earlier OpenOCD session | Restart OpenOCD (only one instance running) |
| 3 | Script hangs, Ctrl+C shows `WWDG_IRQHandler` (= `Default_Handler`), `board_test_result = -1` | HardFault, see analysis below | Compile Unity with `-mcpu=cortex-m4 -mthumb` |
| 4 | Unity report missing in SWO log, only `BOARD_TESTS_FAILED` | `UNITY_INCLUDE_CONFIG_H` and include dirs only set on `board_tests`, not on the `unity` target | Definition and include dirs on `unity` (see below) |
| 5 | `TestExtiInterrupts:FAIL: Expected 1 Was 0` (line 37) | Interrupt taken a few cycles after `EXTI->SWIER` write, check ran first | `dsb` + `isb` after each `SWIER` write |

## Analysis of the HardFault (step 3)

Read in the GDB script after the stop in `Default_Handler`:

```gdb
x/wx 0xE000ED04        # ICSR
x/2wx 0xE000ED28       # CFSR, HFSR
x/8wx $sp              # stacked frame
info symbol <stacked pc>
```

| Register | Value | Meaning |
|----------|-------|---------|
| ICSR | `0x803` | VECTACTIVE = 3, HardFault |
| HFSR | `0x40000000` | FORCED (escalated configurable fault) |
| CFSR | `0x00010000` | UNDEFINSTR (UsageFault) |
| Stacked PC | `0x08003d18` | inside `UnityAssertEqualMemory` |
| Stacked LR | `0x08000389` | inside `main`, after `bl UnityBegin` |

Vector table was checked and was fine (`0x0800009c` -> `EXTI9_5_IRQHandler`, `0x080000e0` -> `EXTI15_10_IRQHandler`; `nm` showed both as strong `T` symbols).

`objdump` of the faulting region showed 32-bit ARM encodings (`e51b3020`, `ebfff71a`, ...). A Cortex-M4 executes Thumb only. `unity` is a separate CMake target created by `FetchContent`, so `target_compile_options(board_tests ...)` did not reach it.

## Final CMake changes (`BoardTests/CMakeLists.txt`)

`PROJECT_ROOT` must be set **before** these lines, otherwise paths collapse to `/Drivers`.

```cmake
FetchContent_MakeAvailable(Unity)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_options(unity PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -g
    -O0
)

target_compile_definitions(unity PRIVATE UNITY_INCLUDE_CONFIG_H)

target_include_directories(unity PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_ROOT}/Drivers
    ${PROJECT_ROOT}/Platform
)
```

Notes:
- `PUBLIC` on `target_include_directories(unity ...)` fails at generate time (absolute source-tree path in an exported interface). Use `PRIVATE`.
- `unity_config.h` includes `swo.h`, hence the `Drivers` include dir.

## Test fix (`board_test_main.cpp`)

```cpp
EXTI->PR = (1U << 8);
EXTI->SWIER = (1U << 8);
__asm volatile ("dsb" ::: "memory");
__asm volatile ("isb" ::: "memory");
```

Same after the `(1U << 10)` write.

## GDB script (final form)

```gdb
target extended-remote :3333
monitor reset halt
monitor tpiu config internal board-tests-swo.log uart off 16000000 125000
monitor itm port 0 on

load
monitor reset halt

break BoardTests_Done
break Default_Handler
continue

printf "\nUnity failure count: "
print board_test_result

if board_test_result == 0
	echo BOARD TESTS PASSED\n
else
	echo BOARD TESTS FAILED\n
	quit 1
end
```

Makefile target:

```make
board-tests-run:
	rm -f board-tests-swo.log
	gdb-multiarch -batch -x scripts/board-tests-swo.gdb $(BOARD_TEST_ELF)
```

## Lessons / gotchas

- `$xpsr` cannot be used in arithmetic in GDB (`Argument to arithmetic operation not a number or boolean`). Read exception number from ICSR (`0xE000ED04`, low 9 bits).
- In batch mode `continue` returns at the first breakpoint. A `break main` without a second `continue` makes the script print `-1` immediately.
- All weak IRQ handlers alias `Default_Handler`, so GDB shows `WWDG_IRQHandler` for any unhandled exception. Use ICSR to tell them apart.
- Build flags for `FetchContent` dependencies must be set on their own targets.
- Unity output goes to SWO via `UNITY_OUTPUT_CHAR` in `unity_config.h`. The log file is appended on each run, hence `rm -f`.
- Useful register dump after a run: ISER1 `0xE000E104` (bit 8 = IRQ40), ISPR1 `0xE000E204`, EXTI IMR `0x40013C00`, RTSR `0x40013C08`, FTSR `0x40013C0C`, PR `0x40013C14`.

## Open items

- Replace deprecated `monitor tpiu config ...` with `stm32f4x.tpiu configure` / `enable`.
- Root `CMakeLists.txt`: the `target_link_options(board_tests ...)` and `target_link_libraries(board_tests ...)` blocks in the `else()` branch reference a non-existent target, and `${PROJECT_ROOT}` is undefined there. The firmware target has no linker script.
- `malloc` is linked in (newlib via Unity/`-lc`), adds code size.