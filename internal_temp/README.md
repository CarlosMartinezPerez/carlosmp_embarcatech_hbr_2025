# RP2040 Temperature Sensor

A simple program to read the internal temperature sensor of the RP2040 microcontroller (Raspberry Pi Pico) and convert the ADC reading to °C, as part of EMBARCATECH tasks.

## Features
- Reads the RP2040's built-in temperature sensor (ADC channel 4).
- Converts raw ADC values to temperature using the formula from the RP2040 datasheet.  
Section 4.9.5.  
- Prints temperature readings every second via USB serial monitor.

## Requirements
- Raspberry Pi Pico (or any RP2040-based board)
- [Pico SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)
- CMake and a build system (e.g., Make)
- Serial terminal (e.g., `minicom`, 115200 baud)

## Some readings:  
Temperatura: 39.31 °C  
Temperatura: 36.03 °C  
Temperatura: 34.63 °C  
Temperatura: 40.71 °C  
Temperatura: 37.44 °C  
Temperatura: 40.71 °C  
Temperatura: 36.50 °C  
Temperatura: 37.44 °C  
Temperatura: 37.91 °C  
Temperatura: 34.16 °C  

## Build and Run
1. Clone this repository:
   ```bash
   git clone <your-repo-url>
   cd rp2040_temp_sensor