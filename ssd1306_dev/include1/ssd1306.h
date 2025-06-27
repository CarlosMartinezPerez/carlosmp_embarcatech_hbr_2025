#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h" // Inclui o header do Pico SDK para I2C

// Definições do display
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64
#define SSD1306_I2C_ADDR    0x3C

// Cores para desenhar
typedef enum {
    BLACK = 0,
    WHITE = 1
} SSD1306_COLOR;

// Funções públicas
void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_draw_pixel(int16_t x, int16_t y, SSD1306_COLOR color);
void ssd1306_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, SSD1306_COLOR color);
void ssd1306_draw_char(int16_t x, int16_t y, char character, SSD1306_COLOR color);
void ssd1306_draw_string(int16_t x, int16_t y, const char *string, SSD1306_COLOR color);
void ssd1306_update_display(void); // Envia o framebuffer para o display

#endif // SSD1306_H