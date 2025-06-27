#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"
#include "button.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();
    button_init();

    bool is_recording = false;
    bool has_recorded = false;

    while (true) {
        // Detecta pressionamento do Botão A (gravação)
        if (button_a_pressed() && !is_recording) {
            printf("Botão A pressionado: Gravando 2 segundos de áudio...\n");
            adc_buffer_reset();
            is_recording = true;
            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                uint16_t sample = adc_read_microphone();
                adc_buffer_add_sample(sample);
                sleep_us(SAMPLE_INTERVAL_US);
            }
            is_recording = false;
            has_recorded = true;
            printf("Gravação concluída.\n");
        }

        // Detecta pressionamento do Botão B (reprodução)
        if (button_b_pressed() && !is_recording && has_recorded) {
            printf("Botão B pressionado: Reproduzindo sinal...\n");
            pwm_output_play_buffer();
            pwm_output_stop();
            printf("Reprodução concluída.\n");
        }

        sleep_us(10); // Pequeno delay para evitar sobrecarga
    }

    return 0;
}