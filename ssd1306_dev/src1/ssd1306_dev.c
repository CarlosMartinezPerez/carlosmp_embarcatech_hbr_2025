#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h" // Inclui o seu arquivo ssd1306.h

// Definições de pinos I2C (ajuste se necessário)
#define PICO_I2C_SDA_PIN 4
#define PICO_I2C_SCL_PIN 5

int main() {
    stdio_init_all(); // Inicializa o stdio (para printf)

    // Inicializa o I2C0 no Pico (ajuste se estiver usando I2C1)
    i2c_init(i2c_default, 400 * 1000); // 400 kHz
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_I2C_SDA_PIN);
    gpio_pull_up(PICO_I2C_SCL_PIN);

    printf("Iniciando teste do SSD1306...\n");

    // Inicializa o display SSD1306
    ssd1306_init();
    printf("Display SSD1306 inicializado.\n");

    ssd1306_clear(); // Limpa o display
    ssd1306_update_display(); // Envia o buffer limpo para o display
    sleep_ms(1000);

    // Teste 1: Desenhar pixels
    printf("Desenhando alguns pixels...\n");
    for (int i = 0; i < 10; i++) {
        ssd1306_draw_pixel(i, i, WHITE);
        ssd1306_draw_pixel(127 - i, i, WHITE);
        ssd1306_draw_pixel(i, 63 - i, WHITE);
        ssd1306_draw_pixel(127 - i, 63 - i, WHITE);
    }
    ssd1306_update_display();
    sleep_ms(2000);

    ssd1306_clear();
    printf("Desenhando algumas linhas...\n");
    // Teste 2: Desenhar linhas
    ssd1306_draw_line(0, 0, 127, 63, WHITE); // Diagonal principal
    ssd1306_draw_line(0, 63, 127, 0, WHITE); // Diagonal secundária
    ssd1306_draw_line(64, 0, 64, 63, WHITE); // Linha vertical no meio
    ssd1306_draw_line(0, 32, 127, 32, WHITE); // Linha horizontal no meio
    ssd1306_update_display();
    sleep_ms(3000);

    ssd1306_clear();
    printf("Exibindo texto...\n");
    // Teste 3: Exibir caracteres e strings
    ssd1306_draw_string(0, 0, "Hello, Pico!", WHITE);
    ssd1306_draw_string(0, 16, "SSD1306 Test", WHITE);
    ssd1306_draw_string(0, 32, "Line 3", BLACK); // Texto preto sobre fundo branco (se o fundo for branco)
    ssd1306_draw_string(0, 48, "Line 4 with more text", WHITE);
    ssd1306_update_display();
    sleep_ms(4000);

    ssd1306_clear();
    printf("Contagem regressiva...\n");
    // Teste 4: Contagem regressiva simples
    for (int i = 5; i >= 0; i--) {
        char buffer[10];
        sprintf(buffer, "Contagem: %d", i);
        ssd1306_clear();
        ssd1306_draw_string(0, 0, buffer, WHITE);
        ssd1306_update_display();
        sleep_ms(1000);
    }

    ssd1306_clear();
    ssd1306_draw_string(20, 24, "FIM DO TESTE", WHITE);
    ssd1306_update_display();
    printf("Teste concluido.\n");

    while (1) {
        // Loop infinito para manter o programa em execução e o display ligado
        sleep_ms(1000);
    }

    return 0;
}