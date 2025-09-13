#include <stdio.h>
#include "pico/stdlib.h"
#include "include2/ssd1306.h"
#include "string.h"

int main() {
    stdio_init_all();      // Inicializa a saída padrão (opcional para debug)
    ssd1306_init();        // Inicializa o I2C1 e o display OLED

    ssd1306_clear();       // Limpa o framebuffer
    ssd1306_show();
    ssd1306_draw_string(0, 0, " !\"#$%&'()*+,-.");
    ssd1306_draw_string(0, 8, "/:;<=>?@[]^_`\\");
    ssd1306_draw_string(0, 16, "0123456789ABCDEF");
    ssd1306_draw_string(0, 24, "GHIJKLMNOPQRSTUV");
    ssd1306_draw_string(0, 32, "WXYZabcdefghijkl");
    ssd1306_draw_string(0, 40, "mnopqrstuvwxyz");
    ssd1306_draw_string(0, 48, "{|} ~ ");
    //ssd1306_draw_line(3, 61, 114, 1, true);
    //ssd1306_draw_line(0, 0, 127, 63, true);
    //ssd1306_draw_pixel(64, 62, true);
    //ssd1306_draw_pixel(65, 62, true);
    //ssd1306_draw_pixel(66, 62, true);
    //ssd1306_draw_pixel(67, 62, true);
    //ssd1306_draw_pixel(68, 62, true);
    ssd1306_show(); // Atualiza o display
    //sleep_ms(3000);
    //ssd1306_walk_horizontal_pixel();
    bool button_pressed = false;gpio_init(6);
    gpio_set_dir(6, GPIO_IN);
    gpio_pull_up(6);
    ssd1306_draw_string(0, 56, " BOTAO B - SAIR ");
    ssd1306_show();

    while (true) {
        if (!gpio_get(6)) {
            sleep_ms(50);
            button_pressed = true;
        } else {
            if (button_pressed) {
                ssd1306_clear();
                ssd1306_show();
                break;
            }
        }
    }
    return 0;
}
