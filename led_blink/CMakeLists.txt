# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Inicializa o Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(led_blink C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(led_blink led_blink.c )

pico_set_program_name(led_blink "led_blink")
pico_set_program_version(led_blink "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(led_blink 0)
pico_enable_stdio_usb(led_blink 0)

# Add the standard library to the build
target_link_libraries(led_blink 
    pico_stdlib
    hardware_clocks 
    hardware_pll)

# Usa um linker script personalizado
pico_set_linker_script(led_blink ${CMAKE_CURRENT_SOURCE_DIR}/memmap_custom.ld)

# Add the standard include files to the build
target_include_directories(led_blink PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(led_blink)