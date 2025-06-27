#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <stdint.h>

void pwm_output_init(void);
void pwm_output_set_duty_from_adc(uint16_t adc_value);
void pwm_output_stop(void);
void pwm_output_play_buffer(void);

#endif
