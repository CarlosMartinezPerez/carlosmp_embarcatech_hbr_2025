#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "include/display_oled.h"
#include "include/tratar_gpio.h"
#include <stdio.h>
#include <string.h>

static mqtt_client_t *client = NULL;
static char ultimo_topico[128] = {0};
static bool mqtt_connected = false;
static void (*user_callback)(const char*, const uint8_t*, size_t) = NULL;
static unsigned long ultima_timestamp_recebida = 0;
modo_t modo_atual = MODO_PUBLISHER;

bool is_mqtt_connected(void) {
    return mqtt_connected;
}

void mqtt_comm_disconnect(void) {
    if (client != NULL) {
        mqtt_client_free(client);
        client = NULL;
        mqtt_connected = false;
        printf("Cliente MQTT desconectado e liberado.\n");
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) { // Changed from MQTT_SUCCESS to MQTT_CONNECT_ACCEPTED
        mqtt_connected = true;
        printf("Conectado ao broker MQTT.\n");
    } else {
        printf("Falha na conexão com o broker. Código: %d\n", status);
        mqtt_comm_disconnect();
    }
}

static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso.\n");
    } else {
        printf("Erro ao publicar: %d\n", result);
    }
}

static void mqtt_sub_request_cb(void *arg, err_t err) {
    if (err == ERR_OK) {
        printf("Inscrição (subscribe) realizada com sucesso.\n");
    } else {
        printf("Falha ao realizar inscrição. Código de erro: %d\n", err);
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    printf("Mensagem recebida no tópico: %s, tamanho: %lu\n", topic, tot_len);
    strncpy(ultimo_topico, topic, sizeof(ultimo_topico) - 1);
    ultimo_topico[sizeof(ultimo_topico) - 1] = '\0';
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    if (user_callback) {
        user_callback(ultimo_topico, data, len);
    } else {
        printf("Payload recebido: %.*s\n", len, data);
    }
}

mqtt_client_t *mqtt_init_client(void) {
    mqtt_client_t *new_client = mqtt_client_new();
    if (new_client == NULL) {
        printf("Erro ao criar cliente MQTT.\n");
        return NULL;
    }
    mqtt_set_inpub_callback(new_client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
    return new_client;
}

mqtt_setup_result_t mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;

    mqtt_connected = false;
    mqtt_comm_disconnect();

    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        return MQTT_ERR_INVALID_IP;
    }

    client = mqtt_init_client();
    if (client == NULL) {
        return MQTT_ERR_CLIENT_INIT;
    }

    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,
        .client_user = user,
        .client_pass = pass,
        .keep_alive = 60
    };

    err_t err = mqtt_client_connect(client, &broker_addr, MQTT_PORT, mqtt_connection_cb, NULL, &ci);
    if (err != ERR_OK) {
        return MQTT_ERR_CONNECT_FAIL;
    }

    return MQTT_OK;
}

void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len, bool retained) {
    if (!mqtt_connected || client == NULL) {
        printf("Erro: Cliente MQTT não está conectado.\n");
        return;
    }

    err_t status = mqtt_publish(client, topic, data, len, 0, retained ? 1 : 0, mqtt_pub_request_cb, NULL);
    if (status != ERR_OK) {
        printf("Falha ao enviar publicação MQTT: %d\n", status);
    }
}

void mqtt_subscribe_to_topic(const char *topic) {
    if (!mqtt_connected || client == NULL) {
        printf("Erro: Cliente MQTT não está conectado.\n");
        return;
    }

    err_t err = mqtt_subscribe(client, topic, 0, mqtt_sub_request_cb, NULL);
    if (err != ERR_OK) {
        printf("Erro ao tentar subscrever: %d\n", err);
    }
}

void mqtt_register_callback(void (*cb)(const char*, const uint8_t*, size_t)) {
    if (!mqtt_connected || client == NULL) {
        printf("Erro: Cliente MQTT não está conectado para registrar callback.\n");
        return;
    }

    user_callback = cb;
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
}

void on_message_cb(const char* topic, const uint8_t* payload, size_t len) {
    if (len % 2 != 0) {
        printf("Payload com tamanho ímpar, inválido para hex.\n");
        return;
    }

    char mensagem[64] = {0};
    xor_decrypt_from_hex((const char*)payload, (uint8_t*)mensagem, XOR_KEY);

    float valor = 0;
    unsigned long nova_timestamp = 0;
    if (sscanf(mensagem, "T=%f %lu", &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem: %s\n", mensagem);
        return;
    }

    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Recebido: T=%.1f, timestamp=%lu\n", valor, nova_timestamp);
        piscar_led(LED_VD);
        char buf1[32], buf2[32];
        snprintf(buf1, sizeof(buf1), "T=%.1f", valor);
        snprintf(buf2, sizeof(buf2), "Ts=%lu", nova_timestamp);
        display_oled_exibir_mensagem(buf1, buf2, topic, modo_atual);
    } else {
        printf("Replay detectado: %s\n", mensagem);
    }
}