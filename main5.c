#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();

    printf("Gravando 2 segundos de áudio...\n");

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_read_microphone();
        adc_buffer_add_sample(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }
    
    printf("Reproduzindo sinal...\n");

    adc_buffer_reset();

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_buffer_get_next_sample();
        pwm_output_set_duty_from_adc(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    pwm_output_stop();
    printf("Reprodução concluída.\n");

    return 0;
}
