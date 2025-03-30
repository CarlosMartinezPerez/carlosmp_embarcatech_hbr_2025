// ==========  AFINADOR ELETRÔNICO NA BITDOGLAB  ==========

//  ---  Importação das bibliotecas necessárias  ---

// As bibliotecas incluem funções essenciais para manipulação de hardware,
// como GPIOs, ADC, PWM, comunicação I2C e cálculos matemáticos.
#include <stdint.h>             // Define tipos como uint8_t, int16_t.
#include <stdio.h>              // Necessária para printf() e sprintf().
#include <math.h>               //  Necessária para fabs() e cálculos matemáticos.
#include "pico/stdlib.h"        // Gerenciamento do tempo (sleep_ms()), entrada e saída.
#include "hardware/gpio.h"      // Controle dos pinos GPIO (LEDs e botões).
#include "hardware/adc.h"       // Leitura do microfone via ADC.
#include "hardware/pwm.h"       // Usado para controle do buzzer.
#include "hardware/clocks.h"    // Definição da frequência do PWM (buzzer).
#include <string.h>             // Usado para memset() e sprintf().
#include <hardware/i2c.h>       // Comunicação com o display OLED (SSD1306).
#include "inc/ssd1306.h"        // Biblioteca do display OLED SSD1306.
#include "yin.h"                // Algoritmo para detecção da frequência fundamental da onda.

// --- Definição de constantes e pinos utilizados para controlar o afinador ---

// Este bloco contém as definições dos pinos usados pelos botões A e B,
// pelo buzzer, pelo I2C e pelos LEDs, além dos índices das notas e parâmetros 
// de configuração para a leitura do microfone e a tolerância da comparação de frequências.
#define BOTAO_DESCER_NOTA 5   // Pino do botão A - usado para diminuir a frequência alvo.
#define BOTAO_SUBIR_NOTA 6   // Pino do botão B - usado para aumentar a frequência alvo.
#define PINO_BUZZER 21          // Pino do buzzer - usado para emitir sons de feedback durante a operação.
#define I2C_SDA 14              // Pino para a linha de dados do protocolo I2C.
#define I2C_SCL 15              // Pino para a linha de clock do protocolo I2C.
#define LED_VERMELHO 13              // Pino do LED vermelho, que indicará que a frequência está abaixo do alvo.
#define LED_VERDE 11            // Pino do LED verde, que indicará que a frequência está correta (afinada).
#define LED_AZUL 12             // Pino do LED azul, que indicará que a frequência está acima do alvo.

#define INDICE_E2 0     // Índice da nota E2 (física) na tabela de frequências.
#define INDICE_A2 1     // Índice da nota A2 na tabela de frequências.
#define INDICE_D3 2     // Índice da nota D3 na tabela de frequências.
#define INDICE_G3 3     // Índice da nota G3 na tabela de frequências.
#define INDICE_B3 4     // Índice da nota B3 na tabela de frequências.
#define INDICE_E4 5     // Índice da nota E4 na tabela de frequências.

#define AMOSTRAS 256            // Número de amostras a serem coletadas para análise da frequência.
#define FREQUENCIA_AMOSTRAGEM 4096 // Frequência de amostragem para capturar os sinais (4096 Hz).
#define ADC_MICROFONE 2              // Canal ADC usado para leitura do sinal do microfone.
#define PINO_MICROFONE 28        // Pino GPIO do microfone.
#define TOLERANCIA 2.0          // Tolerância permitida para variação da frequência alvo (3 Hz).

// --- Definição de variáveis para frequências, notas e parâmetros do microfone ---

// Este bloco contém as definições de variáveis para as frequências das notas, suas cifras 
// correspondentes e relacionadas à amostragem do sinal do microfone.
float frequencias[] = { 82.4, 110.0, 146.8, 196.0, 246.9, 329.6 }; // Frequências das notas, em Hz.
char* notas[] = { "E2", "A2", "D3", "G3", "B3", "E4" }; // Nomes das notas (cifras e oitavas). 
uint16_t periodoAmostragem; // Variável que define o período de amostragem para o microfone.
uint8_t indiceNotaAtual = 0; // Índice da nota alvo. Inicialmente, E2 (com índice 0).
float valoresAmostrados[AMOSTRAS]; // Array que armazena os valores das amostras de áudio capturadas pelo microfone.

//     ---  Funções para a inicialização do hardware  ---

