#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <string.h>
#include <hardware/i2c.h>
#include "inc/ssd1306.h"
#include "yin.h"

#define BOTAO_DESCER_NOTA 5
#define BOTAO_SUBIR_NOTA 6
#define PINO_BUZZER 21
#define I2C_SDA 14
#define I2C_SCL 15
#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12

#define INDICE_E2 0
#define INDICE_A2 1
#define INDICE_D3 2
#define INDICE_G3 3
#define INDICE_B3 4
#define INDICE_E4 5

#define AMOSTRAS 256
#define FREQUENCIA_AMOSTRAGEM 4096
#define ADC_MICROFONE 2
#define PINO_MICROFONE 28
#define TOLERANCIA 2.0

float frequencias[] = { 82.4, 110.0, 146.8, 196.0, 246.9, 329.6 };
char* notas[] = { "E2", "A2", "D3", "G3", "B3", "E4" };
uint16_t periodoAmostragem;
uint8_t indiceNotaAtual = 0;
float valoresAmostrados[AMOSTRAS];

void inicializar_adc_microfone(void) {
    adc_gpio_init(PINO_MICROFONE);
    adc_select_input(ADC_MICROFONE);
}

void inicializar_pwm_buzzer(uint pino) {
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    float clkdiv = (float)clock_get_hz(clk_sys) / (1000.0 * 4096.0);
    pwm_config_set_clkdiv(&config, clkdiv);
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pino, 0);
}

void inicializar_botoes(void) {
    gpio_init(BOTAO_DESCER_NOTA);
    gpio_init(BOTAO_SUBIR_NOTA);
    gpio_set_dir(BOTAO_DESCER_NOTA, GPIO_IN);
    gpio_set_dir(BOTAO_SUBIR_NOTA, GPIO_IN);
    gpio_pull_up(BOTAO_DESCER_NOTA);
    gpio_pull_up(BOTAO_SUBIR_NOTA);
}

void inicializar_led_rgb(void) {
    gpio_init(LED_VERMELHO);
    gpio_init(LED_VERDE);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
}

void inicializar_oled_i2c(void) {
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void emitir_beep(uint pino, uint duracao_ms) {
    pwm_set_gpio_level(pino, 2048);
    sleep_ms(duracao_ms);
    pwm_set_gpio_level(pino, 0);
    sleep_ms(50);
}

void beep_limite_alcancado() {
    emitir_beep(PINO_BUZZER, 100);
    sleep_ms(100);
    emitir_beep(PINO_BUZZER, 100);
}

void aguardar_pressionamento_botao(uint pino_botao) {
    while (gpio_get(pino_botao));
    sleep_ms(20);
    emitir_beep(PINO_BUZZER, 40);
    while (!gpio_get(pino_botao));
    sleep_ms(20);
}

void amostrar_microfone(void) {
    for (uint16_t i = 0; i < AMOSTRAS; i++) {
        valoresAmostrados[i] = (float)adc_read() / 4096.0;
        busy_wait_us(periodoAmostragem);
    }
}

void definir_estado_led_rgb(bool vermelho, bool verde, bool azul) {
    gpio_put(LED_VERMELHO, vermelho);
    gpio_put(LED_VERDE, verde);
    gpio_put(LED_AZUL, azul);
}

void definirNotaAlvo(void) {
    if (!gpio_get(BOTAO_DESCER_NOTA)) {
        aguardar_pressionamento_botao(BOTAO_DESCER_NOTA);
        if (indiceNotaAtual > 0) {
            indiceNotaAtual--;
        } else {
            beep_limite_alcancado();
        }
    }
    if (!gpio_get(BOTAO_SUBIR_NOTA)) {
        aguardar_pressionamento_botao(BOTAO_SUBIR_NOTA);
        if (indiceNotaAtual < 5) {
            indiceNotaAtual++;
        } else {
            beep_limite_alcancado();
        }
    }
}

int main() {
    stdio_usb_init();
    adc_init();
    inicializar_adc_microfone();
    inicializar_oled_i2c();
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    ssd1306_init();
    inicializar_led_rgb();
    definir_estado_led_rgb(false, false, false);
    inicializar_pwm_buzzer(PINO_BUZZER);
    inicializar_botoes();
    Yin yin;
    periodoAmostragem = 1000000 / FREQUENCIA_AMOSTRAGEM;
    Yin_init(&yin, AMOSTRAS, 0.2);

    struct render_area area_exibicao = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area_exibicao);
    static uint8_t buffer_display[ssd1306_buffer_length];
    memset(buffer_display, 0, ssd1306_buffer_length);
    render_on_display(buffer_display, &area_exibicao);

    int estadoAnterior = -1;

    while (1) {
        definirNotaAlvo();
        float frequenciaAlvo = frequencias[indiceNotaAtual];
    
        memset(buffer_display, 0, ssd1306_buffer_length);
    
        ssd1306_draw_string(buffer_display, 5, 0, "  AFINADOR NA  ");
        ssd1306_draw_string(buffer_display, 5, 15, "   BITDOGLAB   ");
    
        char textoNota[16];
        sprintf(textoNota, "    Nota: %s ", notas[indiceNotaAtual]);
        ssd1306_draw_string(buffer_display, 5, 34, textoNota);
    
        amostrar_microfone();
        float frequenciaAtual = Yin_getPitch(&yin, valoresAmostrados);
        
        int novoEstado = 0;
        char statusTexto[10] = "         ";
    
        if (frequenciaAtual == -1) {
            definir_estado_led_rgb(false, false, false);
        } else {
            float diferencaFreq = frequenciaAtual - frequenciaAlvo;
            if (fabs(diferencaFreq) < TOLERANCIA) {
                novoEstado = 1;
                strcpy(statusTexto, " AFINADO ");
            } else if (frequenciaAtual > frequenciaAlvo) {
                novoEstado = 2;
                strcpy(statusTexto, "  ACIMA  ");
            } else {
                novoEstado = 3;
                strcpy(statusTexto, " ABAIXO  ");
            }
        }
    
        ssd1306_draw_string(buffer_display, 32, 56, statusTexto);
        render_on_display(buffer_display, &area_exibicao);
    
        if (novoEstado != estadoAnterior) {
            definir_estado_led_rgb(novoEstado == 3, novoEstado == 1, novoEstado == 2);
            estadoAnterior = novoEstado;
        }
    
        sleep_ms(300);
    }
}
