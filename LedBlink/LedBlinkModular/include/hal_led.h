#ifndef HAL_LED_H
#define HAL_LED_H

#include <stdbool.h>

// Inicializa o LED
bool hal_led_init(void);

// Alterna o estado do LED
void hal_led_toggle(void);

#endif // HAL_LED_H