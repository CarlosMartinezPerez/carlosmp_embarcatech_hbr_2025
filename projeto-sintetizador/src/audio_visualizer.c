// audio_visualizer.c (apenas a função modificada)

#include "audio_visualizer.h"
#include "ssd1306.h"
#include "adc_buffer.h"
#include <stdlib.h> // Para malloc, free, abs
#include <string.h> // Para memset

// Dimensões do display
#define DISPLAY_WIDTH SSD1306_WIDTH
#define DISPLAY_HEIGHT SSD1306_HEIGHT

#define ADC_MAX_VALUE 4095 // Valor máximo de um ADC de 12 bits

// Fator de amplificação visual para a onda
#define WAVEFORM_AMPLIFICATION_FACTOR 3.0f 

// Define a altura da área para o texto
#define TEXT_AREA_HEIGHT 16 // Duas linhas de 8 pixels para o texto "Waveform:"

void audio_visualizer_init(void) {
    // Nenhuma mudança aqui
}

void audio_visualizer_draw_waveform(void) {
    ssd1306_clear(); // Limpa o buffer do display

    // Desenha o texto "Waveform:" no topo do display
    ssd1306_draw_string(0, 0, "FORMA DE ONDA:");

    size_t count = adc_buffer_get_count();

    // Define a área de desenho da forma de onda APÓS o texto
    int waveform_start_y = TEXT_AREA_HEIGHT; // Começa a desenhar a onda abaixo do texto
    int waveform_height = DISPLAY_HEIGHT - waveform_start_y; // Altura disponível para a onda

    // Calcula o passo horizontal para preencher a largura do display
    float samples_per_pixel = (float)count / DISPLAY_WIDTH;

    // Desenha uma linha central para referência na área da forma de onda
    ssd1306_draw_line(0, waveform_start_y + waveform_height / 2,
                      DISPLAY_WIDTH - 1, waveform_start_y + waveform_height / 2, true);

    // Ponto médio do ADC para centralizar o sinal
    float adc_mid_point = (float)ADC_MAX_VALUE / 2.0f;
    // Metade da altura disponível para a forma de onda no display
    float display_waveform_mid_point = (float)(waveform_height - 1) / 2.0f;
    // Escala para mapear a amplitude máxima (da metade do ADC) para metade da altura da forma de onda
    float scale_factor = display_waveform_mid_point / adc_mid_point;


    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
        size_t sample_index = (size_t)(x * samples_per_pixel);

        if (sample_index < count) {
            uint16_t adc_value = adc_buffer_get_sample(sample_index);

            // 1. Calcula o desvio do ponto médio do ADC
            float normalized_adc_value = (float)adc_value - adc_mid_point;

            // 2. Amplifica o desvio
            float amplified_value = normalized_adc_value * WAVEFORM_AMPLIFICATION_FACTOR;
            
            // 3. Mapeia o valor amplificado para a altura disponível da forma de onda, centralizado
            int y_pixel_relative = (int)(amplified_value * scale_factor + display_waveform_mid_point);

            // Adiciona o offset vertical para desenhar abaixo do texto
            int y_pixel = waveform_start_y + y_pixel_relative;

            // Garante que o pixel esteja dentro dos limites da área de forma de onda (e do display)
            if (y_pixel < waveform_start_y) y_pixel = waveform_start_y;
            if (y_pixel >= DISPLAY_HEIGHT) y_pixel = DISPLAY_HEIGHT - 1;

            // Desenha o pixel
            ssd1306_draw_pixel(x, y_pixel, true);
        }
    }
    ssd1306_show(); // Envia o framebuffer para o display
}