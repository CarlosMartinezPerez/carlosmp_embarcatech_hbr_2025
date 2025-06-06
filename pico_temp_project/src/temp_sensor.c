#include "temp_sensor.h"

float adc_to_celsius(uint16_t adc_val) {
    float voltage = adc_val * 3.3f / 4095.0f;
    float temp = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temp;
}