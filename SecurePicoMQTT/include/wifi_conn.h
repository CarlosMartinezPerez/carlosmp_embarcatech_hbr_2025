#ifndef WIFI_CONN_H
#define WIFI_CONN_H

/**
 * Função: connect_to_wifi
 * Objetivo: Conectar o Raspberry Pi Pico W a uma rede Wi-Fi usando SSID e senha fornecidos.
 * Parâmetros:
 *   - ssid: Nome da rede Wi-Fi
 *   - password: Senha da rede Wi-Fi
 */
void connect_to_wifi(const char *ssid, const char *password);

#endif // WIFI_CONN_H