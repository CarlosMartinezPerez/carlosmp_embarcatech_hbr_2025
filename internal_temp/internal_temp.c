#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/adc.h>

int main() {
    // Inicializa os periféricos padrão
    stdio_init_all();
    sleep_ms(3000); // Tempo de abrir o monitor serial
    
    // Inicializa o ADC
    adc_init();
    
    // Habilita o sensor de temperatura interno
    adc_set_temp_sensor_enabled(true);
    
    // Seleciona o canal 4 (sensor de temperatura)
    adc_select_input(4);

    while (true) {
        // Faz a leitura do ADC (12 bits, retorna valores de 0 a 4095)
        uint16_t raw = adc_read();
        
        // Converte a leitura bruta em voltagem
        // Referência de 3.3V, 12 bits (4096 níveis)
        const float conversion_factor = 3.3f / (1 << 12);
        float voltage = raw * conversion_factor;
        
        // Converte a voltagem em temperatura usando a fórmula do datasheet
        // Para o sensor de temperatura do RP2040:
        // Temp (°C) = 27 - (ADC_Voltage - 0.706)/0.001721 - RP2040 datasheet
        float temperature = 27.0f - (voltage - 0.706f) / 0.001721f; // Segundo Seção 4.9.5, "Temperature Sensor"
        
        // Imprime o resultado
        printf("Temperatura: %.2f °C\n", temperature);
        
        // Aguarda 1 segundo antes da próxima leitura
        sleep_ms(1000);
    }

    return 0;
}