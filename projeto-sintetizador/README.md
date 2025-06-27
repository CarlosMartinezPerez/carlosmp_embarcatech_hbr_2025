# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: **Carlos Martinez Perez**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, junho de 2025

---
# Projeto Sintetizador de Áudio
#### Grava e reproduz áudio na BitDogLab

## Etapas do Desenvolvimento

1. Aquisição do sinal do microfone (via ADC)

Desenvolva uma função que aquisite o sinal do microfone (via ADC) e o envie (na forma de valor numérico) ao terminal, permitindo sua visualização. Atente para uma taxa de amostragem adequada.  
Teste essa função: Use um loop para chamar a função repetidamente, detectando vários valores em sequência (recomenda-se um pequeno delay entre cada registro). Observe a variação dos números impressos no terminal.  

Solução: Para responder a esta primeira demanda, adicionam-se 4 arquivos ao projeto. É importante notar que toda a gravação deste projeto, a menos que se use DMA, ocorre com os dados do áudio capturado sendo gravados na memória RAM.    

### Código

main1.c:
```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include <stdint.h>

#define SAMPLE_INTERVAL_US 125  // 8kHz (1s / 8000 = 125µs)

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o monitor serial
    adc_reader_init();

    printf("Iniciando leitura contínua do microfone...\n");

    while (true) {
        uint16_t sample = adc_read_microphone();
        printf("%u\n", sample);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    return 0;
}
```

adc_reader.c:
```c
#include "adc_reader.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdint.h>

#define MIC_ADC_CHANNEL 2  // GPIO 28 → ADC2

void adc_reader_init(void) {
    adc_init();
    adc_gpio_init(28);                // GPIO 28 como entrada ADC
    adc_select_input(MIC_ADC_CHANNEL);
}

uint16_t adc_read_microphone(void) {
    return adc_read();               // Retorna valor de 12 bits (0–4095)
}
```

adc_reader.h:
```c
##ifndef ADC_READER_H
#define ADC_READER_H

#include <stdint.h>

void adc_reader_init(void);
uint16_t adc_read_microphone(void);

#endif
```

CMakeLists.txt:
```c
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(synth_audio C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable( synth_audio
        main1.c
        src/adc_reader.c
)

pico_set_program_name(synth_audio "synth_audio")
pico_set_program_version(synth_audio "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(synth_audio 0)
pico_enable_stdio_usb(synth_audio 1)

# Add the standard library to the build
target_link_libraries(synth_audio
        # Biblioteca padrão do Pico SDK, que fornece funções básicas para o RP2040 (GPIO, temporizadores, UART, etc.).
        pico_stdlib
        )

# Add the standard include files to the build
target_include_directories(synth_audio PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/src
)

# Add any user requested libraries
target_link_libraries(synth_audio 
        hardware_adc
        hardware_dma
        hardware_pwm
)

pico_add_extra_outputs(synth_audio)
```
### Notas da implementação

- O sleep_us(125) configura a taxa de amostragem para aproximadamente 8kHz, suficiente para testes com voz. 1 segundo dividido em 8000 amostras resulta em 0.000125 segundo por amostra ou 125 us.  
- O terminal exibirá uma sequência de números entre 0 e 4095 (ADC de 12 bits), representando o sinal captado.  

### Resultado obtido

Monitor Serial:  
---- Opened the serial port COM4 ----  
Iniciando leitura contínua do microfone...  
2022  
2008  
2005  
2022  
2038  
2043  
2034  
2036  
2019  
2011  
2033  
2034  
2019  
2034  
2037  
2027  
...

---

2. Armazenamento dos dados de ADC num buffer

Altere a função anterior para que ela, ao invés de imprimir o valor no terminal, o armazene em um buffer (tipo de lista ou array, de tamanho pré-definido) – contanto que ainda haja espaço vago nele. Pode-se usar um contador para garantir que o buffer esteja cheio ao final da gravação.   
Teste essa função: Similar ao feito no item 1, use um loop para detectar (e armazenar) vários valores em sequência. Feito isso, imprima o conteúdo do buffer no terminal para comparar os valores armazenados com os números mostrados no passo anterior.  

Solução: A solução envolve a adição de adc_buffer.c e adc_buffer.h, além da alteração em CMakeLists.txt para adicionar esse módulo como executável na compilação e as modofocações para incorporar o buffer no programa proncipal, agora main2.c.  

### Código

main2.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"

#define SAMPLE_INTERVAL_US 125  // 8 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000);  // tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();

    printf("Iniciando aquisição de 2 segundos de áudio...\n");

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_read_microphone();
        adc_buffer_add_sample(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    printf("Amostras capturadas:\n");
    adc_buffer_print();

    return 0;
}
```

adc_buffer.c:  
```c
#include <stdio.h>
#include "adc_buffer.h"

static uint16_t buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

void adc_buffer_init(void) {
    buffer_index = 0;
}

bool adc_buffer_add_sample(uint16_t sample) {
    if (buffer_index < BUFFER_SIZE) {
        buffer[buffer_index++] = sample;
        return true;
    }
    return false;  // buffer cheio
}

bool adc_buffer_is_full(void) {
    return buffer_index >= BUFFER_SIZE;
}

void adc_buffer_print(void) {
    for (size_t i = 0; i < buffer_index; ++i) {
        printf("Amostra %5zu: %u\n", i, buffer[i]);
    }
}
```

adc_buffer.h:  
```c
#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_RATE_HZ     8000
#define RECORD_DURATION_S  2
#define BUFFER_SIZE (SAMPLE_RATE_HZ * RECORD_DURATION_S)

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
bool adc_buffer_is_full(void);
void adc_buffer_print(void);

