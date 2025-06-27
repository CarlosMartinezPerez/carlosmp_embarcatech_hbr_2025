#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define BUFFER_SIZE 40000 // 2 s × 20 kHz

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
void adc_buffer_reset(void);
void adc_buffer_print(void);
uint16_t adc_buffer_get_sample(size_t index);
size_t adc_buffer_get_count(void); // Nova função para contar amostras

#endif