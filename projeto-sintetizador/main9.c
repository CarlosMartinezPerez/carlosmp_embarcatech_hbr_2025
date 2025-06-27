#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"
#include "button.h"
#include "led.h"
#include "oled/ssd1306.h"
#include "audio_visualizer.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();
    button_init();
    led_init();
    ssd1306_init();
    audio_visualizer_init();

    bool is_recording = false;
    bool has_recorded = false;

    ssd1306_clear();
    ssd1306_draw_string(0, 0, "GRAVADOR PRONTO");
    ssd1306_draw_string(0, 8, "PRESSIONE A");
    ssd1306_show();
    sleep_ms(1000);

    while (true) {
        // Detecta pressionamento do Botão A (gravação)
        if (button_a_pressed() && !is_recording) {
            printf("Botão A pressionado: Gravando 2 segundos de áudio...\n");
            led_set_red(true); // Acende LED vermelho
            adc_buffer_reset();
            is_recording = true;
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "GRAVANDO");
            ssd1306_show();

            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                uint16_t sample = adc_read_microphone();
                adc_buffer_add_sample(sample);
                sleep_us(SAMPLE_INTERVAL_US);
            }
            is_recording = false;
            has_recorded = true;
            led_off(); // Apaga LED
            printf("Gravação concluída.\n");

            // Após gravar, visualize o sinal no OLED
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "FORMA DE ONDA:");
            audio_visualizer_draw_waveform(); // Chama a função para desenhar a forma de onda
            sleep_ms(3000); // Exibe a forma de onda por alguns segundos
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "PRESIONE B");
            ssd1306_show();
        }

        // Detecta pressionamento do Botão B (reprodução)
        if (button_b_pressed() && !is_recording && has_recorded) {
            printf("Botão B pressionado: Reproduzindo sinal...\n");
            led_set_green(true); // Acende LED verde
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "TOCANDO...");
            ssd1306_show();
            pwm_output_play_buffer();
            pwm_output_stop();
            led_off(); // Apaga LED
            printf("Reprodução concluída.\n");
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "FEITO");
            ssd1306_show();
            sleep_ms(1000);
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "PRESSIONE A");
            ssd1306_show();
        }

        sleep_us(10); // Pequeno delay para evitar sobrecarga
    }
    return 0;
}