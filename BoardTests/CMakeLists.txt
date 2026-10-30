cmake_minimum_required(VERSION 3.20)

project(stm32f407_board_tests LANGUAGES CXX C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(board_tests
    board_test_main.cpp
    ${PROJECT_ROOT}/Startup/startup_stm32f407xx.s
    ${PROJECT_ROOT}/Platform/stm32f407xx_gpio.c
    ${PROJECT_ROOT}/Platform/stm32f407xx_exti.c
    ${PROJECT_ROOT}/Drivers/keypad.c
    ${PROJECT_ROOT}/Drivers/swo.cpp
)

target_include_directories(board_tests PRIVATE
    ${PROJECT_ROOT}/Platform
    ${PROJECT_ROOT}/Drivers
)

target_compile_options(board_tests PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -Wall
    -Wextra
    -g
    -O0
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

target_compile_options(board_tests PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

target_link_options(board_tests PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -T${PROJECT_ROOT}/Startup/STM32F407VGTx_FLASH.ld
    -nostartfiles
    -nostdlib
    -Wl,--gc-sections
)

target_link_libraries(board_tests PRIVATE
	gcc
	c
	nosys
)