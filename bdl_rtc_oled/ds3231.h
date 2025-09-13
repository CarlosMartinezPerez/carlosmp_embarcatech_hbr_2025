#ifndef DS3231_H
#define DS3231_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h> // Incluir para printf

#define DS3231_ADDRESS 0x68

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} rtc_datetime_t;

void ds3231_set_datetime(i2c_inst_t *i2c, rtc_datetime_t *dt);
void ds3231_get_datetime(i2c_inst_t *i2c, rtc_datetime_t *dt);

#endif