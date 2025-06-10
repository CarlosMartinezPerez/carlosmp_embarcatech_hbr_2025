#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "include/display_oled.h"
#include "include/tratar_gpio.h"
#include "include/mqtt_publisher.h"

#define XOR_KEY 42

void iniciar_publisher(const char *ssid, const char *senha, const char *broker_ip, 
                       const char *usuario, const char *senha_mqtt, 
                       const char *topico, const char *client_id) {
    modo_atual = MODO_PUBLISHER;
    display_oled_mostrar_modo("PUBLISHER");
    printf("Modo selecionado: PUBLISHER\n");

    printf("Conectando ao Wi-Fi...\n");
    connect_to_wifi(ssid, senha);

    printf("Iniciando setup MQTT...\n");
    mqtt_setup_result_t result = mqtt_setup(client_id, broker_ip, usuario, senha_mqtt);

    if (result != MQTT_OK) {
        // ... tratamento de erro igual ao original ...
        sleep_ms(3000);
        display_oled_inicializar();
        return;
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
        return;
    }

    printf("Conectado ao broker MQTT como PUBLISHER.\n");

    char ultima_msg[64] = "";
    char penultima_msg[64] = "";

    while (true) {
        if (!gpio_get(BOTAO_B)) {  // Botão B: enviar mensagem nova
            sleep_ms(300);

            float temp_simulada = 25.0f + (rand() % 50) / 10.0f;
            unsigned long timestamp = to_ms_since_boot(get_absolute_time());

            char mensagem_clara[64];
            snprintf(mensagem_clara, sizeof(mensagem_clara), "T=%.1f %lu", temp_simulada, timestamp);
            printf("Tópico: %s\n", topico);
            printf("Mensagem clara: %s\n", mensagem_clara);
            
            char mensagem_hex[128] = {0};  // buffer para hex criptografado

            // Criptografa + converte para hex
            xor_encrypt_to_hex((const uint8_t *)mensagem_clara, mensagem_hex, strlen(mensagem_clara), XOR_KEY);

            printf("Enviando nova mensagem (hex): %s\n", mensagem_hex);

            if (is_mqtt_connected()) {
                mqtt_comm_publish(topico, (const uint8_t *)mensagem_hex, strlen(mensagem_hex), false); // true durante desenvolvimento e testes
                printf("Mensagem publicada.\n");

                strcpy(penultima_msg, ultima_msg);
                strcpy(ultima_msg, mensagem_clara);
                display_oled_exibir_mensagem(ultima_msg, penultima_msg, topico, modo_atual);
                piscar_led(LED_VM);
            } else {
                printf("MQTT desconectado. Ignorando envio.\n");
                display_oled_exibir_mensagem("MQTT OFF", "", "", MODO_NONE);
            }
        }
        else if (!gpio_get(BOTAO_A)) {  // Botão A: enviar mensagem repetida (se houver)
            sleep_ms(300);
            if (strlen(ultima_msg) > 0) {
                printf("Tópico: %s\n", topico);
                printf("Mensagem clara repetida: %s\n", ultima_msg);
                char mensagem_hex[128] = {0};

                // Recriptografa a última mensagem para hex
                xor_encrypt_to_hex((const uint8_t *)ultima_msg, mensagem_hex, strlen(ultima_msg), XOR_KEY);

                printf("Enviando mensagem repetida (hex): %s\n", mensagem_hex);

                if (is_mqtt_connected()) {
                    mqtt_comm_publish(topico, (const uint8_t *)mensagem_hex, strlen(mensagem_hex), false); // true durante desenvolvimento e testes
                    printf("Mensagem repetida publicada.\n");

                    piscar_led(LED_VM);
                } else {
                    printf("MQTT desconectado. Ignorando envio.\n");
                    display_oled_exibir_mensagem("MQTT OFF", "", "", MODO_NONE);
                }
            } else {
                printf("Nenhuma mensagem anterior para repetir.\n");
            }
        }
    }
}