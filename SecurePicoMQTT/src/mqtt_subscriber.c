#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/display_oled.h"
#include "include/tratar_gpio.h"
#include "include/mqtt_subscriber.h"

void iniciar_subscriber(const char *ssid, const char *senha, const char *broker_ip, 
    const char *usuario, const char *senha_mqtt, const char *topico) {
    modo_atual = MODO_SUBSCRIBER;
    display_oled_mostrar_modo("SUBSCRIBER");
    printf("Modo selecionado: SUBSCRIBER\n");

    printf("Conectando ao Wi-Fi...\n");
    connect_to_wifi(ssid, senha);

    printf("Iniciando setup MQTT...\n");
    mqtt_setup_result_t result = mqtt_setup("pico_client_sub", broker_ip, usuario, senha_mqtt);

    if (result != MQTT_OK) {
        switch (result) {
            case MQTT_ERR_INVALID_IP:
                display_oled_exibir_mensagem("MQTT ERRO", "IP inválido", "", MODO_NONE);
                break;
            case MQTT_ERR_CLIENT_INIT:
                display_oled_exibir_mensagem("MQTT ERRO", "Falha no client", "", MODO_NONE);
                break;
            case MQTT_ERR_CONNECT_FAIL:
                display_oled_exibir_mensagem("MQTT ERRO", "Erro ao conectar", "", MODO_NONE);
                break;
            default:
                display_oled_exibir_mensagem("MQTT ERRO", "Erro desconhecido", "", MODO_NONE);
                break;
        }
        sleep_ms(3000);
        display_oled_inicializar();
        return; // Retorna para evitar execução adicional
    }

    printf("Aguardando conexão com o broker MQTT...\n");
    const uint32_t timeout_ms = 10000;
    uint32_t elapsed = 0;
    while (!is_mqtt_connected() && elapsed < timeout_ms) {
        printf("Tentando conectar ao MQTT...\n");
        sleep_ms(500);
        elapsed += 500;
    }

    if (!is_mqtt_connected()) {
        display_oled_exibir_mensagem("MQTT ERRO", "Timeout", "", MODO_NONE);
        sleep_ms(2000);
        display_oled_inicializar();
        return; // Retorna para evitar execução adicional
    }

    printf("Conectado ao broker MQTT como SUBSCRIBER.\n");
    mqtt_register_callback(on_message_cb);
    mqtt_subscribe_to_topic(topico);

    while (true) {
        sleep_ms(100); // Substitui piscar_led para evitar bloqueio
    }
}