// Função para inicialização do pino do microfone (28).
void inicializar_adc_microfone(void) {
    adc_gpio_init(PINO_MICROFONE);      // Inicializa o pino 28.
    adc_select_input(ADC_MICROFONE);        // Seleciona o canal ADC associado ao microfone.
}

// Função para inicializar o buzzer com PWM.
void inicializar_pwm_buzzer(uint pino) {
    gpio_set_function(pino, GPIO_FUNC_PWM); // Configura o pino como saída de PWM.
    uint slice_num = pwm_gpio_to_slice_num(pino); // Obtém o número do slice PWM do pino usado.

    // Configurar o PWM com frequência desejada (1000 Hz).
    pwm_config config = pwm_get_default_config(); // Seleciona e armazena a configuração básica de PWM.
    float clkdiv = (float)clock_get_hz(clk_sys) / (1000.0 * 4096.0);
    pwm_config_set_clkdiv(&config, clkdiv); // Configura o divisor de clock ára o PWM.
    // O divisor do clock do sistema, multiplicado por um número entre 0 e 4096 vai definir o duty cycle na 
    // frequência de PWM escolhida (1000 Hz) - 2048 define 50% de duty cycle (onda quadrada).
    pwm_init(slice_num, &config, true); // Inicializa o PWM do slice obtido com a configuração
    // especificada, O último argumento (true) indica que o PWM deve ser iniciado imediatamente.
        
    pwm_set_gpio_level(pino, 0); // Inicia o PWM no nível baixo, ou seja, buzzer desligado.
}

// Função para inicializar os pinos dos botões A e B.
void inicializar_botoes(void) {
    gpio_init(BOTAO_DESCER_NOTA); // Inicializa o botão A.
    gpio_init(BOTAO_SUBIR_NOTA); // Inicializa o botão B.
    gpio_set_dir(BOTAO_DESCER_NOTA, GPIO_IN); // Define como entrada.
    gpio_set_dir(BOTAO_SUBIR_NOTA, GPIO_IN); // Define como entrada.
    gpio_pull_up(BOTAO_DESCER_NOTA); // Pull up no pino do botão A.
    gpio_pull_up(BOTAO_SUBIR_NOTA); // Pull up no pino do botão B.
}

// Função para inicialização dos pinos do led RGB.
void inicializar_led_rgb(void) {
    gpio_init(LED_VERMELHO); // Inicializa pino 13.
    gpio_init(LED_VERDE); // Inicializa pino 11.
    gpio_init(LED_AZUL); // Inicializa pino 12.
    gpio_set_dir(LED_VERMELHO, GPIO_OUT); // Define pino 13 como saída.
    gpio_set_dir(LED_VERDE, GPIO_OUT); // Define pino 11 como saída.
    gpio_set_dir(LED_AZUL, GPIO_OUT); // Define pino 12 como saída.
}

// Inicialização dos pinos I2C para o oled display.
void inicializar_oled_i2c(void) {
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define pino 14 como I2C.
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define pino 15 como I2C.
    gpio_pull_up(I2C_SDA); // Pull up no pino 14.
    gpio_pull_up(I2C_SCL); // Pull up no pino 15.
}

//   ---  Funções da operação do hardware  ---

// Função para emitir um beep com a duração especificada.
void emitir_beep(uint pino, uint duracao_ms) {
    // Define o nível de duty cycle (50% para som audível).
    pwm_set_gpio_level(pino, 2048); // Faz o beep
    sleep_ms(duracao_ms); // Aguarda a duração especificada.
    pwm_set_gpio_level(pino, 0); // Desliga o beep.
    sleep_ms(50); // Aguarda 50 mili segundos.
}

// Função que alerta quando se atinge as notas extremas de afinação, emitindo dois 
// beeps seguidos, caso o usuário tentar uma nota além do limite.
void beep_limite_alcancado() {
    emitir_beep(PINO_BUZZER, 100); // Primeiro beep.
    sleep_ms(100); // Agrada.
    emitir_beep(PINO_BUZZER, 100); // Segundo beep.
}

