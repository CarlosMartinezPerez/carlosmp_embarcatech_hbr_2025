// display_oled.h
#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include "oled/ssd1306.h"
#include "oled/ssd1306_i2c.h"
#include "mqtt_comm.h"
#include <stdint.h>

void display_oled_inicializar_i2c(void);
void display_oled_limpar(void);
void display_oled_inicializar(void);
void display_oled_mostrar_modo(const char* modo);
void display_oled_exibir_mensagem(const char* msg1, const char* msg2, const char* topic, modo_t modo);

#endif // DISPLAY_OLED_H
