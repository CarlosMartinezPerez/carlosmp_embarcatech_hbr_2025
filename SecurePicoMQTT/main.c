#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "oled/ssd1306.h"
#include "oled/ssd1306_i2c.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "include/xor_decipher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define BOTAO_A 5
#define BOTAO_B 6
#define LED_VM 13
#define LED_VD 11
#define I2C_SDA 14
#define I2C_SCL 15
#define REPETICAO 5
#define WIFI_SSID "VIVOFIBRA-8991_EXT"
#define WIFI_PASS "cajuca1801"
#define MQTT_BROKER_IP "192.168.15.104"
#define MQTT_USER "CarlosMP"
#define MQTT_PASS "cajuca1801"
#define XOR_KEY 42

// === Configurações da placa ===
#define CLIENT_ID "pico_pub_sala1"  // 🔁 Troque aqui antes de compilar para cada placa publisher
#define MQTT_PUB_TOPIC "escola/sala1/temperatura" // Escolha
#define MQTT_SUB_TOPIC "escola/#"

modo_t modo_atual = MODO_PUBLISHER;

static int64_t ultima_timestamp_recebida = 0;

void inicializar_oled_i2c(void) {
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void inicializa_display() {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, "ESCOLHA:");
    ssd1306_draw_string(fb, 0, 16, "A  PUBLISHER");
    ssd1306_draw_string(fb, 0, 32, "B  SUBSCRIBER");

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

void mostrar_modo_selecionado(const char* modo) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, (char*)modo);

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

void piscar_led(uint gpio_led) {
    gpio_put(gpio_led, 1);
    sleep_ms(100);
    gpio_put(gpio_led, 0);
    sleep_ms(900);
}

void exibir_mensagem_oled(const char* msg1, const char* msg2, const char* topic, modo_t modo) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));

    if (modo == MODO_PUBLISHER) {
        ssd1306_draw_string(fb, 0, 0, "PUBLISHER");
    } else if (modo == MODO_SUBSCRIBER) {
        ssd1306_draw_string(fb, 0, 0, "SUBSCRIBER");
    } // se for MODO_NONE, não escreve nada no topo

    ssd1306_draw_string(fb, 0, 16, (char*)msg1);
    ssd1306_draw_string(fb, 0, 32, (char*)msg2);
    if (topic != NULL && modo == MODO_SUBSCRIBER) {
        ssd1306_draw_string(fb, 0, 48, (char*)topic);
    }

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

// Converte string hex ASCII para bytes binários
void hexstr_to_bytes(const char* hexstr, uint8_t* bytes, size_t bytes_len) {
    for (size_t i = 0; i < bytes_len; i++) {
        sscanf(hexstr + 2*i, "%2hhx", &bytes[i]);
    }
}

