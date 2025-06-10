#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "string.h"

int main() {
    stdio_init_all();      // Inicializa a saída padrão (opcional para debug)
    ssd1306_init();        // Inicializa o I2C1 e o display OLED

    ssd1306_clear();       // Limpa o framebuffer
    ssd1306_show();
    ssd1306_draw_string(0, 0, "!'\"#$%&()*+,-./0");  // Escreve no canto superior esquerdo
    ssd1306_draw_string(0, 16, "123456789ABCDEFG");  // Escreve no canto superior esquerdo
    ssd1306_draw_string(0, 32, "HIJKLMNOPQRSTUVW");  // Escreve no canto superior esquerdo
    ssd1306_draw_string(0, 48, "XYZ");  // Escreve no canto superior esquerdo
    ssd1306_draw_line(3, 61, 114, 1, true);
    ssd1306_draw_line(0, 0, 127, 63, true);
    ssd1306_draw_pixel(64, 62, true);
    ssd1306_draw_pixel(65, 62, true);
    ssd1306_draw_pixel(66, 62, true);
    ssd1306_draw_pixel(67, 62, true);
    ssd1306_draw_pixel(68, 62, true);
    ssd1306_show(); // Atualiza o display
    sleep_ms(3000);
    ssd1306_walk_horizontal_pixel();
    

    while (true) {
        tight_loop_contents();  // Loop infinito vazio
    }

    return 0;
}
