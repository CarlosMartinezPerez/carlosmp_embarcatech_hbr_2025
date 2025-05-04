#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "display.h"

#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

void clear_display_buffer() {
    memset(display_buffer, 0, BUFFER_LENGTH);
}

void ssd1306_update_display() {
    uint8_t command_buffer[2];
    
    command_buffer[0] = 0x00;
    command_buffer[1] = 0x21;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x00;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x7F;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    
    command_buffer[1] = 0x22;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x00;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x07;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    
    uint8_t data_buffer[1025];
    data_buffer[0] = 0x40;
    memcpy(&data_buffer[1], display_buffer, BUFFER_LENGTH);
    i2c_write_blocking(i2c1, 0x3C, data_buffer, BUFFER_LENGTH + 1, false);
}

void ssd1306_setup() {
    uint8_t init_commands[] = {
        0x00, 0xAE, 0x00, 0xD5, 0x80, 0x00, 0xA8, 0x3F, 0x00, 0xD3, 0x00,
        0x00, 0x40, 0x00, 0x8D, 0x14, 0x00, 0x20, 0x00, 0x00, 0xA1, 0x00,
        0xC8, 0x00, 0xDA, 0x12, 0x00, 0x81, 0xCF, 0x00, 0xD9, 0xF1, 0x00,
        0xDB, 0x40, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xAF
    };
    i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);
}

void init_display() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_setup();
    clear_display_buffer();
    ssd1306_update_display();
    clear_display_buffer();
    ssd1306_update_display();
}

void draw_histogram(int *histogram) {
    for (int i = 0; i < CHANNELS; i++) {
        if (histogram[i] > 0) {
            int height = histogram[i] / 2; // Cada 2 bolas adicionam 1 pixel de altura
            if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;
            for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {
                for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {
                    ssd1306_set_pixel(display_buffer, x, y, true);
                }
            }
        }
    }
}

void draw_ball(Ball *ball) {
    if (ball->active) {
        ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);
    }
}

void draw_probabilities(float left_prob) {
    char left_buffer[8];
    char right_buffer[8];
    snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);
    snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);
    ssd1306_draw_string(display_buffer, 0, 28, left_buffer); // Esquerda, y=28
    ssd1306_draw_string(display_buffer, 104, 28, right_buffer); // Direita, ajustado para caber
}

void update_display(Ball *balls, int ball_count, int *histogram) {
    clear_display_buffer();
    for (int i = 0; i < ball_count; i++) {
        draw_ball(&balls[i]);
    }
    draw_histogram(histogram);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Bolas: %d", total_balls);
    ssd1306_draw_string(display_buffer, 0, 0, buffer);
    draw_probabilities(get_left_probability());
    ssd1306_update_display();
}
