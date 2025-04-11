# Counter Interrupt Task

A simple task for the Raspberry Pi Pico that uses interrupts to count button presses and display results on an OLED screen.

- **How it works**: Press Button A (GPIO5) to start a countdown from 9 to 0. Press Button B (GPIO6) during the countdown to increment a counter. The OLED (SSD1306, I2C) shows the countdown and Button B presses.
- **Hardware**: Uses two buttons and an OLED display connected via I2C (SDA on GPIO14, SCL on GPIO15).  
**Challenge**: Uses two buttons and an OLED display connected via I2C (SDA on GPIO14, SCL on GPIO15).
- **Challenge**: Uses a 200ms debounce delay to handle button bounce, but even so, the behavior is still erratic and sometimes misses presses.