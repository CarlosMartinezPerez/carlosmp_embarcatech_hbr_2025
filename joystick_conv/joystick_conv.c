#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Definição dos pinos
#define JOYSTICK_VRX_PIN 27  // Pino para VRx (eixo X)
#define JOYSTICK_VRY_PIN 26  // Pino para VRy (eixo Y)
#define JOYSTICK_SW_PIN  22  // Pino para chave (SW)

// Definição dos canais ADC
#define ADC_VRX_CHANNEL 1    // ADC1 (GPIO27)
#define ADC_VRY_CHANNEL 0    // ADC0 (GPIO26)

int main() {
    // Inicializa a biblioteca padrão e o sistema
    stdio_init_all();
    
    // Inicializa o ADC
    adc_init();
    
    // Configura os pinos do ADC
    adc_gpio_init(JOYSTICK_VRX_PIN); // Inicializa GPIO27 para ADC
    adc_gpio_init(JOYSTICK_VRY_PIN); // Inicializa GPIO26 para ADC
    
    // Configura o pino da chave (SW) como entrada com pull-up
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN); // Habilita resistor de pull-up interno
    
    // Loop principal
    while (true) {
        // Seleciona o canal ADC para VRx (eixo X)
        adc_select_input(ADC_VRX_CHANNEL);
        uint16_t vrx_value = adc_read(); // Lê o valor (0 a 4095)
        
        // Seleciona o canal ADC para VRy (eixo Y)
        adc_select_input(ADC_VRY_CHANNEL);
        uint16_t vry_value = adc_read(); // Lê o valor (0 a 4095)
        
        // Lê o estado da chave (SW)
        bool sw_state = !gpio_get(JOYSTICK_SW_PIN); // Inverte pois pull-up (0 = pressionado)
        
        // Converte os valores ADC para uma escala aproximada (-512 a +511)
        int16_t x_axis = vrx_value - 2048; // Centraliza em 0
        int16_t y_axis = vry_value - 2048; // Centraliza em 0
        
        // Exibe os valores no monitor serial
        printf("Eixo X: %d, Eixo Y: %d, Chave: %d\n", x_axis, y_axis, sw_state);
        
        // Pequeno atraso para evitar sobrecarga no terminal
        sleep_ms(100);
    }
    
    return 0;
}