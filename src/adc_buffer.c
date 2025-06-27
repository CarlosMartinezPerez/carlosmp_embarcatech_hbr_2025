#include <stdio.h>
#include "adc_buffer.h"

static uint16_t buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

void adc_buffer_init(void) {
    buffer_index = 0;
}

bool adc_buffer_add_sample(uint16_t sample) {
    if (buffer_index < BUFFER_SIZE) {
        buffer[buffer_index++] = sample;
        return true;
    }
    return false;
}

void adc_buffer_reset(void) {
    buffer_index = 0;
}

void adc_buffer_print(void) {
    for (size_t i = 0; i < buffer_index; ++i) {
        printf("Amostra %5zu: %u\n", i, buffer[i]);
    }
}

uint16_t adc_buffer_get_sample(size_t index) {
    if (index < buffer_index) {
        return buffer[index];
    }
    return 0;
}

size_t adc_buffer_get_count(void) {
    return buffer_index;
}