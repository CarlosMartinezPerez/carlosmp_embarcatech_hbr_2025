#include "unity.h"
#include "temp_sensor.h"
#include <stdio.h>

void setUp(void) {
    // Setup code, if needed
}

void tearDown(void) {
    // Cleanup code, if needed
}

void test_adc_to_celsius_known_value(void) {
    // Test case: 0.706 V should give 27°C
    // Voltage = adc_val * 3.3 / 4095
    // 0.706 = adc_val * 3.3 / 4095
    // adc_val = 0.706 * 4095 / 3.3 ≈ 876
    uint16_t adc_val = 876;
    float expected = 27.0f;
    float result = adc_to_celsius(adc_val);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, expected, result);
}

void test_adc_to_celsius_zero_adc(void) {
    // Test case: ADC = 0
    uint16_t adc_val = 0;
    float voltage = 0.0f;
    float expected = 27.0f - (voltage - 0.706f) / 0.001721f; // ~436.37°C
    float result = adc_to_celsius(adc_val);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, expected, result);
}

void test_adc_to_celsius_max_adc(void) {
    // Test case: ADC = 4095 (max)
    uint16_t adc_val = 4095;
    float voltage = 3.3f;
    float expected = 27.0f - (voltage - 0.706f) / 0.001721f; // ~-1498.66°C
    float result = adc_to_celsius(adc_val);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, expected, result);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000); // Wait for USB serial to connect

    UNITY_BEGIN();
    printf("Running tests...\n");

    RUN_TEST(test_adc_to_celsius_known_value);
    RUN_TEST(test_adc_to_celsius_zero_adc);
    RUN_TEST(test_adc_to_celsius_max_adc);

    UNITY_END();
    printf("Tests finished.\n");

    while (true) {
        sleep_ms(1000);
    }

    return 0;
}