// Função para aguarda o pressionamento e a liberação de um botão específico
// e evitando leituras indesejadas causadas por bounce mecânico. 
void aguardar_pressionamento_botao(uint pino_botao) {
    while (gpio_get(pino_botao)); // Aguarda até o botão específico ser pressionado (nível lógico baixo).
    sleep_ms(20); // Delay para debounce.
    emitir_beep(PINO_BUZZER, 40);// Emite um breve beep para indicar o pressionamento.
    while (!gpio_get(pino_botao)); // Aguarda até que o botão seja solto (nível lógico alto).
    sleep_ms(20); // Delay para debounce.
}

// Função para coletar amostras do microfone e armazena os valores para análise de frequência
void amostrar_microfone(void) { 
    // Loop para coletar um número fixo de amostras definido por AMOSTRAS
    for (uint16_t i = 0; i < AMOSTRAS; i++) { 
        valoresAmostrados[i] = (float)adc_read() / 4096.0; // Lê um valor do conversor analógico-digital (ADC) e armazena em valoresAmostrados.
        // Como a conversão usa 12 bits, a divisão por 4096 normaliza os valores entre 0 e 1.
        busy_wait_us(periodoAmostragem); // Aguarda o intervalo de tempo antes da próxima leitura.
        // Este intervalo de tempo, em micro segundos, é calculado com base na frequência de amostragem.
    }
}

// Função para informar, através do led RGB, se a nota está abaixo, igual ou acima da frequência correta.
void definir_estado_led_rgb(bool vermelho, bool verde, bool azul) {
    gpio_put(LED_VERMELHO, vermelho); // Define o estado do led vermelho.
    gpio_put(LED_VERDE, verde); // Define o estado do led verde.
    gpio_put(LED_AZUL, azul); // Define o estado do led azul.
}

// Função para escolher a frequência alvo com base nos botões de aumento (A) ou diminuição (B)  de notas.
void definirNotaAlvo(void) {
    // Verifica se o botão para diminuição de notas foi pressionado.
    if (!gpio_get(BOTAO_DESCER_NOTA)) {
        aguardar_pressionamento_botao(BOTAO_DESCER_NOTA); // Aguarda a liberação do botão.
        // Se ainda houver frequências menores disponíveis, decrementa o índice de notas.
        if (indiceNotaAtual > 0) {
            indiceNotaAtual--; // Decrementa o índice de notas.
        // Se já estiver na nota mais baixa (E2), emite 2 beeps.
        } else {
            beep_limite_alcancado(); // Chama a função que emite 2 beep de alerta.
        }
    }
    // Verifica se o botão para aumentar de notas foi pressionado.
    if (!gpio_get(BOTAO_SUBIR_NOTA)) {
        aguardar_pressionamento_botao(BOTAO_SUBIR_NOTA); // Aguarda a liberação do botão.
        // Se ainda houver frequências maiores disponíveis, incrementa o índice de notas.
        if (indiceNotaAtual < 5) {
            indiceNotaAtual++; // Incrementa o índice de notas.
        // Se já estiver na nota mais alta (E4), emite 2 beeps.
        } else {
            beep_limite_alcancado(); // Chama a função que emite 2 beep de alerta.
        }
    }
}

//   ---  PROGRAMA PRINCIPAL   ---

