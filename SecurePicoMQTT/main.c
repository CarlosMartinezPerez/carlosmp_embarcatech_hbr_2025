#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "oled/ssd1306.h"
#include "oled/ssd1306_i2c.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "include/display_oled.h"
#include "include/tratar_gpio.h"
#include "include/mqtt_publisher.h"
#include "include/mqtt_subscriber.h"
#include <string.h>

// Configurações para conexão Wi-Fi, MQTT e criptografia. 
// Ajuste os valores abaixo conforme suas próprias configurações de rede, credenciais e 
// ambiente de desenvolvimento e testes/produção.
#define WIFI_SSID "WIFI SSID"
#define WIFI_PASS "WIFI PASSWORD"
#define MQTT_BROKER_IP "192.168.15.101"
#define MQTT_USER "aluno"
#define MQTT_PASS "senha123"
#define XOR_KEY 42

#define CLIENT_ID "pico_pub_sala47" // Diferente para cada placa trabalhando como publisher
#define MQTT_PUB_TOPIC "escola/sala47/temperatura"
#define MQTT_SUB_TOPIC "escola/#" // Produção
// #define MQTT_SUB_TOPIC "escola/sala 47/temperatura" // Desenvolvimento e testes

int main() {
    stdio_init_all();
    inicializar_gpio();
    display_oled_inicializar_i2c();
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    ssd1306_init();
    display_oled_limpar();
    sleep_ms(100);
    display_oled_inicializar();

    while (true) {
        if (!gpio_get(BOTAO_A)) { // Seleciona modo Publisher
            sleep_ms(300);
            iniciar_publisher(WIFI_SSID, WIFI_PASS, MQTT_BROKER_IP, 
                MQTT_USER, MQTT_PASS, MQTT_PUB_TOPIC, CLIENT_ID);
        }
        else if (!gpio_get(BOTAO_B)) { // Seleciona modo Subscriber
            sleep_ms(300);
            iniciar_subscriber(WIFI_SSID, WIFI_PASS, MQTT_BROKER_IP, 
                MQTT_USER, MQTT_PASS, MQTT_SUB_TOPIC);
        }
    }
}