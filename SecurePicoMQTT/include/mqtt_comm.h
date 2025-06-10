#ifndef MQTT_COMMUNIC_H
#define MQTT_COMMUNIC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"

#ifndef XOR_KEY
#define XOR_KEY 42
#endif

typedef enum {
    MQTT_OK,
    MQTT_ERR_INVALID_IP,
    MQTT_ERR_CLIENT_INIT,
    MQTT_ERR_CONNECT_FAIL
} mqtt_setup_result_t;

typedef enum {
    MODO_PUBLISHER,
    MODO_SUBSCRIBER,
    MODO_NONE
} modo_t;

extern modo_t modo_atual;

// Inicializa o cliente MQTT (internamente usado no setup)
mqtt_client_t *mqtt_init_client(void);

// Realiza configuração e tentativa de conexão com o broker
mqtt_setup_result_t mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);

// Publica uma mensagem em um tópico
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len, bool retained);

// Inscreve o cliente MQTT em um tópico específico
void mqtt_subscribe_to_topic(const char *topic);

// Registra uma função callback para lidar com dados recebidos
void mqtt_register_callback(void (*cb)(const char*, const uint8_t*, size_t));

// Verifica se o cliente está conectado ao broker
bool is_mqtt_connected(void);

// Libera o cliente e desconecta do broker
void mqtt_comm_disconnect(void);

void on_message_cb(const char* topic, const uint8_t* payload, size_t len);

#endif