#endif
```

Alteração em CMakeLists.txt:  
```c
# Add executable. Default name is the project name, version 0.1
add_executable( synth_audio
        main2.c
        src/adc_reader.c
        src/adc_buffer.c
)
```

### Notas da implementação

- Agora, com o uso do buffer, pode-se numerar as amostras do sinal captado. 

### Resultado obtido

Monitor Serial:  
---- Opened the serial port COM4 ----  
Iniciando aquisição de 2 segundos de áudio...  
Amostras capturadas:  
Amostra     0: 2036  
Amostra     1: 2016  
Amostra     2: 2021  
Amostra     3: 2013  
Amostra     4: 2029  
Amostra     5: 2041  
...  
Amostra 15994: 2010  
Amostra 15995: 2019  
Amostra 15996: 2029  
Amostra 15997: 2021  
Amostra 15998: 2014  
Amostra 15999: 2037  

---

3. Configuração da taxa de amostragem  

Dentro do loop, modifique o valor do delay entre os registros sequenciais, até atingir uma taxa de amostragem apropriada para uma gravação (recomendam-se frequências de 8 kHz, 11 kHz, 16 kHz ou 22 kHz, a depender da qualidade desejada).  

Solução: A memória RAM do RP2040 (264 KB) comporta bem mais dados do que estão sendo capturados. Então, altera-se a frequência de amostragem para 20 KHz e serão capturadas 40000 amostras nos 2 segundos de gravação. Como cada amostra tem 12 bits, ela é armazenada em 2 bytes (uibt16_t) e são necessários, portanto, 80 KB de memória. O intervalo entre as amostragens cai para 50 us. Isso implica em alterar o tamsnho do buffer em adc_buffer.h para 40000 e o intervalo entre amostragens no novo main3.c.  

### Código

Alterações em adc_buffer.h:  
```c
#define SAMPLE_RATE_HZ 20000
#define RECORD_DURATION_S  2
```

Alteração no programa principal que agora será main3.c:
```c
#define SAMPLE_INTERVAL_US 50  // 20 kHz
```

Alteração em CMakeLists.txt:  
```c
# Add executable. Default name is the project name, version 0.1
add_executable( synth_audio
        main3.c
        src/adc_reader.c
        src/adc_buffer.c
)
```

### Notas da implementação

- Agora, tem-se 40000 amostras capturadas. 

### Resultado obtido

Monitor Serial:  
---- Opened the serial port COM4 ----  
Iniciando aquisição de 2 segundos de áudio...  
Amostras capturadas:  
Amostra     0: 2022  
Amostra     1: 2027  
Amostra     2: 2028  
Amostra     3: 2008  
Amostra     4: 2016  
Amostra     5: 2032  
...  
Amostra 39994: 2026  
Amostra 39995: 2019  
Amostra 39996: 2032  
Amostra 39997: 2004  
Amostra 39998: 2034  
Amostra 39999: 2015  

---

4. Configuração do período de gravação  

Modifique as especificações do buffer para que ele armazene a quantidade adequada de valores (para a taxa de amostragem escolhida) para um áudio de duração pré-definida.  
Para decidir as especificações, tenha em mente o tamanho de cada amostra de áudio (em bytes), a taxa de amostragem (em bytes/s), a duração desejada (em s) e o espaço disponível na memória.  

Solução já implementada no item anterior.  

Situação atual do projeto:  

|Parâmetro                        | Valor atual                        |
|---------------------------------|------------------------------------|
|Taxa de amostragem               | 20.000 amostras por segundo        |
|Duração da gravação              | 2 segundos                         |
|Total de amostras                | 40.000                             |
|Tamanho de cada amostra          | 2 bytes (uint16_t)                 |
|Tamanho total do buffer          | 40.000 × 2 = 80.000 bytes (≈78 KB) |

---

5. Manipulação do sinal do PWM  

Desenvolva uma função que envie os sinais (gravados no buffer) ao PWM (dos Buzzers). Faça os ajustes necessários para que os valores armazenados sejam convertidos em sinais audíveis.  

Solução: Neste item, implementa-se a reprodução do sinal de áudio capturado previamente (armazenado no buffer) usando um dos buzzers conectados ao GPIO 10 e 21, controlado via PWM a 100 kHz. Para isso, criou-se um novo módulo de nome pwm_output.  

### Código

pwm_output.c:
```c
#include "pwm_output.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// Configurações do PWM
#define BUZZER_GPIO 10              // GPIO conectado ao buzzer
#define PWM_FREQ 20000              // Frequência do PWM: 20 kHz
#define PWM_WRAP 4096               // Resolução: 12 bits (0–4095)

static uint pwm_slice;

void pwm_output_init(void) {
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(BUZZER_GPIO);

    // Calcula divisor de clock para obter a frequência desejada
    float clkdiv = (float)125000000 / (PWM_FREQ * PWM_WRAP);  // 125 MHz / (freq × wrap)
    pwm_set_clkdiv(pwm_slice, clkdiv);

    // Define a resolução do PWM (wrap value)
    pwm_set_wrap(pwm_slice, PWM_WRAP - 1);

    // Inicializa com duty cycle zero
    pwm_set_gpio_level(BUZZER_GPIO, 0);

    // Ativa o PWM
    pwm_set_enabled(pwm_slice, true);
}

void pwm_output_set_duty_from_adc(uint16_t adc_value) {
    if (adc_value > PWM_WRAP - 1) {
        adc_value = PWM_WRAP - 1;  // Garante que valor não exceda a resolução máxima
    }
    pwm_set_gpio_level(BUZZER_GPIO, adc_value);
}

void pwm_output_stop(void) {
    pwm_set_gpio_level(BUZZER_GPIO, 0);    // Duty cycle 0%
    pwm_set_enabled(pwm_slice, false);     // Desativa o PWM
}
```

pwm_output.h:
```c
#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <stdint.h>

void pwm_output_init(void);
void pwm_output_set_duty_from_adc(uint16_t adc_value);
void pwm_output_stop(void);

#endif
```

adc_buffer.c:
```c
#include <stdio.h>
#include "adc_buffer.h"

static uint16_t buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

void adc_buffer_init(void) {
    buffer_index = 0;
}

bool adc_buffer_add_sample(uint16_t sample) {
    if (buffer_index < BUFFER_SIZE) {
        buffer[buffer_index++] = sample;
        return true;
    }
    return false;
}

bool adc_buffer_is_full(void) {
    return buffer_index >= BUFFER_SIZE;
}

void adc_buffer_print(void) {
    for (size_t i = 0; i < buffer_index; ++i) {
        printf("Amostra %5zu: %u\n", i, buffer[i]);
    }
}

void adc_buffer_reset(void) { // O mesmo que adc_buffer_init(), usado para clareza do código no main
    buffer_index = 0;
}

uint16_t adc_buffer_get_next_sample(void) {
    if (buffer_index < BUFFER_SIZE) {
        return buffer[buffer_index++];
    } else {
        return 0;
    }
}
```

adc_buffer.h:
```c
#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_RATE_HZ     20000
#define RECORD_DURATION_S  2
#define BUFFER_SIZE (SAMPLE_RATE_HZ * RECORD_DURATION_S)

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
bool adc_buffer_is_full(void);
void adc_buffer_print(void);
void adc_buffer_reset(void);
uint16_t adc_buffer_get_next_sample(void);