int main() {
    stdio_usb_init(); // Inicializa a comunicação USB padrão para entrada/saída de dados.
    adc_init(); // Inicializa o periférico ADC (Conversor Analógico-Digital) do microcontrolador.
    inicializar_adc_microfone(); // Chama a função para inicialização do pino do microfone (28).
    inicializar_oled_i2c(); // Chama a função que configura os pinos e a comunicação I2C para o display OLED.
    i2c_init(i2c1, ssd1306_i2c_clock * 1000); // Inicializa a comunicação I2C para o display OLED SSD1306.
    ssd1306_init();  // Configura o display OLED SSD1306 para operação.
    inicializar_led_rgb(); // Chama a função que inicializa os pinos do LED RGB.
    definir_estado_led_rgb(false, false, false); // Chama a função para apagar os 3 leds do led RGB.
    inicializar_pwm_buzzer(PINO_BUZZER); // Chama a função de inicialização do buzzer com PWM.
    inicializar_botoes(); // Chama a função que inicializa os pinos dos botões A e B.
    Yin yin; // Estrutura para armazenar os parâmetros do algoritmo Yin de detecção de frequência fundamental.
    periodoAmostragem = 1000000 / FREQUENCIA_AMOSTRAGEM; // Calcula o período de amostragem do microfone em microssegundos.
    Yin_init(&yin, AMOSTRAS, 0.23);  // Inicializa o algoritmo Yin com o número de amostras e o limiar de tolerância.

    // Configuração da área de renderização do display OLED SSD1306.
    struct render_area frame_area = {
        .start_column = 0,                  // Primeira coluna do display.
        .end_column = ssd1306_width - 1,    // Última coluna do display.
        .start_page = 0,                    // Primeira página (linha de 8 pixels).
        .end_page = ssd1306_n_pages - 1     // Última página (altura total do display).
    };
    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer necessário para a área definida.

    // Zera o display inteiro antes de atualizar.
    static uint8_t ssd[ssd1306_buffer_length]; // Buffer de dados para o display.
    memset(ssd, 0, ssd1306_buffer_length); // Preenche o buffer com zeros (apagando qualquer informação anterior).
    render_on_display(ssd, &frame_area); // Renderiza o buffer apagado no display (limpa a tela).

    int estadoAnterior = -1; // Armazena o último estado para evitar atualizações desnecessárias do display ou LEDs.

    //    ---  LOOP INFINITO  ---
    while (1) {
        definirNotaAlvo(); // Chama a função que define a nota alvo com base no índice atual.
        float frequenciaAlvo = frequencias[indiceNotaAtual]; // Obtêm a frequência da nota alvo.
    
        // Limpa o buffer do display antes de redesenhar os textos.
        memset(ssd, 0, ssd1306_buffer_length);
    
        // Exibe a mensagem inicial.
        ssd1306_draw_string(ssd, 5, 0, "  AFINADOR NA  "); // Texto centralizado horizontalmente.
        ssd1306_draw_string(ssd, 5, 15, "   BITDOGLAB   "); // Segunda linha da mensagem inicial.
    
        // Exibe a nota alvo, pulando uma linha e deslocando 5 caracteres à direita.
        char textoNota[16];
        sprintf(textoNota, "    Nota: %s ", notas[indiceNotaAtual]); // Formata a string com a nota atual.
        ssd1306_draw_string(ssd, 5, 34, textoNota); // Linha 34 para pular uma linha.
    
        amostrar_microfone(); // Captura um novo conjunto de amostras do microfone.
        float frequenciaAtual = Yin_getPitch(&yin, valoresAmostrados); // Calcula a frequência fundamental 
        // do som captado pelo microfone usando o algoritmo Yin.
    
        int novoEstado = 0; // Variável para armazenar o novo estado do LED.
        char statusTexto[10] = "         "; // Inicializa variável de estado de afinação com espaços para limpar a área
    
        if (frequenciaAtual == -1) {
            // Sinal de áudio fraco ou ausente, desliga todos os LEDs.
            definir_estado_led_rgb(false, false, false);
        } else {
            // Calcula a diferença entre a frequência detectada e a frequência alvo.
            float diferencaFreq = frequenciaAtual - frequenciaAlvo;
            if (fabs(diferencaFreq) < TOLERANCIA) {
                novoEstado = 1; // Nota afinada.
                strcpy(statusTexto, " AFINADO "); // Define o texto para o display.
            } else if (frequenciaAtual > frequenciaAlvo) {
                novoEstado = 2; // Frequência acima da nota alvo.
                strcpy(statusTexto, "  ACIMA  "); // Define o texto para o display.
            } else {
                novoEstado = 3; // Frequência abaixo da nota alvo.
                strcpy(statusTexto, " ABAIXO  ");  // Define o texto para o display.
            }
        }
    
        ssd1306_draw_string(ssd, 32, 56, statusTexto); // Exibe o estado da afinação.
        render_on_display(ssd, &frame_area); // Atualiza o display com os novos textos.
    
        // Atualiza os LEDs apenas se o estado mudou para evitar mudanças desnecessárias.
        if (novoEstado != estadoAnterior) {
            bool vermelho = (novoEstado == 3); // LED vermelho acende se a nota estiver abaixo do alvo.
            bool verde = (novoEstado == 1); // LED verde acende se a nota estiver afinada.
            bool azul = (novoEstado == 2); // LED azul acende se a nota estiver acima do alvo.
    
            definir_estado_led_rgb(vermelho, verde, azul); // Atualiza os LEDs.
            estadoAnterior = novoEstado; // Armazena o novo estado.
        }
    
        // Adiciona um pequeno delay entre as leituras para evitar atualizações muito rápidas.
        sleep_ms(300);
    }
}