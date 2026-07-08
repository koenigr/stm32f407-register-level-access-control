cmake_minimum_required(VERSION 3.20)

project(
    stm32f407_access_control
    LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    Core/main.cpp
    Startup/startup_stm32f407xx.s
    Platform/stm32f407xx_gpio.c
    Drivers/keypad.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        Core
        Platform
        Drivers
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -Wall
        -Wextra
        -g
        -O0
    $<$<COMPILE_LANGUAGE:CXX>:
        -fno-exceptions
        -fno-rtti
    >
)

target_link_options(${PROJECT_NAME}
    PRIVATE
        -T${CMAKE_SOURCE_DIR}/Startup/STM32F407VGTx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -nostartfiles
)