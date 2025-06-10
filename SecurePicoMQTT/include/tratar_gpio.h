//tratar_gpio.h
#ifndef TRATAR_GPIO_H
#define TRATAR_GPIO_H

#include "pico/stdlib.h"  // para o tipo uint, etc.

// Definições dos pinos — coloque aqui para ficar disponível em main.c e outros arquivos
#define BOTAO_A 5
#define BOTAO_B 6
#define LED_VM 13
#define LED_VD 11

// Protótipos das funções
void piscar_led(uint gpio_led);
void inicializar_gpio(void);

#endif
