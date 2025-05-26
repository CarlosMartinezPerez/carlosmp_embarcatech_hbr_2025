#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>

// Variável global para verificar conexão MQTT (externa, definida em mqtt_comm.c)
extern volatile bool mqtt_connected;

int main() {
    // Inicializa a biblioteca padrão do Pico (necessária para GPIO, UART, etc.)
    stdio_init_all();

    // Aguarda inicialização do USB para depuração (opcional)
    sleep_ms(5000);

    // Credenciais da rede Wi-Fi
    const char *ssid = "VIVOFIBRA-8991_EXT";
    const char *password = "cajuca1801";

    // Conectar ao Wi-Fi
    connect_to_wifi(ssid, password);

    // Aguarda a conexão Wi-Fi estabilizar
    sleep_ms(2000);

    // Configurações do broker MQTT
    const char *client_id = "pico_client";
    const char *broker_ip = "192.168.15.101";  // IP real da máquina com o Mosquitto
    const char *user = "CarlosMP";
    const char *pass = "cajuca1801";

    // Inicia a conexão com o broker MQTT
    mqtt_setup(client_id, broker_ip, user, pass);

    // Aguarda até a conexão MQTT ser confirmada pelo callback (máximo 10s)
    for (int i = 0; i < 50 && !mqtt_connected; ++i) {
        printf("Aguardando conexão MQTT...\n");
        sleep_ms(200);
    }

    if (!mqtt_connected) {
        printf("Falha ao conectar ao broker MQTT. Abortando.\n");
        return 1;
    }

    // Loop principal: publica mensagem a cada 5 segundos
    while (true) {
        const char *message = "Hello, MQTT!";
        const char *topic = "test/topic";

        mqtt_comm_publish(topic, (const uint8_t *)message, strlen(message));

        sleep_ms(5000);
    }

    return 0;
}
