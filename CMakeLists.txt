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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
else()
    add_executable(${PROJECT_NAME}
        Core/main.cpp
        Startup/startup_stm32f407xx.s
        Platform/stm32f407xx_gpio.c
        Platform/stm32f407xx_exti.c
        Drivers/keypad.c
        Drivers/swo.cpp

        App/Adapters/KeypadAdapter.cpp
        App/Adapters/LedOutputAdapter.cpp
        App/Adapters/SysTickClock.cpp
        App/PinValidator.cpp
        App/sha256.c
        App/AttemptCounter.cpp
        App/AccessController.cpp

        App/cpp_runtime.cpp
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
            -Wconversion
            -Wshadow
            -g
            -O0
        $<$<COMPILE_LANGUAGE:CXX>:
            -fno-exceptions
            -fno-rtti
            -fno-unwind-tables
            -fno-asynchronous-unwind-tables
        >
    )

    # Handle bare-metal linking using toolchain specs rather than -nostdlib
    target_link_options(board_tests PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -T${PROJECT_ROOT}/Startup/STM32F407VGTx_FLASH.ld
        -nostartfiles
        -Wl,--gc-sections
    )

    # Link your internal test framework targets
    target_link_libraries(board_tests PRIVATE 
        unity
        --specs=nano.specs        # Uses the lightweight Newlib-nano
        --specs=nosys.specs       # Provides empty stubs for _write, _close, etc. satisfying the linker
    )

endif()
