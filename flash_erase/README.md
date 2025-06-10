# Flash_Erase

![Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-pinout.svg)  
*A utility to erase a sector of Flash memory on the Raspberry Pi Pico (RP2040-based microcontroller) and display its contents before and after the operation.*

## Overview

**Flash_Erase** is a C-based project designed for the Raspberry Pi Pico, utilizing the RP2040 microcontroller. This program demonstrates how to erase a specific sector of the onboard Flash memory and provides a hexadecimal dump of the memory contents before and after the erase operation. It’s a valuable tool for developers exploring Flash memory management, debugging, or experimenting with low-level operations on the RP2040.

Built with the Pico SDK, this project showcases the use of the `flash_range_erase` function while ensuring safe execution with interrupt handling.

## Features

- Erases a specified sector of Flash memory at a configurable offset.
- Displays the Flash contents in hexadecimal format before and after erasing.
- Uses interrupt-safe Flash erasing with `save_and_disable_interrupts` and `restore_interrupts`.
- Compatible with the RP2040’s XIP (Execute-In-Place) Flash memory.
- Lightweight and easy to modify for different memory regions or sizes.

## Requirements

- **Hardware**: Raspberry Pi Pico (or any RP2040-based board).
- **Software**: 
  - Pico SDK (installed and configured).
  - A C compiler (e.g., `gcc-arm-none-eabi`).
  - A serial terminal (e.g., PuTTY, Minicom, or Picocom) to view the output.
- **Optional**: CMake and a compatible IDE (e.g., VS Code) for building the project.

## Installation

1. **Set Up the Pico SDK**  
   Follow the official [Raspberry Pi Pico C/C++ SDK guide](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html) to install and configure the Pico SDK on your system. Ensure the `PICO_SDK_PATH` environment variable is set.

2. **Clone the Repository**  
   ```bash
   git clone https://github.com/CarlosMartinezPerez/Flash_Erase.git
   cd Flash_Erase
   ```

3. **Build the Project**  
   Create a build directory and compile the code using CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
   This will generate a `.uf2` file (e.g., `Flash_Erase.uf2`) in the `build` directory.

4. **Flash the Pico**  
   - Hold the `BOOTSEL` button on your Raspberry Pi Pico.
   - Connect it to your computer via USB (it will appear as a mass storage device).
   - Drag and drop the `Flash_Erase.uf2` file onto the Pico’s storage.
   - The Pico will automatically reboot and run the program.

## Usage

1. **Connect to Serial**  
   Open a serial terminal (e.g., Minicom or PuTTY) and connect to the Pico’s serial port (typically at 115200 baud). On Linux, this might be `/dev/ttyACM0`; on Windows, check Device Manager for the COM port.

2. **View the Output**  
   After a 3-second delay (to allow serial connection), the program will:
   - Display the Flash contents at the target address before erasing.
   - Erase the specified Flash sector.
   - Display the Flash contents after erasing.  
   Example output:
   ```
   Conteúdo da Flash antes de apagar:
   101FF000: 12 34 56 78 9A BC DE F0 12 34 56 78 9A BC DE F0

   Apagando setor da Flash no endereço: 0x101FF000

   Conteúdo da Flash após apagar:
   101FF000: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF

   Operação concluída!
   ```

3. **Customize the Target**  
   Modify the following constants in `main.c` to adjust the erase operation:
   - `FLASH_TARGET_OFFSET`: The offset from `XIP_BASE` (default is `0x1FF000`, near the end of the 2 MB Flash).
   - `FLASH_SECTOR_SIZE`: Defined in the Pico SDK as 4096 bytes (one sector).

## Code Explanation

- **`stdio_init_all()`**: Initializes standard I/O for serial communication.
- **`sleep_ms(3000)`**: Waits 3 seconds to ensure the serial terminal is connected.
- **`dump_flash()`**: A helper function to print a hexadecimal dump of the specified memory region.
- **`save_and_disable_interrupts()`** and **`restore_interrupts()`**: Ensures the Flash erase operation is atomic by disabling interrupts.
- **`flash_range_erase()`**: Erases the specified Flash sector (offset from `XIP_BASE`).

## RP2040 Memory Structure

The RP2040 microcontroller features a 2 MB onboard Flash memory, accessible starting at address `0x10000000` (defined as `XIP_BASE` in the Pico SDK). This Flash is used for Execute-In-Place (XIP) operation, allowing the CPU to run code directly from it. The memory map also includes 264 KB of SRAM split across six banks, but this project focuses solely on the Flash region. The `FLASH_TARGET_OFFSET` in the code is added to `XIP_BASE` to define the starting point for the erase operation, and care should be taken to ensure the offset and size stay within the 2 MB boundary (`0x10000000` to `0x101FFFFF`).

## Notes

- The default `FLASH_TARGET_OFFSET` is set to `0x1FF000` (the last sector of the 2 MB Flash). Adjust this value carefully to avoid erasing critical data, such as the program itself.
- Flash erasing sets all bits in the sector to `0xFF` (erased state).
- The RP2040’s Flash is organized in 4 KB sectors, and `FLASH_SECTOR_SIZE` is fixed at 4096 bytes.
- This operation is destructive—ensure the target region is safe to erase.

## Contributing

Feel free to fork this repository, submit issues, or send pull requests! Suggestions for enhancements (e.g., adding write functionality, multi-sector erasing, or error checking) are welcome.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Built with the [Pico SDK](https://github.com/raspberrypi/pico-sdk) by Raspberry Pi.
- Inspired by embedded systems programming and Flash memory management techniques.
