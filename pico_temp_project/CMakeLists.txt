# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Import Pico SDK
include(pico_sdk_import.cmake)

project(pico_temp_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize Pico SDK
pico_sdk_init()

# Production executable
add_executable(pico_temp
    src/main.c
    src/temp_sensor.c
)

# Link libraries for production
target_link_libraries(pico_temp
    pico_stdlib
    hardware_adc
)

# Enable USB output, disable UART
pico_enable_stdio_usb(pico_temp 1)
pico_enable_stdio_uart(pico_temp 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(pico_temp)

# Test executable (only compiled if PICO_TEST is defined)
option(PICO_TEST "Build test executable" OFF)
if (PICO_TEST)
    add_executable(test_temp
        src/temp_sensor.c
        test/test_temp.c
        test/unity/unity.c
    )

    # Link libraries for test
    target_link_libraries(test_temp
        pico_stdlib
    )

    # Enable USB output for tests
    pico_enable_stdio_usb(test_temp 1)
    pico_enable_stdio_uart(test_temp 0)

    # Create map/bin/hex/uf2 files for test
    pico_add_extra_outputs(test_temp)

    # Define TEST macro for conditional compilation
    target_compile_definitions(test_temp PRIVATE TEST)
endif()