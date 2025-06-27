#include "led.h"
#include "pico/stdlib.h"

#define LED_RED_GPIO 13   // Vermelho
#define LED_GREEN_GPIO 11 // Verde

void led_init(void) {
    // Configura GPIOs como saída
    gpio_init(LED_RED_GPIO);
    gpio_set_dir(LED_RED_GPIO, GPIO_OUT);
    gpio_put(LED_RED_GPIO, 0); // Desligado

    gpio_init(LED_GREEN_GPIO);
    gpio_set_dir(LED_GREEN_GPIO, GPIO_OUT);
    gpio_put(LED_GREEN_GPIO, 0); // Desligado
}

void led_set_red(bool state) {
    gpio_put(LED_RED_GPIO, state);
}

void led_set_green(bool state) {
    gpio_put(LED_GREEN_GPIO, state);
}

void led_off(void) {
    led_set_red(false);
    led_set_green(false);
}