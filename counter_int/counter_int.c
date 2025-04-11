#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "ssd1306.h"      // Funções de alto nível para SSD1306 ..

// Definições de pinos e parâmetros
#define BUTTON_A 5         // GPIO5 para Botão A
#define BUTTON_B 6         // GPIO6 para Botão B
#define I2C_PORT i2c1      // I2C1 para o display
#define I2C_SDA 14         // GPIO14 para SDA
#define I2C_SCL 15         // GPIO15 para SCL
#define I2C_FREQ 400000    // 400kHz
#define SSD1306_ADDR 0x3C  // Endereço I2C do SSD1306
#define DEBOUNCE_DELAY_US 200000  // 200ms de debounce

// Buffer para o display (128x64 pixels, 1 bit por pixel = 8192 bits ou1024 bytes)
uint8_t display_buffer[1024];

// Variáveis globais
volatile uint8_t counter = 0;        // Contador decrescente
volatile uint16_t button_b_count = 0;// Contagem de pressionamentos do Botão B
volatile bool counting = false;      // Estado da contagem
uint64_t last_time = 0;              // Controle de tempo para contagem
volatile uint64_t last_a_press = 0;  // Último pressionamento do Botão A
volatile uint64_t last_b_press = 0;  // Último pressionamento do Botão B

// Função para limpar o buffer
void clear_display_buffer() {
    memset(display_buffer, 0, sizeof(display_buffer));
}

// Função para enviar o buffer ao SSD1306
void ssd1306_update_display() {
    uint8_t command_buffer[2];
    
    command_buffer[0] = 0x00;  // Control byte (Co=0, D/C=0 para comando)
    command_buffer[1] = 0x21;  // Set column address
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    command_buffer[1] = 0x00;  // Coluna inicial
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    command_buffer[1] = 0x7F;  // Coluna final (127)
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    
    command_buffer[1] = 0x22;  // Set page address
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    command_buffer[1] = 0x00;  // Página inicial
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    command_buffer[1] = 0x07;  // Página final (7 para 64 linhas)
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, command_buffer, 2, false);
    
    uint8_t data_buffer[1025];
    data_buffer[0] = 0x40;  // Control byte (Co=0, D/C=1 para dados)
    memcpy(&data_buffer[1], display_buffer, 1024);
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, data_buffer, 1025, false);
}

// Função para atualizar o display
void update_display() {
    char buffer[32];
    clear_display_buffer();
    
    sprintf(buffer, "COUNTER: %d", counter);
    ssd1306_draw_string(display_buffer, 0, 0, buffer);  // Linha superior
    
    sprintf(buffer, "BUTTON B: %d", button_b_count);
    ssd1306_draw_string(display_buffer, 0, 20, buffer); // Linha inferior
    
    ssd1306_update_display();
}

// Função de callback para interrupções
void gpio_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();

    if (gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL)) {
        if ((current_time - last_a_press) >= DEBOUNCE_DELAY_US) {
            counter = 9;                    // Reinicia contador
            button_b_count = 0;             // Zera contagem do Botão B
            counting = true;                // Ativa contagem
            last_a_press = current_time;
            update_display();
        }
    } 
    else if (gpio == BUTTON_B && counting && (events & GPIO_IRQ_EDGE_FALL)) {
        if ((current_time - last_b_press) >= DEBOUNCE_DELAY_US) {
            button_b_count++;
            last_b_press = current_time;
            update_display();
        }
    }
}

// Inicialização básica do SSD1306
void ssd1306_setup() {
    uint8_t init_commands[] = {
        0x00, 0xAE,        // Display OFF
        0x00, 0xD5, 0x80,  // Set display clock div
        0x00, 0xA8, 0x3F,  // Set multiplex ratio (64-1)
        0x00, 0xD3, 0x00,  // Set display offset
        0x00, 0x40,        // Set start line
        0x00, 0x8D, 0x14,  // Charge pump
        0x00, 0x20, 0x00,  // Memory mode: horizontal
        0x00, 0xA1,        // Segment remap
        0x00, 0xC8,        // COM output scan direction
        0x00, 0xDA, 0x12,  // COM pins
        0x00, 0x81, 0xCF,  // Set contrast
        0x00, 0xD9, 0xF1,  // Pre-charge period
        0x00, 0xDB, 0x40,  // VCOMH deselect level
        0x00, 0xA4,        // Entire display ON
        0x00, 0xA6,        // Normal display (not inverted)
        0x00, 0xAF         // Display ON
    };
    i2c_write_blocking(I2C_PORT, SSD1306_ADDR, init_commands, sizeof(init_commands), false);
}

int main() {
    // Inicialização básica
    stdio_init_all();
    
    // Configuração do I2C para o display
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicialização do SSD1306
    ssd1306_setup();
    clear_display_buffer();
    ssd1306_draw_string(display_buffer, 0, 0, "STARTED");
    ssd1306_draw_string(display_buffer, 0, 20, "PRESS A");
    ssd1306_update_display();
    
    // Configuração dos botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    // Configuração das interrupções
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Loop principal
    while (true) {
        if (counting) {
            uint64_t current_time = time_us_64();
            if ((current_time - last_time) >= 1000000) {
                if (counter > 0) {
                    counter--;
                    last_time = current_time;
                    update_display();
                }
                if (counter == 0) {
                    counting = false;
                    update_display();
                }
            }
        }
        
        sleep_ms(10);
    }
    
    return 0;
}