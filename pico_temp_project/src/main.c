#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "temp_sensor.h"

int main() {
    stdio_init_all();
    adc_init();
    
    // Select ADC input 4 (temperature sensor)
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    while (true) {
        // Read ADC value
        uint16_t adc_val = adc_read();
        // Convert to temperature
        float temp = adc_to_celsius(adc_val);
        // Print to serial
        printf("Temperature: %.2f C\n", temp);
        sleep_ms(1000);
    }

    return 0;
}