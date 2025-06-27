#include <stdint.h>
#include "adc_reader.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define MIC_ADC_CHANNEL 2  // GPIO 28 → ADC2

void adc_reader_init(void) {
    adc_init();
    adc_gpio_init(28);                // GPIO 28 como entrada ADC
    adc_select_input(MIC_ADC_CHANNEL);
}

uint16_t adc_read_microphone(void) {
    return adc_read();               // Retorna valor de 12 bits (0–4095)
}
