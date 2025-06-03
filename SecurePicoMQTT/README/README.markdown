# SecurePicoMQTT - Segurança em IoT na BitDogLab com MQTT

## Visão Geral

Este projeto implementa uma comunicação MQTT segura utilizando a placa **BitDogLab baseada no Raspberry Pi Pico W**, programada em C com o SDK do Pico e a pilha de rede lwIP. O objetivo é estabelecer uma conexão Wi-Fi, conectar-se a um broker MQTT (Mosquitto), publicar mensagens em tópicos específicos, implementar autenticação, aplicar criptografia leve com XOR e adicionar proteção contra ataques de sniffing e replay, atendendo aos requisitos da **Tarefa Unidade 2 - Parte 2** do programa **EMBARCATECH**.

---

## Objetivos

- **Conexão Wi-Fi**: Estabelecer conexão com uma rede Wi-Fi protegida usando autenticação WPA2.
- **Conexão MQTT**: Conectar a placa ao broker Mosquitto com autenticação de usuário e senha.
- **Publicação de Mensagens**: Enviar mensagens em texto claro e criptografadas no tópico `escola/sala1/temperatura` e outros.
- **Criptografia Leve**: Implementar ofuscação com cifra XOR (chave 42) para proteger contra sniffing básico.
- **Proteção contra Replay**: Adicionar timestamp às mensagens para evitar ataques de repetição.
- **Diagnóstico de Rede**: Utilizar Wireshark para monitoramento e análise de tráfego, com solução de problemas de firewall.

---

## Introdução

- O que é MQTT?  
MQTT (Message Queuing Telemetry Transport) é um protocolo de comunicação leve e eficiente, projetado para conectar dispositivos em redes com pouca largura de banda, latência alta ou conectividade intermitente — exatamente o tipo de ambiente encontrado em aplicações de Internet das Coisas (IoT).
O MQTT segue o modelo publish/subscribe (publicação/assinatura), no qual os dispositivos podem atuar como:

  - Publicadores (publishers): enviam mensagens sobre determinado tópico (ex: "sensor/temperatura").

  - Subscritores (subscribers): recebem as mensagens dos tópicos que assinaram.

  Esse modelo desacopla os emissores e receptores — ou seja, eles não precisam conhecer a existência um do outro, o que facilita muito a escalabilidade e flexibilidade de sistemas IoT com dezenas ou centenas de dispositivos distribuídos.

- O que é um broker MQTT?  
  O broker é o servidor que faz o papel de intermediário: ele recebe mensagens dos publicadores e distribui para todos os subscritores interessados. Ele garante a entrega correta, eficiente e (em níveis mais avançados) confiável das mensagens.

- O que é o Mosquitto?  
  Mosquitto é uma implementação leve e de código aberto de um broker MQTT. Ele é amplamente usado em projetos de IoT por sua facilidade de uso, suporte a autenticação, criptografia e compatibilidade com diversas plataformas, incluindo sistemas embarcados como o Raspberry Pi Pico W.


## Materiais Necessários

- **Hardware**:
  - Placa BitDogLab com Raspberry Pi Pico W
- **Software**:
  - Visual Studio Code com extensões CMake e Pico SDK
  - Mosquitto (broker MQTT) configurado no sistema
  - Wireshark para análise de tráfego
  - SDK do Raspberry Pi Pico
  - Biblioteca lwIP para MQTT
- **Rede**:
  - Rede Wi-Fi protegida com WPA2 (SSID e senha fornecidos)

---

## Estrutura do Projeto

```
SecurePicoMQTT/
├── CMakeLists.txt
├── main.c
├──src/display_oled.c 
├──include/display_oled.h
├──src/mqtt_comm.c
├── include/mqtt_comm.h
├──src/mqtt_publisher.c
├──include/mqtt_publisher.h
├──src/mqtt_subscriber.c
├──include/mqtt_subscriber.h
├──src/tratar_gpio.c
├──include/tratar_gpio.h
├── src/wifi_conn.c
├── include/wifi_conn.h
├── src/xor_cipher.c
├── include/xor_cipher.h
├──oled/ssd1306_font.h
├──oled/ssd1306_i2c.c
├──oled/ssd1306_i2c.h
├──oled/ssd1306.h
├──.gitignore
├──mosq_pub.py
├── lwipopts.h
├── pico_sdk_import.cmake
└── README.md
```

---

## Etapas de Implementação

