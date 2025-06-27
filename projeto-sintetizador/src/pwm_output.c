#include "pwm_output.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "adc_buffer.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

// Configurações do PWM
#define BUZZER_GPIO 10              // GPIO conectado ao buzzer
#define PWM_FREQ 20000              // Frequência do PWM: 20 kHz
#define PWM_WRAP 4096               // Resolução: 12 bits (0–4095)

static uint pwm_slice;

void pwm_output_init(void) {
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(BUZZER_GPIO);

    // Calcula divisor de clock para obter a frequência desejada
    float clkdiv = (float)125000000 / (PWM_FREQ * PWM_WRAP);  // 125 MHz / (freq × wrap)
    pwm_set_clkdiv(pwm_slice, clkdiv);

    // Define a resolução do PWM (wrap value)
    pwm_set_wrap(pwm_slice, PWM_WRAP - 1);

    // Inicializa com duty cycle zero
    pwm_set_gpio_level(BUZZER_GPIO, 0);

    // Ativa o PWM
    pwm_set_enabled(pwm_slice, true);
}

void pwm_output_set_duty_from_adc(uint16_t adc_value) {
    if (adc_value > PWM_WRAP - 1) {
        adc_value = PWM_WRAP - 1;  // Garante que valor não exceda a resolução máxima
    }
    pwm_set_gpio_level(BUZZER_GPIO, adc_value);
}

void pwm_output_stop(void) {
    pwm_set_gpio_level(BUZZER_GPIO, 0);    // Duty cycle 0%
}

void pwm_output_play_buffer(void) {
    size_t count = adc_buffer_get_count();
    for (size_t i = 0; i < count; ++i) {
        uint16_t sample = adc_buffer_get_sample(i);
        pwm_output_set_duty_from_adc(sample);
        sleep_us(SAMPLE_INTERVAL_US);
    }
}