#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include "lwip/apps/mqtt.h"

/**
 * Função: mqtt_setup
 * Objetivo: Configurar e iniciar a conexão com o broker MQTT.
 * Parâmetros:
 *   - client_id: Identificador único do cliente
 *   - broker_ip: Endereço IP do broker (ex.: "192.168.1.1")
 *   - user: Nome de usuário para autenticação (pode ser NULL)
 *   - pass: Senha para autenticação (pode ser NULL)
 */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);

/**
 * Função: mqtt_comm_publish
 * Objetivo: Publicar dados em um tópico MQTT.
 * Parâmetros:
 *   - topic: Nome do tópico (ex.: "sensor/temperatura")
 *   - data: Payload da mensagem (bytes)
 *   - len: Tamanho do payload
 */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);

#endif // MQTT_COMM_H