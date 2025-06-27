#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa o visualizador de áudio.
 * Deve ser chamado após a inicialização do SSD1306.
 */
void audio_visualizer_init(void);

/**
 * @brief Desenha o sinal de áudio gravado no display OLED.
 * Os dados são lidos do adc_buffer.
 */
void audio_visualizer_draw_waveform(void);

#endif