#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"
#include "oled/ssd1306.h"
#include "oled/ssd1306_i2c.h"
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BOTAO_A 5
#define BOTAO_B 6
#define LED_VM 13  // LED vermelho
#define LED_VD 11  // LED verde
#define I2C_SDA 14
#define I2C_SCL 15
#define REPETICAO 5
#define WIFI_SSID "CARLOSMP"
#define WIFI_PASS "cajuca1801"
#define MQTT_BROKER_IP "to be find"
#define MQTT_USER "residente"
#define MQTT_PASS "test1234"
#define MQTT_TOPIC "test/topic"
#define XOR_KEY 42

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
    gpio_put(gpio_led, 1); // 10% ligado
    sleep_ms(100);
    gpio_put(gpio_led, 0); // 90% desligado
    sleep_ms(900);
}

void exibir_mensagem_oled(const char* msg1, const char* msg2) {
    static uint8_t fb[ssd1306_buffer_length];
    memset(fb, 0, sizeof(fb));
    ssd1306_draw_string(fb, 0, 0, "PUBLISHER");
    ssd1306_draw_string(fb, 0, 16, (char*)msg1);
    ssd1306_draw_string(fb, 0, 32, (char*)msg2);

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
                        snprintf(mensagem, sizeof(mensagem), "T=%.1f %llu", temp_simulada, to_ms_since_boot(get_absolute_time()));
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
            }
        }

        if (!gpio_get(BOTAO_B)) {
            mostrar_modo_selecionado("SUBSCRIBER");
            while (true) {
                piscar_led(LED_VD);
            }
        }

        sleep_ms(100);
    }
}
