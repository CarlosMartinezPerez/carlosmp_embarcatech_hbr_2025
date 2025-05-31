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

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(SecurePicoMQTT C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(SecurePicoMQTT 
        main.c 
        src/display_oled.c
        src/wifi_conn.c 
        src/mqtt_comm.c 
        src/xor_cipher.c
        src/tratar_gpio.c
        src/mqtt_publisher.c
        src/mqtt_subscriber.c
        oled/ssd1306_i2c.c
)

pico_set_program_name(SecurePicoMQTT "SecurePicoMQTT")
pico_set_program_version(SecurePicoMQTT "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(SecurePicoMQTT 0)
pico_enable_stdio_usb(SecurePicoMQTT 1)

# Add the standard library to the build
target_link_libraries(SecurePicoMQTT
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(SecurePicoMQTT PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(SecurePicoMQTT 
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
        pico_lwip
        pico_cyw43_driver
        hardware_i2c
)

pico_add_extra_outputs(SecurePicoMQTT)

