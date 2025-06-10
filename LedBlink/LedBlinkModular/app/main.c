#include "pico/stdlib.h"
#include "hal_led.h"

int main(void) {
    // Inicializa entrada/saída padrão
    stdio_init_all();

    // Inicializa o LED
    if (!hal_led_init()) {
        // Loop infinito em caso de falha
        while (true) {
            // Pode ser estendido para indicar erro
        }
    }

    // Loop principal: pisca o LED a cada 500 ms
    while (true) {
        hal_led_toggle();
        sleep_ms(500);
    }

    return 0;
}