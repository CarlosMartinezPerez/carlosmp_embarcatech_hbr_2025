#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "include/tratar_gpio.h"  // já com as definições

void piscar_led(uint gpio_led) {
    gpio_put(gpio_led, 1);
    sleep_ms(50);
    gpio_put(gpio_led, 0);
    sleep_ms(950);
}

void inicializar_gpio() {
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    gpio_init(LED_VM);
    gpio_set_dir(LED_VM, GPIO_OUT);
    gpio_put(LED_VM, 0);

    gpio_init(LED_VD);
    gpio_set_dir(LED_VD, GPIO_OUT);
    gpio_put(LED_VD, 0);
}
