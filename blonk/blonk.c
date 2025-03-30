#include <stdio.h>
#include "pico/stdlib.h"

// Subrotina para esperar o pressionamento do botão B e realizar debounce
void wait_button_press(uint button_pin) {
    while (gpio_get(button_pin) == 1) { // Aguarda o botão ser pressionado (nível baixo)
        sleep_ms(10); // Pequeno atraso para não consumir muitos recursos
    }
    sleep_ms(200); // Debounce
    while (gpio_get(button_pin) == 0) { // Aguarda o botão ser solto
        sleep_ms(10);
    }

}

int main() {
    // Inicializa a E/S padrão
    stdio_init_all();

    // Configuração do botão B como entrada com pull-up
    const uint BUTTON_B_PIN = 5;
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração dos pinos do LED RGB como saída
    const uint BLUE_LED_PIN = 12;
    const uint RED_LED_PIN = 13;
    const uint GREEN_LED_PIN = 11;
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Variável para controlar o estado do LED
    int led_state = -1; // 0: vermelho, 1: verde, 2: azul

    while (true) {
        // Chama a subrotina para esperar o pressionamento do botão
        wait_button_press(BUTTON_B_PIN);

        // Avança para o próximo estado do LED
        led_state = (led_state + 1) % 8;
        
        // Atualiza o LED RGB com a cor correspondente
        switch (led_state) {
            case 0: // Vermelho
                gpio_put(RED_LED_PIN, 1);
                gpio_put(GREEN_LED_PIN, 0);
                gpio_put(BLUE_LED_PIN, 0);
                break;
            case 1: // Verde
                gpio_put(RED_LED_PIN, 0);
                gpio_put(GREEN_LED_PIN, 1);
                gpio_put(BLUE_LED_PIN, 0);
                break;
            case 2: // Azul
                gpio_put(RED_LED_PIN, 0);
                gpio_put(GREEN_LED_PIN, 0);
                gpio_put(BLUE_LED_PIN, 1);
                break;
                case 3: // Ciano
                gpio_put(RED_LED_PIN, 0);
                gpio_put(GREEN_LED_PIN, 1);
                gpio_put(BLUE_LED_PIN, 1);
                break;
                case 4: // Magenta
                gpio_put(RED_LED_PIN, 1);
                gpio_put(GREEN_LED_PIN, 0);
                gpio_put(BLUE_LED_PIN, 1);
                break;
                case 5: // Amarelo
                gpio_put(RED_LED_PIN, 1);
                gpio_put(GREEN_LED_PIN, 1);
                gpio_put(BLUE_LED_PIN, 0);
                break;
                case 6: // Branco
                gpio_put(RED_LED_PIN, 1);
                gpio_put(GREEN_LED_PIN, 1);
                gpio_put(BLUE_LED_PIN, 1);
                break;
                case 7: // Apagado
                gpio_put(RED_LED_PIN, 0);
                gpio_put(GREEN_LED_PIN, 0);
                gpio_put(BLUE_LED_PIN, 0);
                break;
        }
    }

    return 0;
}