#endif
```

main5.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();

    printf("Gravando 2 segundos de áudio...\n");

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_read_microphone();
        adc_buffer_add_sample(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }
    
    printf("Reproduzindo sinal...\n");

    adc_buffer_reset();

    while (!adc_buffer_is_full()) {
        uint16_t amostra = adc_buffer_get_next_sample();
        pwm_output_set_duty_from_adc(amostra);
        sleep_us(SAMPLE_INTERVAL_US);
    }

    pwm_output_stop();
    printf("Reprodução concluída.\n");

    return 0;
}
```

### Notas da implementação

O módulo pwm_output possui três funções principais:  

- pwm_output_init(): Inicializa o GPIO 10 como saída PWM com frequência de 20 kHz e resolução de 12 bits (wrap = 4096). O cálculo do divisor do clock foi ajustado manualmente para maior controle da frequência.  
- pwm_output_set_duty_from_adc(uint16_t adc_value): Recebe um valor do buffer (0–4095) e atualiza o ciclo de trabalho do PWM para gerar a forma de onda sonora correspondente.  
- pwm_output_stop(): Define o duty cycle como zero e desabilita o PWM, encerrando o som emitido.
A integração com o programa principal (main5.c) ocorre com após a aquisição de 2 segundos de áudio (como nos itens anteriores), o conteúdo do buffer é reproduzido via pwm_output_set_duty_from_adc() em tempo real. Usa-se sleep_us(50) para manter a taxa de 20 kHz, reproduzindo cada amostra no mesmo intervalo em que foi capturada.  
É importante observar que o valor do ADC vai de 0 a 4095 e o wrap do PWM também foi ajustado para 4095. O mapeamento entre o sinal e o ciclo de trabalho é direto e linear, facilitando a reprodução fiel da forma de onda.  

O módulo adc_buffer.c foi alterado nessa etapa do projeto para acomodar as seguintes funções:  

adc_buffer_get_next_sample()  
Permite percorrer sequencialmente os valores armazenados, independentemente do modo de aquisição. Utiliza uma variável estática de índice interno (playback_index) para leitura contínua do buffer, útil na reprodução com PWM.  
adc_buffer_reset()  
Zera os índices de gravação e reprodução (buffer_index e playback_index), preparando o buffer para uma nova aquisição ou reprodução.  
Embora adc_buffer_init() e adc_buffer_reset() executem funções idênticas (zerar índices), manteve-se ambas por clareza semântica:  
adc_buffer_init() → usada na inicialização geral do sistema.  
adc_buffer_reset() → usada entre ciclos de regravação ou reprodução.  

### Resultado obtido

[Clique aqui e baixe o arquivo para ouvir a gravação](audio/captura.wav)

---

6. Reprodução completa do áudio armazenado

Modifique a função anterior para que ela reproduza o áudio, via PWM, dentro da frequência desejada (ajustada de acordo com a taxa de amostragem). Recomenda-se o uso de um contador, para garantir que todos os valores do buffer sejam devidamente enviados ao PWM.  
Teste essa função: Altere a função main() para que ela realize todas as operações (gravação, armazenamento e reprodução) em intervalos de tempo pré-programados. Ouça o áudio sendo reproduzido nos Buzzers e faça os ajustes necessários para garantir uma maior fidelidade à gravação original.  

Solução:  Implementa-se a lógica para:  
Grava 2 segundos de áudio com intervalo de 50 μs (20 kHz).  
Aguarda 1 segundo.  
Reproduz todo o conteúdo do buffer via PWM, também em 20 kHz.  
Aguarda mais 1 segundo.  
Repete indefinidamente.  



### Código

main6.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz
#define DELAY_BETWEEN_PHASES_MS 1000

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();

    while (true) {
        printf("Gravando 2 segundos de áudio...\n");
        adc_buffer_reset();
        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            uint16_t sample = adc_read_microphone();
            adc_buffer_add_sample(sample);
            sleep_us(SAMPLE_INTERVAL_US);
        }

        printf("Aguardando para reproduzir...\n");
        sleep_ms(DELAY_BETWEEN_PHASES_MS);

        printf("Reproduzindo sinal...\n");
        pwm_output_play_buffer();
        pwm_output_stop();
        printf("Reprodução concluída.\n");

        sleep_ms(DELAY_BETWEEN_PHASES_MS);
    }

    return 0;
}
```

adc_buffer.c:
```c
#include <stdio.h>
#include "adc_buffer.h"

static uint16_t buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

void adc_buffer_init(void) {
    buffer_index = 0;
}

bool adc_buffer_add_sample(uint16_t sample) {
    if (buffer_index < BUFFER_SIZE) {
        buffer[buffer_index++] = sample;
        return true;
    }
    return false;
}

void adc_buffer_reset(void) {
    buffer_index = 0;
}

void adc_buffer_print(void) {
    for (size_t i = 0; i < buffer_index; ++i) {
        printf("Amostra %5zu: %u\n", i, buffer[i]);
    }
}

uint16_t adc_buffer_get_sample(size_t index) {
    if (index < buffer_index) {
        return buffer[index];
    }
    return 0;
}
```

adc_buffer.h:
```c
#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 40000 // 2 s × 20 kHz

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
void adc_buffer_reset(void);
void adc_buffer_print(void);
uint16_t adc_buffer_get_sample(size_t index);

#endif
```

pwm_output.c:
```c
#include "pwm_output.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "adc_buffer.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

// Configurações do PWM
#define BUZZER_GPIO 10              // GPIO conectado ao buzzer
#define PWM_FREQ 20000              // Frequência do PWM: 20 kHz
#define PWM_WRAP 4096               // Resolução: 12 bits (0–4095)

static uint pwm_slice;

void pwm_output_init(void) {
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(BUZZER_GPIO);

    // Calcula divisor de clock para obter a frequência desejada
    float clkdiv = (float)125000000 / (PWM_FREQ * PWM_WRAP);  // 125 MHz / (freq × wrap)
    pwm_set_clkdiv(pwm_slice, clkdiv);

    // Define a resolução do PWM (wrap value)
    pwm_set_wrap(pwm_slice, PWM_WRAP - 1);

    // Inicializa com duty cycle zero
    pwm_set_gpio_level(BUZZER_GPIO, 0);

    // Ativa o PWM
    pwm_set_enabled(pwm_slice, true);
}

