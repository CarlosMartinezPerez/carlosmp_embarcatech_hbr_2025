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

void iniciar_publisher(const char *ssid, const char *senha, const char *broker_ip, const char *usuario, const char *senha_mqtt, const char *topico, const char *client_id) {
    modo_atual = MODO_PUBLISHER;
    display_oled_mostrar_modo("PUBLISHER");
    printf("Modo selecionado: PUBLISHER\n");

    printf("Conectando ao Wi-Fi...\n");
    connect_to_wifi(ssid, senha);

    printf("Iniciando setup MQTT...\n");
    mqtt_setup_result_t result = mqtt_setup(client_id, broker_ip, usuario, senha_mqtt);

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

    int contador = 0;
    char ultima_msg[64] = "";
    char penultima_msg[64] = "";

    while (true) {
        if (!gpio_get(BOTAO_B)) {  // Botão B: enviar mensagem nova
            sleep_ms(300);
            float temp_simulada = 25.0f + (rand() % 50) / 10.0f;
            int64_t timestamp = to_ms_since_boot(get_absolute_time());
            char mensagem[64];
            snprintf(mensagem, sizeof(mensagem), "T=%.1f %" PRId64, temp_simulada, timestamp);
            printf("Enviando nova mensagem: %s\n", mensagem);

            char mensagem_cript[64];
            xor_apply((const uint8_t *)mensagem, (uint8_t *)mensagem_cript, strlen(mensagem), XOR_KEY);

            if (is_mqtt_connected()) {
                mqtt_comm_publish(topico, (const uint8_t *)mensagem_cript, strlen(mensagem));
                printf("Mensagem publicada.\n");

                strcpy(penultima_msg, ultima_msg);
                strcpy(ultima_msg, mensagem);
                display_oled_exibir_mensagem(ultima_msg, penultima_msg, "", modo_atual);
                piscar_led(LED_VM);
                contador++;
            } else {
                printf("MQTT desconectado. Ignorando envio.\n");
                display_oled_exibir_mensagem("MQTT OFF", "", "", MODO_NONE);
            }
        }
        else if (!gpio_get(BOTAO_A)) {  // Botão A: enviar mensagem repetida (se houver)
            sleep_ms(300);
            if (strlen(ultima_msg) > 0) {
                printf("Enviando mensagem repetida: %s\n", ultima_msg);

                char mensagem_cript[64];
                xor_apply((const uint8_t *)ultima_msg, (uint8_t *)mensagem_cript, strlen(ultima_msg), XOR_KEY);

                if (is_mqtt_connected()) {
                    mqtt_comm_publish(topico, (const uint8_t *)mensagem_cript, strlen(ultima_msg));
                    printf("Mensagem repetida publicada.\n");

                    piscar_led(LED_VM);
                    contador++;
                } else {
                    printf("MQTT desconectado. Ignorando envio.\n");
                    display_oled_exibir_mensagem("MQTT OFF", "", "", MODO_NONE);
                }
            } else {
                printf("Nenhuma mensagem anterior para repetir.\n");
                // Opcional: mostrar no OLED ou piscar LED para indicar
            }
        }
    }
}
    