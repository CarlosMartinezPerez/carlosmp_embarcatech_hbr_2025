#ifndef ADC_READER_H
#define ADC_READER_H

#include <stdint.h>

void adc_reader_init(void);
uint16_t adc_read_microphone(void);

#endif