void pwm_output_set_duty_from_adc(uint16_t adc_value) {
    if (adc_value > PWM_WRAP - 1) {
        adc_value = PWM_WRAP - 1;  // Garante que valor não exceda a resolução máxima
    }
    pwm_set_gpio_level(BUZZER_GPIO, adc_value);
}

void pwm_output_stop(void) {
    pwm_set_gpio_level(BUZZER_GPIO, 0);    // Duty cycle 0%
}

void pwm_output_play_buffer(void) {
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        uint16_t sample = adc_buffer_get_sample(i);
        pwm_output_set_duty_from_adc(sample);
        sleep_us(SAMPLE_INTERVAL_US);
    }
}
```

pwm_output.h:
```c
#ifndef PWM_OUTPUT_H
#define PWM_OUTPUT_H

#include <stdint.h>

void pwm_output_init(void);
void pwm_output_set_duty_from_adc(uint16_t adc_value);
void pwm_output_stop(void);
void pwm_output_play_buffer(void);

#endif
```

### Notas da implementação

O início do loop infinito em main6.c resulta em um buffer contendo 40000 amostras que representam 2 segundo de áidio. Após uma espera de 1 segundo, chama a função pwm_output_play_buffer() que executa 40000 iterações lendo cada amostra do buffer com adc_buffer_get_sample(i). Cadea uma delas define o duty cycle do pwm com a função pwm_output_set_duty_from_adc(sample)a cada 50 us. Esse sinal modulado chega, atravéd de GPIO 10, ao buzzer. Após o loop, chama pwm_output_stop(), que zera o duty cycle, silenciando o buzzer. Após 1 segundo, tudo se repete.

### Resultado obtido

Gravações sucessivas semelhantes a do item 5.

---

7. Controle das funções com os botões

Modifique o fluxo de controle da função principal para que, em vez de as funções serem chamadas automaticamente, cada um dos dois botões controle uma função separada. Por exemplo: ao apertar o Botão A, a BitDogLab começa a gravar o áudio, e ao apertar o Botão B (após o fim da gravação), a BitDogLab reproduz o áudio.

Solução: Adicionam-se os arquivos button.c e button.h para controle dos botões com debounce.

### Código

button.c:
```c
#include "button.h"
#include "pico/stdlib.h"

#define BUTTON_A_GPIO 5 // Botão A na GPIO 5
#define BUTTON_B_GPIO 6 // Botão B na GPIO 6

static bool last_a_state = true; // Estado inicial: não pressionado (pull-up)
static bool last_b_state = true;

void button_init(void) {
    // Configura GPIOs como entrada com pull-up
    gpio_init(BUTTON_A_GPIO);
    gpio_set_dir(BUTTON_A_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_A_GPIO);

    gpio_init(BUTTON_B_GPIO);
    gpio_set_dir(BUTTON_B_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_B_GPIO);
}

bool button_a_pressed(void) {
    bool current_state = gpio_get(BUTTON_A_GPIO); // 0 = pressionado, 1 = solto
    bool pressed = last_a_state && !current_state; // Borda de descida
    last_a_state = current_state;
    return pressed;
}

bool button_b_pressed(void) {
    bool current_state = gpio_get(BUTTON_B_GPIO); // 0 = pressionado, 1 = solto
    bool pressed = last_b_state && !current_state; // Borda de descida
    last_b_state = current_state;
    return pressed;
}
```

button.h:
```c
#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 40000 // 2 s × 20 kHz

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
void adc_buffer_reset(void);
void adc_buffer_print(void);
uint16_t adc_buffer_get_sample(size_t index);
size_t adc_buffer_get_count(void); // Nova função para contar amostras

#endif
```

main7.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"
#include "button.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();
    button_init();

    bool is_recording = false;
    bool has_recorded = false;

    while (true) {
        // Detecta pressionamento do Botão A (gravação)
        if (button_a_pressed() && !is_recording) {
            printf("Botão A pressionado: Gravando 2 segundos de áudio...\n");
            adc_buffer_reset();
            is_recording = true;
            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                uint16_t sample = adc_read_microphone();
                adc_buffer_add_sample(sample);
                sleep_us(SAMPLE_INTERVAL_US);
            }
            is_recording = false;
            has_recorded = true;
            printf("Gravação concluída.\n");
        }

        // Detecta pressionamento do Botão B (reprodução)
        if (button_b_pressed() && !is_recording && has_recorded) {
            printf("Botão B pressionado: Reproduzindo sinal...\n");
            pwm_output_play_buffer();
            pwm_output_stop();
            printf("Reprodução concluída.\n");
        }

        sleep_us(10); // Pequeno delay para evitar sobrecarga
    }

    return 0;
}
```
adc_buffer.c:
```c
#include <stdio.h>
#include "adc_buffer.h"

static uint16_t buffer[BUFFER_SIZE];
static size_t buffer_index = 0;

void adc_buffer_init(void) {
    buffer_index = 0;
}

bool adc_buffer_add_sample(uint16_t sample) {
    if (buffer_index < BUFFER_SIZE) {
        buffer[buffer_index++] = sample;
        return true;
    }
    return false;
}

void adc_buffer_reset(void) {
    buffer_index = 0;
}

void adc_buffer_print(void) {
    for (size_t i = 0; i < buffer_index; ++i) {
        printf("Amostra %5zu: %u\n", i, buffer[i]);
    }
}

uint16_t adc_buffer_get_sample(size_t index) {
    if (index < buffer_index) {
        return buffer[index];
    }
    return 0;
}

size_t adc_buffer_get_count(void) {
    return buffer_index;
}
```

adc_buffer.h:
```c
#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 40000 // 2 s × 20 kHz

void adc_buffer_init(void);
bool adc_buffer_add_sample(uint16_t sample);
void adc_buffer_reset(void);
void adc_buffer_print(void);
uint16_t adc_buffer_get_sample(size_t index);
size_t adc_buffer_get_count(void); // Nova função para contar amostras

#endif
```

