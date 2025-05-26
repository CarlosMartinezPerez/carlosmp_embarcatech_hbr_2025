#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "oled/ssd1306.h"
#include "oled/ssd1306_i2c.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "include/xor_decipher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/apps/mqtt.h"

#define BOTAO_A 5
#define BOTAO_B 6
#define LED_VM 13  // LED vermelho
#define LED_VD 11  // LED verde
#define I2C_SDA 14
#define I2C_SCL 15
#define REPETICAO 5
#define WIFI_SSID "VIVOFIBRA-8991_EXT"
#define WIFI_PASS "cajuca1801"
#define MQTT_BROKER_IP "192.168.15.101"
#define MQTT_USER "CarlosMP"
#define MQTT_PASS "cajuca1801"
#define MQTT_TOPIC "test/topic"
#define XOR_KEY 42

// Variável global para controle de replay
static uint32_t ultima_timestamp_recebida = 0;

// Assumindo que mqtt_client é definido em mqtt_comm.c
extern mqtt_client_t* mqtt_client;

void exibir_mensagem_oled(const char* msg1, const char* msg2);

// Callback para cabeçalho de mensagens recebidas
void on_message(void *arg, const char *topic, u32_t tot_len) {
    printf("Mensagem recebida no tópico: %s, tamanho total: %lu\n", topic, tot_len);
}

// Callback para dados da mensagem recebida
void on_message_data(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    char mensagem_cript[64] = {0};
    char mensagem[64] = {0};

    if (len >= sizeof(mensagem_cript)) len = sizeof(mensagem_cript) - 1;

    memcpy(mensagem_cript, data, len);
    mensagem_cript[len] = '\0';

    xor_decrypt((const uint8_t*)mensagem_cript, (uint8_t*)mensagem, len, XOR_KEY);
    mensagem[len] = '\0';

    // Parse do JSON {"valor":<float>,"ts":<uint32>}
    float valor = 0;
    uint32_t nova_timestamp = 0;
    if (sscanf(mensagem, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem JSON: %s\n", mensagem);
        return;
    }

    // Verificação de replay
    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Nova leitura: %.2f (ts: %lu)\n", valor, nova_timestamp);

        // Mostrar no OLED
        char buf[32];
        snprintf(buf, sizeof(buf), "Val: %.2f Ts:%lu", valor, nova_timestamp);
        exibir_mensagem_oled("SUBSCRIBER", buf);
    } else {
        printf("Replay detectado (ts: %lu <= %lu)\n", nova_timestamp, ultima_timestamp_recebida);
    }
}

// Callback para confirmar a subscrição
void on_subscribe(void *arg, err_t err) {
    if (err == ERR_OK) {
        printf("Subscrição bem-sucedida no tópico %s\n", (const char*)arg);
    } else {
        printf("Falha na subscrição: erro %d\n", err);
    }
}

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

void exibir_mensagem_oled(const char* msg1, const char* msg2) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, (char*)msg1);
    ssd1306_draw_string(fb, 0, 16, (char*)msg2);

    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(fb, &area);
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    // Inicializar Wi-Fi em modo background
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar cyw43_arch\n");
        return 1;
    }

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
    sleep_ms(2000);

    inicializa_display();

    while (true) {
        if (!gpio_get(BOTAO_A)) {
            mostrar_modo_selecionado("PUBLISHER");

            printf("Conectando ao Wi-Fi...\n");
            connect_to_wifi(WIFI_SSID, WIFI_PASS);
            printf("Conectado ao Wi-Fi.\n");

            printf("Conectando ao broker MQTT...\n");
            mqtt_setup("pico_client", MQTT_BROKER_IP, MQTT_USER, MQTT_PASS);
            if (mqtt_client == NULL) {
                printf("Erro: mqtt_client não inicializado\n");
                sleep_ms(1000);
                continue;
            }
            printf("Conectado ao broker MQTT.\n");

            int contador = 0;
            char ultima_msg[64] = "";
            char penultima_msg[64] = "";

            while (true) {
                if (!gpio_get(BOTAO_B)) {
                    char mensagem[64];
                    if ((contador + 1) % REPETICAO == 0 && strlen(ultima_msg) > 0) {
                        strcpy(mensagem, ultima_msg);
                        printf("Enviando mensagem repetida: %s\n", mensagem);
                    } else {
                        float temp_simulada = 25.0f + (rand() % 50) / 10.0f;
                        snprintf(mensagem, sizeof(mensagem), "{\"valor\":%.1f,\"ts\":%llu}", 
                                 temp_simulada, to_ms_since_boot(get_absolute_time()));
                        printf("Enviando nova mensagem: %s\n", mensagem);
                    }

                    char mensagem_cript[64];
                    xor_encrypt((const uint8_t *)mensagem, (uint8_t *)mensagem_cript, strlen(mensagem), XOR_KEY);
                    mqtt_comm_publish(MQTT_TOPIC, (const uint8_t *)mensagem_cript, strlen(mensagem));
                    printf("Mensagem publicada (sem retorno).\n");

                    strcpy(penultima_msg, ultima_msg);
                    strcpy(ultima_msg, mensagem);
                    exibir_mensagem_oled(ultima_msg, penultima_msg);

                    contador++;
                    piscar_led(LED_VM);
                    sleep_ms(500);
                }
                sleep_ms(10); // Pequeno delay para evitar uso excessivo da CPU
            }
        } else if (!gpio_get(BOTAO_B)) {
            mostrar_modo_selecionado("SUBSCRIBER");

            printf("Conectando ao Wi-Fi...\n");
            connect_to_wifi(WIFI_SSID, WIFI_PASS);
            printf("Conectado ao Wi-Fi.\n");

            printf("Conectando ao broker MQTT...\n");
            mqtt_setup("pico_client_sub", MQTT_BROKER_IP, MQTT_USER, MQTT_PASS);
            if (mqtt_client == NULL) {
                printf("Erro: mqtt_client não inicializado\n");
                sleep_ms(1000);
                continue;
            }

            // Configurar callback para mensagens recebidas
            mqtt_set_inpub_callback(mqtt_client, on_message, on_message_data, NULL);

            // Registrar subscrição no tópico
            mqtt_subscribe(mqtt_client, MQTT_TOPIC, 0, on_subscribe, (void*)MQTT_TOPIC);
            printf("Solicitando subscrição no tópico %s...\n", MQTT_TOPIC);

            while (true) {
                piscar_led(LED_VD); // Feedback visual
                sleep_ms(100);      // Pequeno delay
            }
        }

        sleep_ms(100);
    }
}