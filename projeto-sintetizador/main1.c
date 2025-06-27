#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"

#define SAMPLE_INTERVAL_US 125  // 8kHz (1s / 8000 = 125µs)

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o monitor serial
    adc_reader_init();

    printf("Iniciando leitura contínua do microfone...\n");

    while (true) {
        uint16_t sample = adc_read_microphone();
        printf("%u\n", sample);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    return 0;
}
