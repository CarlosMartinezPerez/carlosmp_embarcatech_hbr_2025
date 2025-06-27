#include <stdbool.h> // Inclui definições de bool, true, false

#ifndef LED_H
#define LED_H

void led_init(void);
void led_set_red(bool state);
void led_set_green(bool state);
void led_off(void);

#endif