cmake_minimum_required(VERSION 3.20)

project(
    stm32f407_access_control
    LANGUAGES C ASM
)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
    Core/main.c

    Platform/stm32f407xx_gpio.c

    Drivers/keypad.c

    App/access_control.c

    Startup/startup_stm32f407xx.s
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        Core
        Platform
        Drivers
        App
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -Wall
        -Wextra
)

target_link_options(${PROJECT_NAME}
    PRIVATE
        -T${CMAKE_SOURCE_DIR}/Startup/STM32F407VGTx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
)