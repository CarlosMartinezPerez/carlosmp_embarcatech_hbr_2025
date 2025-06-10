#ifndef LED_EMBUTIDO_H
#define LED_EMBUTIDO_H

#include <stdbool.h>

// Inicializa o LED embutido
bool led_embutido_init(void);

// Define o estado do LED (true = ligado, false = desligado)
void led_embutido_set(bool state);

#endif // LED_EMBUTIDO_H