### Etapa 1: Conexão Wi-Fi
- **Objetivo**: Conectar a Pico W a uma rede Wi-Fi usando autenticação WPA2.
- **Implementação**:
  - Arquivos: `wifi_conn.c`, `wifi_conn.h`, `SecurePicoMQTT.c`, `CMakeLists.txt`
  - Código bloquenate (só libera após conectado) que configura o chip CYW43 para modo estação (STA) e tenta conexão com timeout de 30 segundos.

#### Código

- main.c:
```c
#include "include/wifi_conn.h"
#include "pico/stdlib.h"
#include <stdio.h>

int main() {
    // Inicializa a biblioteca padrão do Pico (necessária para GPIO, UART, etc.)
    stdio_init_all();
    sleep_ms(2000); // Tempo para abrir o monitor serial

    // Credenciais da rede Wi-Fi (substitua pelos valores reais)
    const char *ssid = "minha-rede-wifi";
    const char *password = "minha-senha-rede-wifi";

    // Chama a função para conectar ao Wi-Fi
    connect_to_wifi(ssid, password);

    // Loop principal para manter o programa em execução
    while (true) {
        // Adicione aqui futuras funcionalidades (ex.: MQTT, autenticação, criptografia)
        sleep_ms(1000); // Aguarda 1 segundo para evitar consumo excessivo de CPU
    }

    return 0;
}
```

- wifi_conn.c:
```c
#include "include/wifi_conn.h"         // Cabeçalho com a declaração da função de conexão Wi-Fi
#include "pico/cyw43_arch.h"           // Biblioteca para controle do chip Wi-Fi CYW43 no Raspberry Pi Pico W
#include <stdio.h>                     // Biblioteca padrão de entrada/saída (para usar printf)

/**
 * Função: connect_to_wifi
 * Objetivo: Inicializar o chip Wi-Fi da Pico W e conectar a uma rede usando SSID e senha fornecidos.
 */
void connect_to_wifi(const char *ssid, const char *password) {
    // Inicializa o driver Wi-Fi (CYW43). Retorna 0 se for bem-sucedido.
    if (cyw43_arch_init()) {
        printf("Erro ao iniciar Wi-Fi\n");
        return;
    }

    // Habilita o modo estação (STA) para se conectar a um ponto de acesso.
    cyw43_arch_enable_sta_mode();

    // Tenta conectar à rede Wi-Fi com um tempo limite de 30 segundos (30000 ms).
    // Utiliza autenticação WPA2 com criptografia AES.
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Erro ao conectar\n");  // Se falhar, imprime mensagem de erro.
    } else {        
        printf("Conectado ao Wi-Fi\n");  // Se conectar com sucesso, exibe confirmação.
    }
}

- wifi_conn.h:
```c
#ifndef WIFI_CONN_H
#define WIFI_CONN_H

void connect_to_wifi(const char *ssid, const char *password);

#endif
```

#### Resultado
- Conexão bem-sucedida, confirmada no monitor serial:
  ```
  ---- Opened the serial port COM4 ----
  Conectado ao Wi-Fi
  ```
---
### Etapa 2: Configuração MQTT Básica
- **Objetivo**: Estabelecer conexão com o broker Mosquitto e publicar mensagens no tópico `test/topic`.
- **Implementação**:
  - Arquivos adicionados: `main.c`, `mqtt_comm.c`, `mqtt_comm.h`, `lwipopts.h` modificado.
  - Configuração do broker com autenticação (usuário: `aluno`, senha: `senha123`).
  - Solução de problema: Firewall do Windows bloqueava pacotes MQTT na porta 1883. Criada regra de entrada para permitir tráfego TCP na porta 1883.

#### Código

- main.c:
```c
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

int main() {
    // Inicializa a biblioteca padrão do Pico (necessária para GPIO, UART, etc.)
    stdio_init_all();
    
    // Aguarda inicialização do USB para depuração (opcional, dá tempo para o console conectar)
    sleep_ms(5000);

    // Credenciais da rede Wi-Fi (substitua pelos valores reais)
    const char *ssid = "VIVOFIBRA-8991_EXT";
    const char *password = "cajuca1801";

    // Conectar ao Wi-Fi
    connect_to_wifi(ssid, password);

    // Aguarda a conexão Wi-Fi estabilizar
    sleep_ms(2000);

    // Configurações do broker MQTT
    const char *client_id = "pico_client";
    const char *broker_ip = "127.0.0.1"; // Substitua pelo IP do broker (localhost para testes locais)
    const char *user = "CarlosMP";      // Substitua pelo usuário configurado no Mosquitto
    const char *pass = "cajuca1801";      // Substitua pela senha configurada no Mosquitto

    // Configura e conecta ao broker MQTT
    mqtt_setup(client_id, broker_ip, user, pass);

    // Aguarda a conexão MQTT (dá tempo para o callback confirmar)
    sleep_ms(2000);

    // Loop principal para publicar mensagens de teste
    while (true) {
        // Mensagem de teste
        const char *message = "Hello, MQTT!";
        const char *topic = "test/topic";
        
        // Publica a mensagem no tópico
        mqtt_comm_publish(topic, (const uint8_t *)message, strlen(message));

        // Aguarda 5 segundos antes de publicar novamente
        sleep_ms(5000);
    }

    return 0;
}
```

- mqtt_comm.c:
```c
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "include/mqtt_comm.h"    // Header file com as declarações locais
// Base: https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/lwipopts.h
#include "lwipopts.h"             // Configurações customizadas do lwIP

