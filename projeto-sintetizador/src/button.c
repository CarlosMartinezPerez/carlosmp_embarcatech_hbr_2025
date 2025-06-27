#include "button.h"
#include "pico/stdlib.h"

#define BUTTON_A_GPIO 5 // Botão A na GPIO 5
#define BUTTON_B_GPIO 6 // Botão B na GPIO 6

static bool last_a_state = true; // Estado inicial: não pressionado (pull-up)
static bool last_b_state = true;

void button_init(void) {
    // Configura GPIOs como entrada com pull-up
    gpio_init(BUTTON_A_GPIO);
    gpio_set_dir(BUTTON_A_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_A_GPIO);

    gpio_init(BUTTON_B_GPIO);
    gpio_set_dir(BUTTON_B_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_B_GPIO);
}

bool button_a_pressed(void) {
    bool current_state = gpio_get(BUTTON_A_GPIO); // 0 = pressionado, 1 = solto
    bool pressed = last_a_state && !current_state; // Borda de descida
    last_a_state = current_state;
    return pressed;
}

bool button_b_pressed(void) {
    bool current_state = gpio_get(BUTTON_B_GPIO); // 0 = pressionado, 1 = solto
    bool pressed = last_b_state && !current_state; // Borda de descida
    last_b_state = current_state;
    return pressed;
}