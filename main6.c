#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz
#define DELAY_BETWEEN_PHASES_MS 1000

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();

    while (true) {
        printf("Gravando 2 segundos de áudio...\n");
        adc_buffer_reset();
        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            uint16_t sample = adc_read_microphone();
            adc_buffer_add_sample(sample);
            sleep_us(SAMPLE_INTERVAL_US);
        }

        printf("Aguardando para reproduzir...\n");
        sleep_ms(DELAY_BETWEEN_PHASES_MS);

        printf("Reproduzindo sinal...\n");
        pwm_output_play_buffer();
        pwm_output_stop();
        printf("Reprodução concluída.\n");

        sleep_ms(DELAY_BETWEEN_PHASES_MS);
    }

    return 0;
}