pwm_output.c:
```c
#include "pwm_output.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "adc_buffer.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

// Configurações do PWM
#define BUZZER_GPIO 10              // GPIO conectado ao buzzer
#define PWM_FREQ 20000              // Frequência do PWM: 20 kHz
#define PWM_WRAP 4096               // Resolução: 12 bits (0–4095)

static uint pwm_slice;

void pwm_output_init(void) {
    gpio_set_function(BUZZER_GPIO, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(BUZZER_GPIO);

    // Calcula divisor de clock para obter a frequência desejada
    float clkdiv = (float)125000000 / (PWM_FREQ * PWM_WRAP);  // 125 MHz / (freq × wrap)
    pwm_set_clkdiv(pwm_slice, clkdiv);

    // Define a resolução do PWM (wrap value)
    pwm_set_wrap(pwm_slice, PWM_WRAP - 1);

    // Inicializa com duty cycle zero
    pwm_set_gpio_level(BUZZER_GPIO, 0);

    // Ativa o PWM
    pwm_set_enabled(pwm_slice, true);
}

void pwm_output_set_duty_from_adc(uint16_t adc_value) {
    if (adc_value > PWM_WRAP - 1) {
        adc_value = PWM_WRAP - 1;  // Garante que valor não exceda a resolução máxima
    }
    pwm_set_gpio_level(BUZZER_GPIO, adc_value);
}

void pwm_output_stop(void) {
    pwm_set_gpio_level(BUZZER_GPIO, 0);    // Duty cycle 0%
}

void pwm_output_play_buffer(void) {
    size_t count = adc_buffer_get_count();
    for (size_t i = 0; i < count; ++i) {
        uint16_t sample = adc_buffer_get_sample(i);
        pwm_output_set_duty_from_adc(sample);
        sleep_us(SAMPLE_INTERVAL_US);
    }
}
```

Alterações em CMakeLISTS.txt:
```c
# Add executable. Default name is the project name, version 0.1
add_executable( synth_audio
        main7.c
        src/adc_reader.c
        src/adc_buffer.c
        src/pwm_output.c
        src/button.c
)
```

### Notas de implementação

A função adc_buffer_get_count() foi adicionada em adc_buffer.c para permitir que a reprodução em pwm_output_play_buffer() use apenas as amostras válidas (buffer_index) em vez de sempre iterar até BUFFER_SIZE. Isso é importante no fluxo controlado por botões, onde a gravação pode ser interrompida ou reiniciada, garantindo que apenas o áudio gravado seja reproduzido. As alterações em pwm_output.c ocorreram pelo mesmo motivo. Além disso, foi removida a linha pwm_set_enabled(pwm_slice, false);, pois desativar o PWM causava problemas nas reproduções subsequentes. Assim. apenas zera o duty cycle, mantendo o PWM ativo, o que é suficiente para silenciar o buzzer e permite as reproduções posteriores.  

### Resultado obtido

As gravações agora são controladas pelos botões e ficaram semelhantes a do item 5.

---

8. Retorno visual de ativação (com LEDs)

Adicione suporte ao LED RGB da BitDogLab – para feedback visual. Modifique as funções de gravação e reprodução para que, ao iniciar a gravação, a luz vermelha do LED acenda, e, ao iniciar a reprodução, a luz verde do LED acenda.  

Solução: Para atender ao item 8 do projeto audio_synthesizer, será preciso adicionar introduzir um novo módulo led.c e led.h para controlar o LED RGB e modificar main.c, que agora será main8.c.

### Código

led.c:
```c
#include "led.h"
#include "pico/stdlib.h"

#define LED_RED_GPIO 13   // Vermelho
#define LED_GREEN_GPIO 14 // Verde

void led_init(void) {
    // Configura GPIOs como saída
    gpio_init(LED_RED_GPIO);
    gpio_set_dir(LED_RED_GPIO, GPIO_OUT);
    gpio_put(LED_RED_GPIO, 0); // Desligado

    gpio_init(LED_GREEN_GPIO);
    gpio_set_dir(LED_GREEN_GPIO, GPIO_OUT);
    gpio_put(LED_GREEN_GPIO, 0); // Desligado
}

void led_set_red(bool state) {
    gpio_put(LED_RED_GPIO, state);
}

void led_set_green(bool state) {
    gpio_put(LED_GREEN_GPIO, state);
}

void led_off(void) {
    led_set_red(false);
    led_set_green(false);
}
```

led.h:
```c
#include <stdbool.h> // Inclui definições de bool, true, false

#ifndef LED_H
#define LED_H

void led_init(void);
void led_set_red(bool state);
void led_set_green(bool state);
void led_off(void);

#endif
```

main8.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"
#include "button.h"
#include "led.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();
    button_init();
    led_init();

    bool is_recording = false;
    bool has_recorded = false;

    while (true) {
        // Detecta pressionamento do Botão A (gravação)
        if (button_a_pressed() && !is_recording) {
            printf("Botão A pressionado: Gravando 2 segundos de áudio...\n");
            led_set_red(true); // Acende LED vermelho
            adc_buffer_reset();
            is_recording = true;
            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                uint16_t sample = adc_read_microphone();
                adc_buffer_add_sample(sample);
                sleep_us(SAMPLE_INTERVAL_US);
            }
            is_recording = false;
            has_recorded = true;
            led_off(); // Apaga LED
            printf("Gravação concluída.\n");
        }

        // Detecta pressionamento do Botão B (reprodução)
        if (button_b_pressed() && !is_recording && has_recorded) {
            printf("Botão B pressionado: Reproduzindo sinal...\n");
            led_set_green(true); // Acende LED verde
            pwm_output_play_buffer();
            pwm_output_stop();
            led_off(); // Apaga LED
            printf("Reprodução concluída.\n");
        }

        sleep_us(10); // Pequeno delay para evitar sobrecarga
    }

    return 0;
}
```

Alterações em CMakeLists.txt:
```c
add_executable( synth_audio
        main8.c
        src/adc_reader.c
        src/adc_buffer.c
        src/pwm_output.c
        src/button.c
        src/led.c
)
```

### Notas de implementação

Não há muito o que dizer. LED RGB uncluído no projeto.  

### Resultado obtido

Agora, as gravações ocorrem com o led vermelho aceso e as reproduções, com o verde.

---

9. Visualização da forma de onda no Display OLED

Desenvolva uma função que, para cada amostra do áudio, desenha uma coluna de pixels no display OLED – cuja altura varia de acordo com o valor detectado pelo ADC.

Solução: Implementação do módulo audio_visualizer.c para cuidar da interface visual. Para cuidar da implementação do controle do display, usou-se o driver ssd1306.c.  

### Código

audio_visualizer.c:
```c
// audio_visualizer.c (apenas a função modificada)

