# Joystick Reader for BitDogLab (RP2040)

This task provides a C program to read analog and digital values from a joystick connected to a BitDogLab board, based on the Raspberry Pi RP2040 microcontroller as part of EMBARCATECH tasks. The program outputs the joystick's X-axis (VRx), Y-axis (VRy), and button (SW) states to the serial monitor, making it a versatile foundation for joystick-based applications study.

## Table of Contents
- [Hardware Setup](#hardware-setup)
- [Program Overview](#program-overview)
- [Code Explanation](#code-explanation)
- [Setup Instructions](#setup-instructions)
- [Potential Uses](#potential-uses)

## Hardware Setup
The joystick is connected to the BitDogLab board as follows:
- **VRx (X-axis)**: Connected to GPIO27 (ADC1)
- **VRy (Y-axis)**: Connected to GPIO26 (ADC0)
- **SW (Button)**: Connected to GPIO22 (Digital input with internal pull-up)

The joystick typically operates with:
- **VRx and VRy**: Analog outputs providing voltage levels proportional to the joystick's position, read via the RP2040's 12-bit ADC (0 to 4095).
- **SW**: A digital switch that connects to ground when pressed, configured with an internal pull-up resistor.

Ensure proper connections and verify that the joystick's VCC and GND pins are correctly wired to the BitDogLab's 3.3V and ground pins, respectively.

## Program Overview
The program initializes the RP2040's ADC and GPIO systems, reads the joystick's X and Y axis values (via ADC) and button state (via digital input), and prints them to the serial monitor. The values are processed as follows:
- **X and Y axes**: Raw ADC values (0 to 4095) are centered around 0 (mapped to approximately -512 to +511) for easier interpretation.
- **Button**: Reports `1` when pressed and `0` when released.

The output is displayed every 100 ms to avoid overwhelming the serial monitor.

## Code Explanation
The program (`joystick_conv.c`) includes:
- **Libraries**:
  - `pico/stdlib.h`: For standard Pico SDK functions (GPIO, stdio).
  - `hardware/adc.h`: For ADC configuration and reading.
- **Pin Definitions**:
  - VRx: GPIO27 (ADC1)
  - VRy: GPIO26 (ADC0)
  - SW: GPIO22 (digital input)
- **Initialization**:
  - Configures the ADC for VRx and VRy.
  - Sets up GPIO22 as an input with an internal pull-up for the button.
  - Initializes serial communication (USB or UART, depending on configuration).
- **Main Loop**:
  - Reads VRx and VRy via ADC (12-bit resolution).
  - Reads the button state (inverted logic due to pull-up).
  - Centers ADC values by subtracting 2048 (half of 4095) to produce a range of ~-512 to +511.
  - Outputs the results to the serial monitor using `printf`.
  - Adds a 100 ms delay for stable output.

The output format is:  
Eixo X: -17, Eixo Y: -75, Chave: 0  
Eixo X: -17, Eixo Y: -75, Chave: 0  
Eixo X: -17, Eixo Y: -75, Chave: 0  
Eixo X: -17, Eixo Y: -74, Chave: 0  
Eixo X: -17, Eixo Y: -75, Chave: 0  
Eixo X: -17, Eixo Y: -74, Chave: 0  
Eixo X: -17, Eixo Y: -75, Chave: 0  
Eixo X: -17, Eixo Y: -74, Chave: 0  
  
## Setup Instructions
### Prerequisites
- **Hardware**:
  - BitDogLab board (RP2040-based).
  - Joystick module connected as specified.
  - USB cable for programming and serial communication.
- **Software**:
  - Pico SDK installed.
  - A C/C++ compiler for ARM (e.g., `arm-none-eabi-gcc`).
  - CMake for building the project.
  - A serial terminal (e.g., PuTTY, Minicom, or VS Code with Pico extension) set to 115200 baud.

### Steps
1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd joystick-bitdoglab
   ```

2. **Set Up Pico SDK**:
   - Ensure the Pico SDK is installed and the `PICO_SDK_PATH` environment variable is set.
   - Example: `export PICO_SDK_PATH=/path/to/pico-sdk`

3. **Build the Project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Flash the Firmware**:
   - Hold the BOOTSEL button on the BitDogLab board and connect it via USB.
   - Copy the generated `joystick_conv.uf2` file (in the `build` directory) to the RP2040's mass storage device.

5. **Monitor Output**:
   - Open a serial terminal (e.g., `minicom -D /dev/ttyACM0 -b 115200` or equivalent).
   - Reset the board to start the program.
   - Move the joystick and press the button to observe the output.

### CMake Configuration
The provided `CMakeLists.txt` includes:
```cmake
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(joystick_conv C CXX ASM)
pico_sdk_init()
add_executable(joystick_conv joystick_conv.c)
pico_enable_stdio_usb(joystick_conv 1)
pico_enable_stdio_uart(joystick_conv 0)
target_link_libraries(joystick_conv pico_stdlib hardware_adc)
pico_add_extra_outputs(joystick_conv)
```
This configures the project for USB serial output and links the necessary Pico SDK libraries.

## Potential Uses
The joystick peripheral, combined with this program, can serve as a building block for various applications, including:

1. **Gaming Controllers**:
   - Use the joystick to control characters or objects in a game, with the X/Y axes for movement and the button for actions (e.g., jumping, shooting).
   - Extend the program to interface with a PC or console via USB HID.

2. **Robotics**:
   - Control a robot's movement (e.g., differential drive for wheeled robots) by mapping X/Y values to motor speeds.
   - Use the button to toggle modes (e.g., manual vs. autonomous).

3. **Human-Machine Interfaces**:
   - Create an interface for navigating menus or controlling cursors on a display (e.g., LCD or OLED connected to the RP2040).
   - Map joystick movements to specific commands for IoT devices.

4. **Data Visualization**:
   - Log joystick data for analysis or visualization (e.g., plot X/Y positions to study user behavior).
   - Stream data to a computer via serial for real-time applications.

5. **Educational Projects**:
   - Teach concepts like analog-to-digital conversion, GPIO handling, and serial communication.
   - Use as a starting point for learning embedded systems programming with the RP2040.

6. **Prototyping**:
   - Prototype control systems for drones, RC vehicles, or other devices requiring 2D input.
   - Combine with other sensors (e.g., accelerometers) for more complex interactions.

To enhance functionality, consider:
- Adding a calibration routine to handle joystick drift or non-linearities.
- Implementing a mapping function to scale values (e.g., 0 to 100 or -100 to 100).
- Filtering ADC readings (e.g., moving average) to reduce noise.
- Integrating with communication protocols (e.g., I2C, SPI, or USB) for external devices.
