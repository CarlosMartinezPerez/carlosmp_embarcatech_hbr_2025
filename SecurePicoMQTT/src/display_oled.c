// display_oled.c
#include "include/display_oled.h"
#include <string.h>

#define I2C_SDA 14
#define I2C_SCL 15

void display_oled_inicializar_i2c(void) {
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void display_oled_limpar(void) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    struct render_area area_clear = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area_clear);
    render_on_display(fb, &area_clear);
}

void display_oled_inicializar(void) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, "ESCOLHA:");
    ssd1306_draw_string(fb, 0, 16, "A  PUBLISHER");
    ssd1306_draw_string(fb, 0, 32, "B  SUBSCRIBER");

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

void display_oled_mostrar_modo(const char* modo) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, (char*)modo);

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

void display_oled_exibir_mensagem(const char* msg1, const char* msg2, const char* topic, modo_t modo) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));

    if (modo == MODO_PUBLISHER) {
        ssd1306_draw_string(fb, 0, 0, "PUBLISHER");
    } else if (modo == MODO_SUBSCRIBER) {
        ssd1306_draw_string(fb, 0, 0, "SUBSCRIBER");
    }

    ssd1306_draw_string(fb, 0, 16, (char*)msg1);
    ssd1306_draw_string(fb, 0, 32, (char*)msg2);
    if (topic != NULL) { 
        ssd1306_draw_string(fb, 0, 48, (char*)topic);
    }

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}