#include "audio_visualizer.h"
#include "ssd1306.h"
#include "adc_buffer.h"
#include <stdlib.h> // Para malloc, free, abs
#include <string.h> // Para memset

// Dimensões do display
#define DISPLAY_WIDTH SSD1306_WIDTH
#define DISPLAY_HEIGHT SSD1306_HEIGHT

#define ADC_MAX_VALUE 4095 // Valor máximo de um ADC de 12 bits

// Fator de amplificação visual para a onda
#define WAVEFORM_AMPLIFICATION_FACTOR 3.0f 

// Define a altura da área para o texto
#define TEXT_AREA_HEIGHT 16 // Duas linhas de 8 pixels para o texto "Waveform:"

void audio_visualizer_init(void) {
    // Nenhuma mudança aqui
}

void audio_visualizer_draw_waveform(void) {
    ssd1306_clear(); // Limpa o buffer do display

    // Desenha o texto "Waveform:" no topo do display
    ssd1306_draw_string(0, 0, "FORMA DE ONDA:");

    size_t count = adc_buffer_get_count();

    // Define a área de desenho da forma de onda APÓS o texto
    int waveform_start_y = TEXT_AREA_HEIGHT; // Começa a desenhar a onda abaixo do texto
    int waveform_height = DISPLAY_HEIGHT - waveform_start_y; // Altura disponível para a onda

    // Calcula o passo horizontal para preencher a largura do display
    float samples_per_pixel = (float)count / DISPLAY_WIDTH;

    // Desenha uma linha central para referência na área da forma de onda
    ssd1306_draw_line(0, waveform_start_y + waveform_height / 2,
                      DISPLAY_WIDTH - 1, waveform_start_y + waveform_height / 2, true);

    // Ponto médio do ADC para centralizar o sinal
    float adc_mid_point = (float)ADC_MAX_VALUE / 2.0f;
    // Metade da altura disponível para a forma de onda no display
    float display_waveform_mid_point = (float)(waveform_height - 1) / 2.0f;
    // Escala para mapear a amplitude máxima (da metade do ADC) para metade da altura da forma de onda
    float scale_factor = display_waveform_mid_point / adc_mid_point;


    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
        size_t sample_index = (size_t)(x * samples_per_pixel);

        if (sample_index < count) {
            uint16_t adc_value = adc_buffer_get_sample(sample_index);

            // 1. Calcula o desvio do ponto médio do ADC
            float normalized_adc_value = (float)adc_value - adc_mid_point;

            // 2. Amplifica o desvio
            float amplified_value = normalized_adc_value * WAVEFORM_AMPLIFICATION_FACTOR;
            
            // 3. Mapeia o valor amplificado para a altura disponível da forma de onda, centralizado
            int y_pixel_relative = (int)(amplified_value * scale_factor + display_waveform_mid_point);

            // Adiciona o offset vertical para desenhar abaixo do texto
            int y_pixel = waveform_start_y + y_pixel_relative;

            // Garante que o pixel esteja dentro dos limites da área de forma de onda (e do display)
            if (y_pixel < waveform_start_y) y_pixel = waveform_start_y;
            if (y_pixel >= DISPLAY_HEIGHT) y_pixel = DISPLAY_HEIGHT - 1;

            // Desenha o pixel
            ssd1306_draw_pixel(x, y_pixel, true);
        }
    }
    ssd1306_show(); // Envia o framebuffer para o display
}
```

audio_visualizer.h:
```c
#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa o visualizador de áudio.
 * Deve ser chamado após a inicialização do SSD1306.
 */
void audio_visualizer_init(void);

/**
 * @brief Desenha o sinal de áudio gravado no display OLED.
 * Os dados são lidos do adc_buffer.
 */
void audio_visualizer_draw_waveform(void);

#endif
```

Main9.c:
```c
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "adc_reader.h"
#include "adc_buffer.h"
#include "pwm_output.h"
#include "button.h"
#include "led.h"
#include "oled/ssd1306.h"
#include "audio_visualizer.h"

#define SAMPLE_INTERVAL_US 50  // 20 kHz

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o terminal

    adc_reader_init();
    adc_buffer_init();
    pwm_output_init();
    button_init();
    led_init();
    ssd1306_init();
    audio_visualizer_init();

    bool is_recording = false;
    bool has_recorded = false;

    ssd1306_clear();
    ssd1306_draw_string(0, 0, "GRAVADOR PRONTO");
    ssd1306_draw_string(0, 8, "PRESSIONE A");
    ssd1306_show();
    sleep_ms(1000);

    while (true) {
        // Detecta pressionamento do Botão A (gravação)
        if (button_a_pressed() && !is_recording) {
            printf("Botão A pressionado: Gravando 2 segundos de áudio...\n");
            led_set_red(true); // Acende LED vermelho
            adc_buffer_reset();
            is_recording = true;
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "GRAVANDO");
            ssd1306_show();

            for (size_t i = 0; i < BUFFER_SIZE; ++i) {
                uint16_t sample = adc_read_microphone();
                adc_buffer_add_sample(sample);
                sleep_us(SAMPLE_INTERVAL_US);
            }
            is_recording = false;
            has_recorded = true;
            led_off(); // Apaga LED
            printf("Gravação concluída.\n");

            // Após gravar, visualize o sinal no OLED
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "FORMA DE ONDA:");
            audio_visualizer_draw_waveform(); // Chama a função para desenhar a forma de onda
            sleep_ms(3000); // Exibe a forma de onda por alguns segundos
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "PRESIONE B");
            ssd1306_show();
        }

        // Detecta pressionamento do Botão B (reprodução)
        if (button_b_pressed() && !is_recording && has_recorded) {
            printf("Botão B pressionado: Reproduzindo sinal...\n");
            led_set_green(true); // Acende LED verde
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "TOCANDO...");
            ssd1306_show();
            pwm_output_play_buffer();
            pwm_output_stop();
            led_off(); // Apaga LED
            printf("Reprodução concluída.\n");
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "FEITO");
            ssd1306_show();
            sleep_ms(1000);
            ssd1306_clear();
            ssd1306_draw_string(0, 0, "PRESSIONE A");
            ssd1306_show();
        }

        sleep_us(10); // Pequeno delay para evitar sobrecarga
    }
    return 0;
}
```

SSD1306.c:
```c
// ssd1306.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT  64
#define SSD1306_I2C i2c1
#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_SDA_PIN 14
#define SSD1306_SCL_PIN 15
#define SSD1306_BUF_LEN (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t framebuffer[SSD1306_BUF_LEN];

