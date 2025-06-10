# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Definição da placa
set(PICO_BOARD pico CACHE STRING "Board type")

# Configuração mínima do CMake
cmake_minimum_required(VERSION 3.13)

# Importação do SDK
include(pico_sdk_import.cmake)

# Nome do projeto e padrões de compilação
project(afinador C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Adiciona diretórios de inclusão
include_directories(${PROJECT_SOURCE_DIR})

# Inicializa o SDK do Pico
pico_sdk_init()

# Adiciona os arquivos-fonte ao executável
add_executable(afinador afinador.c yin.c ../inc/ssd1306_i2c.c)
target_compile_options(afinador PRIVATE -Wall -Wextra -Werror)



# Linka bibliotecas necessárias
target_link_libraries(afinador 
    pico_stdlib 
    hardware_adc 
    m 
    hardware_pwm 
    hardware_clocks 
    hardware_i2c
    )

# Habilita saída via USB e UART
pico_enable_stdio_usb(afinador 1)
pico_enable_stdio_uart(afinador 1)

# Gera arquivos extras (UF2, bin, etc.)
pico_add_extra_outputs(afinador)