/* Variável global estática para armazenar a instância do cliente MQTT
 * 'static' limita o escopo deste arquivo */
static mqtt_client_t *client;

/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string (ex: "192.168.1.1")
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;  // Estrutura para armazenar o IP do broker
    
    // Converte o IP de string para formato numérico
    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Erro no IP\n");
        return;
    }

    // Cria uma nova instância do cliente MQTT
    client = mqtt_client_new();
    if (client == NULL) {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    // Configura as informações de conexão do cliente
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,  // ID do cliente
        .client_user = user,     // Usuário (opcional)
        .client_pass = pass      // Senha (opcional)
    };

    // Inicia a conexão com o broker
    // Parâmetros:
    //   - client: instância do cliente
    //   - &broker_addr: endereço do broker
    //   - 1883: porta padrão MQTT
    //   - mqtt_connection_cb: callback de status
    //   - NULL: argumento opcional para o callback
    //   - &ci: informações de conexão
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso!\n");
    } else {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    // Envia a mensagem MQTT
    err_t status = mqtt_publish(
        client,              // Instância do cliente
        topic,               // Tópico de publicação
        data,                // Dados a serem enviados
        len,                 // Tamanho dos dados
        0,                   // QoS 0 (nenhuma confirmação)
        0,                   // Não reter mensagem
        mqtt_pub_request_cb, // Callback de confirmação
        NULL                 // Argumento para o callback
    );

    if (status != ERR_OK) {
        printf("mqtt_publish falhou ao ser enviada: %d\n", status);
    }
}
```

- mqtt_comm.h:
```c
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