// Fonte 8x8 - bit 7 = pixel mais à esquerda
static const uint8_t font8x8_basic[128][8] = {
    [' '] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    ['!'] = {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},
    ['"'] = {0x6C,0x6C,0x48,0x00,0x00,0x00,0x00,0x00},
    ['#'] = {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00},
    ['$'] = {0x10,0x7E,0x90,0x7C,0x12,0xFC,0x10,0x00},
    ['%'] = {0xC2,0xC6,0x0C,0x18,0x30,0x66,0xC6,0x00},
    ['&'] = {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00},
    ['\''] = {0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00},
    ['('] = {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00},
    [')'] = {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    ['*'] = {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},
    ['+'] = {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00},
    [','] = {0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00},
    ['-'] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00},
    ['.'] = {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00},
    ['/'] = {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},
    ['0'] = {0x3C,0x66,0xCE,0xD6,0xE6,0x66,0x3C,0x00},
    ['1'] = {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00},
    ['2'] = {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00},
    ['3'] = {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00},
    ['4'] = {0x0C,0x1C,0x3C,0x6C,0xFE,0x0C,0x0C,0x00},
    ['5'] = {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00},
    ['6'] = {0x3C,0x66,0x60,0x7C,0x66,0x66,0x3C,0x00},
    ['7'] = {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00},
    ['8'] = {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00},
    ['9'] = {0x3C,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00},
    ['A'] = {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00},
    ['B'] = {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00},
    ['C'] = {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00},
    ['D'] = {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00},
    ['E'] = {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00},
    ['F'] = {0x7F,0x40,0x40,0x7F,0x40,0x40,0x40,0x00},
    ['G'] = {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00},
    ['H'] = {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00},
    ['I'] = {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00},
    ['J'] = {0x0E,0x06,0x06,0x06,0x66,0x66,0x3C,0x00},
    ['K'] = {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00},
    ['L'] = {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00},
    ['M'] = {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00},
    ['N'] = {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00},
    ['O'] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['P'] = {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},
    ['Q'] = {0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00},
    ['R'] = {0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00},
    ['S'] = {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00},
    ['T'] = {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00},
    ['U'] = {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00},
    ['V'] = {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00},
    ['W'] = {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    ['X'] = {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00},
    ['Y'] = {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00},
    ['Z'] = {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00},
    [' '] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

// Envia um comando de controle ao display
static void ssd1306_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(SSD1306_I2C, SSD1306_I2C_ADDR, buf, 2, false);
}

// Envia os dados aos display. É chamada pela função ssd1306_show()
static void ssd1306_write_data(const uint8_t *data, size_t len) {
    uint8_t *buf = malloc(len + 1);
    buf[0] = 0x40;
    memcpy(&buf[1], data, len);
    i2c_write_blocking(SSD1306_I2C, SSD1306_I2C_ADDR, buf, len + 1, false);
    free(buf);
}
// Inicialização do display
void ssd1306_init(void) {
    i2c_init(SSD1306_I2C, 400 * 1000);
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCL_PIN);
    sleep_ms(100);

    const uint8_t init_cmds[] = {
        0xAE,       // Display OFF
        0x20, 0x00, // Set Memory Addressing Mode = Horizontal
        0x40,       // Set Display Start Line = 0
        0xA1,       // Set Segment Re-map (espelha horizontalmente)
        0xA8, 0x3F, // Set Multiplex Ratio = 63 (para 64 linhas)
        0xC8,       // COM Output Scan Direction remap (espelha vertical)
        0xD3, 0x00, // Set Display Offset = 0
        0xDA, 0x12, // Set COM Pins hardware configuration
        0x81, 0x7F, // Set Contrast Control = 0x7F
        0xA4,       // Resume to RAM content (normal display mode)
        0xA6,       // Set Normal Display (não invertido)
        0xD5, 0x80, // Set Display Clock Divide Ratio/Oscillator Frequency
        0x8D, 0x14, // Charge Pump = Enable
        0xDB, 0x20, // Set VCOMH Deselect Level
        0xD9, 0xF1, // Set Pre-charge Period
        0xAF        // Display ON
};
    for (uint8_t i = 0; i < sizeof(init_cmds); i++) {
        ssd1306_write_cmd(init_cmds[i]);
    }
    ssd1306_clear();
    ssd1306_show();
}

void ssd1306_clear(void) {
    memset(framebuffer, 0, sizeof(framebuffer));
}

// Desenha um único pinel nas coordenadas (x,y)
void ssd1306_draw_pixel(int x, int y, bool on) {
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT) return;
    int byte_index = (y / 8) * SSD1306_WIDTH + x;
    if (on)
        framebuffer[byte_index] |= (1 << (y % 8));
    else
        framebuffer[byte_index] &= ~(1 << (y % 8));
}

// Desenha uma linha de (x0,y0) até (x1,y1)
void ssd1306_draw_line(int x0, int y0, int x1, int y1, bool on) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (true) {
        ssd1306_draw_pixel(x0, y0, on);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// 
void ssd1306_draw_char(int x, int y, char c) {
    const uint8_t *bitmap = font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = bitmap[row];
        for (int col = 0; col < 8; col++) {
            // bit mais significativo (7 - col) representa pixel mais à esquerda
            bool on = (bits & (1 << (7 - col))) != 0;
            ssd1306_draw_pixel(x + col, y + row, on);
        }
    }
}

// Escreve uma sequência de caracteres no display
void ssd1306_draw_string(int x, int y, const char *str) {
    while (*str) {
        ssd1306_draw_char(x, y, *str++);
        x += 8;
    }
}

// Renderiza os dados do framebuffer para o dispaly
void ssd1306_show(void) {
    for (uint8_t page = 0; page < SSD1306_HEIGHT / 8; page++) {
        ssd1306_write_cmd(0xB0 + page);
        ssd1306_write_cmd(0x00);
        ssd1306_write_cmd(0x10);
        ssd1306_write_data(&framebuffer[page * SSD1306_WIDTH], SSD1306_WIDTH);
    }
}

