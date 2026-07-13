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


option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    add_subdirectory(Tests)
else()
    add_executable(${PROJECT_NAME}
        Core/main.cpp
        Startup/startup_stm32f407xx.s
        Platform/stm32f407xx_gpio.c
        Drivers/keypad.c

        App/Adapters/KeypadAdapter.cpp
        App/Adapters/LockOutputAdapter.cpp
        App/PinValidator.cpp
        App/AttemptCounter.cpp
        App/AccessController.cpp
    )

    target_include_directories(${PROJECT_NAME}
        PRIVATE
            Core
            Platform
            Drivers
            App
            App/Interfaces
            App/Adapters
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
            -fno-unwind-tables
            -fno-asynchronous-unwind-tables
        >
    )

    target_link_options(${PROJECT_NAME}
        PRIVATE
            -T${CMAKE_SOURCE_DIR}/Startup/STM32F407VGTx_FLASH.ld
            -mcpu=cortex-m4
            -mthumb
            -nostartfiles
            -nostdlib
    )

    target_link_libraries(${PROJECT_NAME}
        gcc
        c
        nosys
    )

endif()
