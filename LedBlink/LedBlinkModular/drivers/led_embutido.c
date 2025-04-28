#include "pico/cyw43_arch.h"
#include "led_embutido.h"

// Inicializa o chip Wi-Fi para controle do LED embutido
bool led_embutido_init(void) {
    return cyw43_arch_init() == 0;
}

// Define o estado do LED (ligado/desligado)
void led_embutido_set(bool state) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
}