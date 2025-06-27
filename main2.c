#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"

#define SAMPLE_INTERVAL_US 125  // 8 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000);  // tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();

    printf("Iniciando aquisição de 2 segundos de áudio...\n");

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_read_microphone();
        adc_buffer_add_sample(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    printf("Amostras capturadas:\n");
    adc_buffer_print();

    return 0;
}
