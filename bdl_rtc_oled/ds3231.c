#include "ds3231.h"

static uint8_t bcd_to_dec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}

static uint8_t dec_to_bcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}

void ds3231_set_datetime(i2c_inst_t *i2c, rtc_datetime_t *dt) {
    uint8_t buf[8];
    buf[0] = 0x00;
    buf[1] = dec_to_bcd(dt->second);
    buf[2] = dec_to_bcd(dt->minute);
    buf[3] = dec_to_bcd(dt->hour);
    buf[4] = dec_to_bcd(dt->day);
    buf[5] = dec_to_bcd(dt->date);
    buf[6] = dec_to_bcd(dt->month);
    buf[7] = dec_to_bcd(dt->year - 2000);
    
    printf("ds3231_set_datetime: Convertendo para BCD...\n");
    printf("  second: %02d -> 0x%02x\n", dt->second, buf[1]);
    printf("  minute: %02d -> 0x%02x\n", dt->minute, buf[2]);
    printf("  hour:   %02d -> 0x%02x\n", dt->hour,   buf[3]);
    printf("  day:    %02d -> 0x%02x\n", dt->day,    buf[4]);
    printf("  date:   %02d -> 0x%02x\n", dt->date,   buf[5]);
    printf("  month:  %02d -> 0x%02x\n", dt->month,  buf[6]);
    printf("  year:   %04d -> 0x%02x\n", dt->year,   buf[7]);
    
    int result = i2c_write_blocking(i2c, DS3231_ADDRESS, buf, 8, false);
    printf("ds3231_set_datetime: i2c_write_blocking retornou %d (8 bytes)\n", result);
}

void ds3231_get_datetime(i2c_inst_t *i2c, rtc_datetime_t *dt) {
    uint8_t reg = 0x00;
    uint8_t buf[7];

    printf("ds3231_get_datetime: Solicitando 7 bytes do RTC...\n");
    int write_result = i2c_write_blocking(i2c, DS3231_ADDRESS, &reg, 1, true);
    int read_result = i2c_read_blocking(i2c, DS3231_ADDRESS, buf, 7, false);

    printf("ds3231_get_datetime: i2c_write_blocking retornou %d (1 byte)\n", write_result);
    printf("ds3231_get_datetime: i2c_read_blocking retornou %d (7 bytes)\n", read_result);
    
    if (read_result == 7) {
        printf("ds3231_get_datetime: Dados brutos recebidos (BCD):\n");
        for (int i = 0; i < 7; i++) {
            printf("  buf[%d]: 0x%02x\n", i, buf[i]);
        }
        
        dt->second = bcd_to_dec(buf[0] & 0x7F);
        dt->minute = bcd_to_dec(buf[1]);
        dt->hour   = bcd_to_dec(buf[2] & 0x3F);
        dt->day    = bcd_to_dec(buf[3]);
        dt->date   = bcd_to_dec(buf[4]);
        dt->month  = bcd_to_dec(buf[5] & 0x1F);
        dt->year   = 2000 + bcd_to_dec(buf[6]);
        
        printf("ds3231_get_datetime: Valores convertidos:\n");
        printf("  Data: %02d/%02d/%04d\n", dt->date, dt->month, dt->year);
        printf("  Hora: %02d:%02d:%02d\n", dt->hour, dt->minute, dt->second);
    } else {
        printf("ds3231_get_datetime: Erro na leitura do I2C. Nao foi possivel ler os 7 bytes.\n");
    }
}