#endif
```
- modificações em lwipopts.h:  
```c
#define MEMP_NUM_SYS_TIMEOUT   (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)
#define MQTT_REQ_MAX_IN_FLIGHT  (5)
```

#### Resultado
- Monitor serial:
Monitor Serial:
```
---- Opened the serial port COM4 ----
Conectado ao Wi-Fi
Conectado ao broker MQTT com sucesso!
Publicação MQTT enviada com sucesso!
Publicação MQTT enviada com sucesso!
Publicação MQTT enviada com sucesso!
Publicação MQTT enviada com sucesso!
Publicação MQTT enviada com sucesso!
```



    ```
    Conectado ao Wi-Fi
    Conectado ao broker MQTT com sucesso!
    Publicação MQTT enviada com sucesso!
    ```
  - Log do Mosquitto:
    ```
    New client connected from 192.168.15.102 as pico_client
    Received PUBLISH from pico_client on topic test/topic
    ```

### Etapa 3: Publicação em Texto Claro
- **Objetivo**: Publicar mensagem `"26.5"` no tópico `escola/sala1/temperatura`.
- **Implementação**:
  - Substituição no `SecurePicoMQTT.c`:
    ```c
    const char *message = "26.5";
    const char *topic = "escola/sala1/temperatura";
    ```
- **Resultado**:
  - Mensagem capturada no Wireshark em texto claro (`32362e35` em ASCII hexadecimal, decodificado como `"26.5"`).
  - Log do Mosquitto:
    ```
    Received PUBLISH from pico_client (d0, q0, r0, m0, 'escola/sala1/temperatura', ... (4 bytes))
    ```

### Etapa 4: Autenticação no Mosquitto
- **Objetivo**: Configurar autenticação no broker e testar com cliente.
- **Implementação**:
  - Configuração do `mosquitto.conf`:
    ```conf
    listener 1883
    allow_anonymous false
    password_file C:\caminho\para\passwd
    ```
  - Criação de senha:
    ```
    mosquitto_passwd -c C:\caminho\para\passwd CarlosMP
    ```
  - Testes com `mosquitto_pub` e `mosquitto_sub` confirmaram funcionamento.
- **Resultado**:
  - Publicações manuais de `"37.3"`, `"38.4"`, `"40.2"` no tópico `escola/sala1/temperatura` foram recebidas corretamente.

### Etapa 5: Criptografia com XOR
- **Objetivo**: Ofuscar mensagens com cifra XOR (chave 42) para evitar sniffing básico.
- **Implementação**:
  - Arquivos adicionados: `xor_cipher.c`, `xor_cipher.h`
  - Código em `SecurePicoMQTT.c`:
    ```c
    const char* mensagem_clara = "26.5";
    char mensagem_cript[32];
    xor_encrypt((const uint8_t*)mensagem_clara, (uint8_t*)mensagem_cript, strlen(mensagem_clara), XOR_KEY);
    mqtt_comm_publish(MQTT_TOPIC, (uint8_t*)mensagem_cript, strlen(mensagem_clara));
    ```
  - Mensagem `"26.5"` criptografada para `181C041F` (XOR com 0x2A).
- **Resultado**:
  - Wireshark capturou mensagem ofuscada (`181C041F`).
  - Decriptografia via Python:
    ```python
    msg = bytes([0x18, 0x1C, 0x04, 0x1F])
    decifrada = bytes([b ^ 42 for b in msg])
    print(decifrada.decode())  # Saída: 26.5
    ```
  - Log do Mosquitto confirmou recebimento.

### Etapa 6: Proteção contra Replay
- **Objetivo**: Adicionar timestamp às mensagens para evitar ataques de replay.
- **Implementação**:
  - Timestamp gerado com `to_ms_since_boot(get_absolute_time())` (ex.: `396547116` ms ≈ 6 minutos e 36 segundos desde o boot).
  - Sugestão de formato legível: `T=26.5 t=396.5s`.
- **Resultado**:
  - Timestamp incluído, mas formato bruto exibido (`396547116`). Necessária implementação no subscriber para validar timestamps e descartar mensagens antigas.

---

## 📡 Resultados

- **Conexão Wi-Fi**: Estabelecida com sucesso.
- **Conexão MQTT**: Funcionando com autenticação no broker Mosquitto.
- **Publicação**: Mensagens enviadas em texto claro e criptografadas com XOR.
- **Segurança**:
  - Autenticação implementada no Mosquitto.
  - Criptografia XOR aplicada, com mensagem ofuscada capturada no Wireshark.
  - Timestamp adicionado para proteção contra replay (pendente validação no subscriber).
- **Diagnóstico**: Problema de firewall resolvido com regra TCP na porta 1883. Wireshark foi essencial para identificar pacotes bloqueados.

---

## 🧠 Lições Aprendidas

- **Firewall do Windows**: Pode bloquear pacotes MQTT silenciosamente, mesmo com regras explícitas. O Wireshark em modo promíscuo mascara o problema temporariamente.
- **lwIP**: Sensível a descartes silenciosos de pacotes, exigindo configuração cuidadosa do ambiente de rede.
- **Criptografia XOR**: Eficaz para ofuscação básica, mas inadequada para segurança robusta devido à simplicidade.
- **Testes de Rede**: Essenciais para validar comportamento em ambientes reais, especialmente em sistemas embarcados.

---

## 🔒 Próximos Passos

- **Melhorar Criptografia**: Substituir XOR por AES ou HMAC usando mbedTLS para maior segurança.
- **Proteger contra Replay**: Implementar validação de timestamps no subscriber, descartando mensagens com timestamps antigos.
- **Escalabilidade**: Testar com múltiplas placas BitDogLab em uma rede escolar, ajustando QoS e tópicos.
- **TLS**: Migrar para MQTT com TLS (porta 8883) para criptografia de transporte.

---

## 📚 Referências

- [Raspberry Pi Pico SDK Networking](https://www.raspberrypi.com/documentation/pico-sdk/networking.html)
- [Mosquitto Documentation](https://mosquitto.org/)
- [lwIP Documentation](https://www.nongnu.org/lwip/)
- [BitDogLab GitHub](https://github.com/BitDogLab/BitDogLab-C)

---

## 💡 Observações

- Durante os testes, foi necessário manter o Wireshark ativo para contornar bloqueios do firewall no Windows. Para ambientes de produção, recomenda-se usar Linux ou configurar o broker em um dispositivo dedicado (ex.: Raspberry Pi).
- A implementação de timestamp no subscriber precisa ser concluída para validar proteção contra replay.