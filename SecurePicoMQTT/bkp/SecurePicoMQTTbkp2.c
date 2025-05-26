#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include <stdio.h>
#include <string.h>

#define WIFI_SSID       "VIVOFIBRA-8991_EXT"
#define WIFI_PASS       "cajuca1801"
#define MQTT_BROKER_IP  "192.168.15.101"
#define MQTT_USER       "CarlosMP"
#define MQTT_PASS       "cajuca1801"
#define MQTT_TOPIC      "test/topic"
#define XOR_KEY         42

int main() {
    stdio_init_all();
    sleep_ms(3000);  // Tempo para iniciar o terminal

    printf("Conectando ao Wi-Fi...\n");
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    printf("Conectado ao Wi-Fi.\n");

    printf("Conectando ao broker MQTT...\n");
    mqtt_setup("pico_pub", MQTT_BROKER_IP, MQTT_USER, MQTT_PASS);
    printf("Conectado ao broker MQTT.\n");

    const char* mensagem_clara = "26.5";
    char mensagem_cript[32];

    while (true) {
        xor_encrypt((const uint8_t*)mensagem_clara, (uint8_t*)mensagem_cript, strlen(mensagem_clara), XOR_KEY);
        mqtt_comm_publish(MQTT_TOPIC, (const uint8_t*)mensagem_cript, strlen(mensagem_clara));
        printf("Mensagem criptografada '%s' publicada com sucesso\n", mensagem_clara);

        sleep_ms(5000);  // Aguarda 5 segundos
    }

    return 0;
}