// Faz um pixel riscar uma linha no meio do display em 2 segundos
void ssd1306_walk_horizontal_pixel(void) {
    ssd1306_clear();
    int y = SSD1306_HEIGHT / 2; // linha central vertical (y = 32)
    const int delay_ms = 2000 / SSD1306_WIDTH; // ≈ 15.6ms por passo

    for (int x = 0; x < SSD1306_WIDTH; x++) {
        ssd1306_draw_pixel(x, y, true);
        ssd1306_show();
        sleep_ms(delay_ms);
    }
}
```

ssd1306.h:
```c
// ssd1306.h
#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

#define SSD1306_WIDTH   128
#define SSD1306_HEIGHT  64
#define SSD1306_I2C i2c1
#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_SDA_PIN 14
#define SSD1306_SCL_PIN 15

// SSD1306 command definitions
#define SSD1306_DISPLAY_OFF            0xAE
#define SSD1306_DISPLAY_ON             0xAF
#define SSD1306_SET_CONTRAST           0x81
#define SSD1306_ENTIRE_DISPLAY_RESUME  0xA4
#define SSD1306_ENTIRE_DISPLAY_ON      0xA5
#define SSD1306_NORMAL_DISPLAY         0xA6
#define SSD1306_INVERT_DISPLAY         0xA7
#define SSD1306_SET_DISPLAY_OFFSET     0xD3
#define SSD1306_SET_START_LINE         0x40
#define SSD1306_SET_SEGMENT_REMAP_0    0xA0
#define SSD1306_SET_SEGMENT_REMAP_1    0xA1
#define SSD1306_SET_COM_SCAN_INC       0xC0
#define SSD1306_SET_COM_SCAN_DEC       0xC8
#define SSD1306_SET_MULTIPLEX_RATIO    0xA8
#define SSD1306_SET_DISPLAY_CLOCK_DIV  0xD5
#define SSD1306_SET_PRECHARGE          0xD9
#define SSD1306_SET_VCOMH_DESELECT     0xDB
#define SSD1306_SET_CHARGE_PUMP        0x8D
#define SSD1306_MEMORY_MODE            0x20
#define SSD1306_COLUMN_ADDR            0x21
#define SSD1306_PAGE_ADDR              0x22

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_show(void);

void ssd1306_draw_pixel(int x, int y, bool on);
void ssd1306_draw_line(int x0, int y0, int x1, int y1, bool on);
void ssd1306_draw_char(int x, int y, char c);
void ssd1306_draw_string(int x, int y, const char *str);

void ssd1306_walk_horizontal_pixel(void);

#endif
```

Alterações no CMakeLists.txt:
```c
add_executable( synth_audio
        main9.c
        src/adc_reader.c
        src/adc_buffer.c
        src/pwm_output.c
        src/button.c
        src/led.c
        src/audio_visualizer.c
        oled/ssd1306.c
)
```

### Notas de implementação

Utilizando o módulo ssd1306.c para controle do display e o adc_buffer.h para acesso às amostras de áudio, foi desenvolvido um novo módulo, audio_visualizer.c. Este módulo é responsável por mapear os valores brutos do ADC para coordenadas de pixel no display, desenhando a forma de onda. A amplitude da onda é ajustável, permitindo a representação visual do sinal capturado, mesmo em condições de baixo volume.  

### Resultado obtido

Além da reprodução do audio gravado, o led e o display oled oferecem um feedback visual.  

---

## Reflexão Final

- Quais técnicas de programação podemos usar para melhorar a gravação e a reprodução do áudio?

    Apesar de a pergunta focar especificamente em técnicas de programação, não se pode deixar de mencionar algumas considerações de Hardware:  
    - LM386: Foi utilizado um amplificador LM386 para amplificar o sinal de saída do PWM, redirecionado do GPIO 10 (buzzer) para o GPIO 16 (conector da placa), antes de enviá-lo a um fone de ouvido, melhorando o volume e a qualidade do áudio reproduzido.  
    - Filtros: Foi utulizado, nessa configuração, um filtro passa-baixa RC (resistor de 1 kΩ em série e capacitor de 15 nF para o GND) no sinal de saída para remover ruído de alta frequência (acima de 10 kHz) e um filtro RC (resistor de 100 kΩ em série com capacitor de 10 µF) para remover componente DC na entrada do LM386. Essa aplicação resultando em uma reprodução razoável em termos de potência sonora e ruído.  

    Em termos de software, é possível implementar:  
    - Interrupções por Timer: Usar temporizadores para disparar leituras do ADC e atualizações do PWM a 20 kHz, assegurando precisão na taxa de amostragem.
    - DMA: Implementar transferência direta de memória para mover dados do ADC ao buffer e do buffer ao PWM, reduzindo a carga da CPU e aumentando a quantidade de áudio gravado.
    - Buffer Circular: Adotar um buffer circular para gravação contínua, substituindo amostras antigas e evitando resets manuais. Isto melhora a performance do processamento.
    - Oversampling e Filtros Digitais: Ler múltiplas amostras e aplicar média móvel ou filtros passa-baixas digitais para reduzir ruído.
    - Multicore: Dividir tarefas (ex.: áudio no núcleo 0, OLED no núcleo 1) para melhorar desempenho e responsividade.
    - Normalização do Sinal: Ajustar dinamicamente o sinal do ADC para o intervalo do PWM, evitando clipping.  

    Uma palavra a respeito de DSP (Digital Signal Processing):  
    - O DSP pode ser explorado para processar o áudio digitalmente, como aplicar filtros FIR/IIR, equalização ou compressão, otimizando a qualidade em tempo real. Isso exige algoritmos avançados e é necessário um estudo aprofundado para verificar a eficiência no microcontrolador.  

    Referências
        - Livro (PDF): Ünsalan, C., Yücel, M., & Gürhan, H. D. (2018). *Digital Signal Processing using Arm Cortex-M based Microcontrollers: Theory and Practice*. Arm Education Media, ISBN: 978-1-911531-15-9, seção sobre técnicas de amostragem e filtros digitais.  

- Como é possível gravar áudios mais extensos, sem prejudicar a qualidade da gravação?

    Para gravar áudios mais extensosé preciso aumentar a capacidade de armazenamento e/ou otimizar o uso da memória, talvez aumentando a taxa de amostragem e usando técnicas de compressão de dados, como no formato MP3.  
    Para tanto, pode-se usar o DMA (direct memory access) inclusive com o uso de memória externa.  
    Outra alternativa é utilizar outro dispositivo ou provedor de nuvem para uma aboprdagem do tipo streaming.  


---

## 📜 Licença

GNU GPL-3.0.