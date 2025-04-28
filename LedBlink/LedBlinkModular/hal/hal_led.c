#include "hal_led.h"
#include "led_embutido.h"

static bool led_state = false;

// Inicializa o LED através do driver
bool hal_led_init(void) {
    return led_embutido_init();
}

// Alterna o estado do LED
void hal_led_toggle(void) {
    led_state = !led_state;
    led_embutido_set(led_state);
}