// Trata as mensagens recebidas: descriptografa, extrai dados (temperatura e timestamp), verifica se não
// é mensagem repetida e exibe os dados no display OLED, se for válida
void on_message_cb(const char* topic, const uint8_t* payload, size_t len) {
    if (len % 2 != 0) {
        printf("Payload com tamanho ímpar, inválido para hex.\n");
        return;
    }

    size_t bytes_len = len / 2;
    uint8_t mensagem_cript[64] = {0};
    char mensagem[64] = {0};

    // Converter hex ASCII para bytes binários
    hexstr_to_bytes((const char*)payload, mensagem_cript, bytes_len);

    // Descriptografar
    xor_decrypt(mensagem_cript, (uint8_t*)mensagem, bytes_len, XOR_KEY);
    mensagem[bytes_len] = '\0';

    float valor = 0;
    int64_t nova_timestamp = 0;
    if (sscanf(mensagem, "T=%f %" SCNd64, &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem: %s\n", mensagem);
        return;
    }

    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Recebido: T=%.2f, timestamp=%" PRId64 "\n", valor, nova_timestamp);

        char buf1[32], buf2[32];
        snprintf(buf1, sizeof(buf1), "T=%.2f", valor);
        snprintf(buf2, sizeof(buf2), "ts=%" PRId64, nova_timestamp);
        exibir_mensagem_oled(buf1, buf2, topic, modo_atual);
    } else {
        printf("Replay detectado: %s\n", mensagem);
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    gpio_init(LED_VM);
    gpio_set_dir(LED_VM, GPIO_OUT);
    gpio_put(LED_VM, 0);

    gpio_init(LED_VD);
    gpio_set_dir(LED_VD, GPIO_OUT);
    gpio_put(LED_VD, 0);

    inicializar_oled_i2c();
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    ssd1306_init();

    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    struct render_area area_clear = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area_clear);
    render_on_display(fb, &area_clear);
    sleep_ms(1000); // Tempo para o display limpar

    inicializa_display();

    while (true) {
        if (!gpio_get(BOTAO_A)) { // Modo PUBLISHER
            sleep_ms(300);
            modo_atual = MODO_PUBLISHER;
            mostrar_modo_selecionado("PUBLISHER");
            printf("Modo selecionado: PUBLISHER\n");

            printf("Conectando ao Wi-Fi...\n");
            connect_to_wifi(WIFI_SSID, WIFI_PASS);

            printf("Iniciando setup MQTT...\n");
            mqtt_setup_result_t result = mqtt_setup(CLIENT_ID, MQTT_BROKER_IP, MQTT_USER, MQTT_PASS);

            if (result != MQTT_OK) {
                switch (result) {
                    case MQTT_ERR_INVALID_IP:
                        exibir_mensagem_oled("MQTT ERRO", "IP inválido", "", MODO_NONE);
                        break;
                    case MQTT_ERR_CLIENT_INIT:
                        exibir_mensagem_oled("MQTT ERRO", "Falha no client", "", MODO_NONE);
                        break;
                    case MQTT_ERR_CONNECT_FAIL:
                        exibir_mensagem_oled("MQTT ERRO", "Erro ao conectar", "", MODO_NONE);
                        break;
                    default:
                        exibir_mensagem_oled("MQTT ERRO", "Erro desconhecido", "", MODO_NONE);
                        break;
                }
                sleep_ms(3000);
                inicializa_display();
                continue;
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
                exibir_mensagem_oled("MQTT ERRO", "Timeout", "", MODO_NONE);
                sleep_ms(2000);
                inicializa_display();
                continue;
            }

            printf("Conectado ao broker MQTT como PUBLISHER.\n");

            int contador = 0;
            char ultima_msg[64] = "";
            char penultima_msg[64] = "";

            while (true) {
                if (!gpio_get(BOTAO_B)) {
                    sleep_ms(300);
                    char mensagem[64];

                    if ((contador + 1) % REPETICAO == 0 && strlen(ultima_msg) > 0) {
                        strcpy(mensagem, ultima_msg);
                        printf("Enviando mensagem repetida: %s\n", mensagem);
                    } else {
                        float temp_simulada = 25.0f + (rand() % 50) / 10.0f;
                        int64_t timestamp = to_ms_since_boot(get_absolute_time());
                        snprintf(mensagem, sizeof(mensagem), "T=%.1f %" PRId64, temp_simulada, timestamp);
                        printf("Enviando nova mensagem: %s\n", mensagem);
                    }

                    char mensagem_cript[64];
                    xor_encrypt((const uint8_t *)mensagem, (uint8_t *)mensagem_cript, strlen(mensagem), XOR_KEY);

                    // Imprime a mensagem original
                    printf("Mensagem original: %s\n", mensagem);

                    if (is_mqtt_connected()) {
                        mqtt_comm_publish(MQTT_PUB_TOPIC, (const uint8_t *)mensagem_cript, strlen(mensagem));
                        printf("Mensagem publicada.\n");

                        strcpy(penultima_msg, ultima_msg);
                        strcpy(ultima_msg, mensagem);
                        exibir_mensagem_oled(ultima_msg, penultima_msg, "", modo_atual);
                        piscar_led(LED_VM);
                        contador++;
                    } else {
                        printf("MQTT desconectado. Ignorando envio.\n");
                        exibir_mensagem_oled("MQTT OFF", "", "", MODO_NONE);
                    }
                }
            }
        }
        else if (!gpio_get(BOTAO_B)) { // Modo SUBSCRIBER
            sleep_ms(300);
            modo_atual = MODO_SUBSCRIBER;
            mostrar_modo_selecionado("SUBSCRIBER");
            printf("Modo selecionado: SUBSCRIBER\n");

            printf("Conectando ao Wi-Fi...\n");
            connect_to_wifi(WIFI_SSID, WIFI_PASS);

            printf("Iniciando setup MQTT...\n");
            mqtt_setup_result_t result = mqtt_setup("pico_client_sub", MQTT_BROKER_IP, MQTT_USER, MQTT_PASS);

            if (result != MQTT_OK) {
                switch (result) {
                    case MQTT_ERR_INVALID_IP:
                        exibir_mensagem_oled("MQTT ERRO", "IP inválido", "", MODO_NONE);
                        break;
                    case MQTT_ERR_CLIENT_INIT:
                        exibir_mensagem_oled("MQTT ERRO", "Falha no client", "", MODO_NONE);
                        break;
                    case MQTT_ERR_CONNECT_FAIL:
                        exibir_mensagem_oled("MQTT ERRO", "Erro ao conectar", "", MODO_NONE);
                        break;
                    default:
                        exibir_mensagem_oled("MQTT ERRO", "Erro desconhecido", "", MODO_NONE);
                        break;
                }
                sleep_ms(3000);
                inicializa_display();
                continue;
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
                exibir_mensagem_oled("MQTT ERRO", "Timeout", "", MODO_NONE);
                inicializa_display();
                continue;
            }

            printf("Conectado ao broker MQTT como SUBSCRIBER.\n");

            mqtt_register_callback(on_message_cb);
            mqtt_subscribe_to_topic(MQTT_SUB_TOPIC);

            // Loop principal
            while (true) {
                // Liga o LED por 0,1 segundo
                gpio_put(LED_VD, 1);
                sleep_ms(10);
                // Desliga o LED por 1,5 segundo
                gpio_put(LED_VD, 0);
                sleep_ms(2000);
            }
        }
    }
}