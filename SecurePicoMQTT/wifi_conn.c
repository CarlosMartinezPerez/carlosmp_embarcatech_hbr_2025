#include "include/wifi_conn.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

void connect_to_wifi(const char *ssid, const char *password) {
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro ao conectar ao Wi-Fi\n");
    } else {
        printf("Conectado ao Wi-Fi\n");
    }
}