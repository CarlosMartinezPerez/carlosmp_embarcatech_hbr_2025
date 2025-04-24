# GALTON BOARD

Este projeto implementa uma simulação digital de um **Tabuleiro de Galton** (Galton Board) utilizando um microcontrolador Raspberry Pi Pico e um display OLED SSD1306 (128x64 pixels), todos presentes na placa BitDogLab. A simulação modela a queda de bolinhas através de fileiras de pinos, resultando em uma distribuição binomial, que é visualizada em um histograma no display e analisada via saída serial. A distribuição das bolinhas no histograma, para um número suficientemente grande de ensaios, se aproxima cada vez mais, da forma de uma curva gaussiana (ou normal, denotada por N(0,1)), conforme estabelece o Teorema do Limite Central.

## 1. O que é um Tabuleiro de Galton?

O Tabuleiro de Galton, também conhecido como Quincunx, é um dispositivo inventado por Sir Francis Galton no século XIX para demonstrar o teorema central do limite e a distribuição binomial. Consiste em uma placa vertical com fileiras de pinos dispostos em padrão triangular. Bolinhas são liberadas do topo e, ao colidirem com os pinos, desviam aleatoriamente para a esquerda ou direita com igual probabilidade (50%). Ao final, as bolinhas se acumulam em compartimentos (bins) na base, formando uma curva em forma de sino que se aproxima da forma da distribuição normal para um grande número de ensaios.

A beleza do Tabuleiro de Galton está em sua capacidade de ilustrar conceitos estatísticos fundamentais de forma visual e intuitiva, sendo amplamente utilizado em estudos de probabilidade, estatística e física. Um exemplo bastante ilustrativo dessa demonstração pode ser visto [neste vídeo](https://www.youtube.com/watch?v=L4RMRq1FUcg), em que um Tabuleiro de Galton é posto em ação, destacando a formação da curva em sino com milhares de bolinhas. O conceito envolvido é o fato de que pode-se obter um resultado previsível mesmo tratando com eventos completamente aleatórios.

## 2. A Simulação Digital (Digital Twin)

A simulação digital desenvolvida neste projeto recria o comportamento de um Tabuleiro de Galton em um display OLED de 128x64 pixels, funcionando como um "gêmeo digital" do dispositivo físico. A seguir, descrevem-se os detalhes da implementação:

### Estrutura da Simulação
- **Display e Bins**: Os 128 pixels horizontais do display são divididos em 16 bins, cada um com 8 pixels de largura (128 ÷ 16 = 8). Esses bins, localizados na base do display, acumulam as bolinhas após sua queda.
- **Pinos e Fileiras**: Acima dos bins, há 15 fileiras de pinos dispostos em um padrão triangular (15 pinos na primeira fileira, 14 na segunda, e assim por diante). Como o display tem 64 pixels de altura, as fileiras de pinos seriam teoricamente espaçadas por ~4,27 pixels (64 ÷ 15), mas isso é insuficiente para uma bolinha de ~6 pixels de diâmetro passar sem ficar presa. Assim, as fileiras superiores são consideradas fora da tela, e a simulação foca na trajetória das bolinhas.
- **Geometria dos Pinos**: Os pinos formam triângulos equiláteros com lados de 8 pixels. A distância vertical entre fileiras é calculada como a altura de um triângulo equilátero de lado 8, ou 4 vezes a raiz quadrada de 3, aproximadamente 6,93 (7 pixels), mas a simulação abstrai isso para manter a lógica simples.
- **Comportamento das Bolinhas**: Cada bolinha começa no centro (x = 64) e sofre 15 colisões, desviando aleatoriamente ±4 pixels (para a esquerda ou para a direita) por colisão, com 50% de chance para cada direção. Após 15 colisões, a bolinha cai em um dos 16 bins.
- **Casos Extremos**:
  - Máximo à direita: \( 64 + 15 x 4 = 124 \) (bin 16).
  - Máximo à esquerda: \( 64 - 15 x 4 = 4 \) (bin 1).
- **Visualização**: As bolinhas são exibidas como pontos no display durante a queda, e o histograma na base é desenhado com quadrados, onde cada 2 bolas incrementam 1 pixel de altura (escala ajustada para evitar preenchimento muito rápido da tela).
- **Saída Serial**: A cada 100 bolas, o programa exibe:
  - Total de bolas.
  - Quantidade de bolas em cada bin.
  - Média da distribuição.
  - Desvio padrão da distribuição.
- Controle de Probabilidades: Dois botões, conectados aos pinos GPIO 5 (botão A) e GPIO 6 (botão B), permitem ajustar dinamicamente as probabilidades de desvio das bolinhas. Inicialmente, a probabilidade é 50% para esquerda e 50% para direita. Cada pressão do botão A aumenta a probabilidade de desvio à esquerda em 10% (e reduz a direita em 10%), até um máximo de 90% esquerda / 10% direita. O botão B faz o oposto, aumentando a probabilidade de desvio à direita até 10% esquerda / 90% direita. As porcentagens atuais são exibidas nas laterais do display, permitindo visualizar o impacto dessas alterações na distribuição binomial, que se torna assimétrica conforme as probabilidades mudam.

### Características
- A simulação utiliza o gerador de números aleatórios do Pico (`get_rand_32`) para garantir desvios aleatórios.
- Até 10 bolinhas podem estar ativas simultaneamente, caindo em uma "chuva" contínua.
- A probabilidade teórica de uma bolinha cair nos bins extremos (1 ou 16) é 1 sobre 2 elevado a 15 vezes 100% ou 0,003%. Só existe uma maneira de uma bolinha atingir o bin 16, ela tem que desviar à direita nas 15 colisões. Já para chegar aos bins 8 ou 9 (centrais), a bolinha tem 6435 caminhos diferentes para fazer. O cálculo dessa probabilidade é dado pela combinação de 15, 7 a 7, vezes 1 sobre 2 elevado a 15 vezes 100% ou 6435 vezes 0,003%, ou aproximadamente 19.64\%. A diferença nessas probabilidades reflete bem a natureza binomial da simulação.

## 3. Análise do Código

### Modularização
O código é organizado em módulos para promover clareza, reusabilidade e manutenção. A modularização separa a lógica da simulação, a interface do display e a inicialização do sistema, oferecendo as seguintes vantagens:
- **Clareza**: Cada módulo tem uma responsabilidade única, facilitando a compreensão.
- **Manutenção**: Alterações em um módulo (ex.: ajustar a escala do histograma) não afetam outros.
- **Reusabilidade**: Funções como `random_direction` podem ser usadas em outros projetos.
- **Depuração**: Isolamento de erros em módulos específicos agiliza a correção.

### Estrutura do Código
O projeto é composto por seis arquivos principais:  
**galton.c / galton.h**:  
   - Contém a lógica da simulação do Tabuleiro de Galton.  
   - Funções principais:  
     - `init_ball`: Inicializa uma bolinha no centro (x = 64, y = 0).  
     - `update_ball`: Move a bolinha verticalmente e realiza desvios horizontais (±4 pixels) em 15 colisões.  
     - `register_ball_landing`: Registra a bolinha em um bin e atualiza o histograma.  
     - `calculate_statistics`: Calcula e exibe (a cada 100 bolas) o total de bolas, contagem por bin, média e desvio padrão.  
     - `test_randomness`: Testa a aleatoriedade do gerador (`random_direction`), útil para depuração.  
   - Variáveis globais: `histogram` (contagem de bolas por bin) e `total_balls` (total acumulado).  

**main.c**:  
   - Orquestra a simulação, inicializando o display, gerenciando até 10 bolinhas ativas e chamando `calculate_statistics` a cada 100 bolas.  
   - Loop principal: Atualiza bolinhas, registra quedas e refresca o display a cada 50ms.  

**display.c / display.h**:  
   - Gerencia o display OLED SSD1306.  
   - Funções principais:  
     - `init_display`: Configura o display.  
     - `draw_histogram`: Desenha o histograma com altura proporcional às bolas (escala: 1 pixel por 2 bolas).  
     - `update_display`: Atualiza o display com bolinhas e histograma.  

**CMakeLists.txt**:  
O arquivo CMakeLists.txt é usado para configurar o processo de compilação do projeto, definindo as instruções para o CMake, uma ferramenta de construção, especificando:  
   - O nome do projeto e a versão do CMake necessária.  
   - Os arquivos-fonte (ex.: main.c, galton.c, display.c) a serem compilados.  
   - As bibliotecas do Pico SDK (ex.: pico_stdlib, hardware_i2c) a serem vinculadas.  
   - Configurações como suporte a USB e UART para saída serial.  
   - Geração do executável para o Raspberry Pi Pico.  

### Detalhes Técnicos
- **Display**: O SSD1306 é controlado via I2C, com buffer para renderizar bolinhas (pixels individuais) e histograma (retângulos por bin).
- **Escala do Histograma**: Ajustada para `histogram[i] / 2` para crescimento lento, evitando preenchimento rápido da tela.
- **Saída Serial**: Via USB, configurada com `stdio_usb_init`, exibindo estatísticas a cada 100 bolas para análise.
- **Aleatoriedade**: O `get_rand_32` garante desvios uniformes (50% esquerda, 50% direita), validados pela função `test_randomness`.

### Resultados Esperados
A simulação produz uma distribuição binomial, com média próxima a 8.5 (centro dos 16 bins) e desvio padrão próximo a 2, refletindo as 15 colisões com probabilidade de 50%. Após 975 bolas, por exemplo, observou-se:
- Média: ~8.49
- Desvio Padrão: ~1.87
- Pico nos bins 8-9, com zero bolas nos extremos (bins 1 e 16), consistente com a probabilidade teórica de 0,003%.

### Como Executar
1. **Hardware**:
   - Raspberry Pi Pico (ou Pico W).
   - Display OLED SSD1306 (128x64, conectado via I2C).
2. **Configuração**:
   - Instale o Pico SDK (versão 2.1.1 ou superior).
   - Conecte o display aos pinos I2C do Pico (ex.: SDA ao GPIO 14, SCL ao GPIO 15).
3. **Compilação**:
   - Clone o repositório.
   - Execute `cmake` e `make` no diretório `build`.
   - Carregue o firmware no Pico via USB.
4. **Saída**:
   - Visualize a chuva de bolinhas e o histograma no display.
   - Conecte-se ao monitor serial (ex.: `minicom`, 115200 baud) para ver estatísticas a cada 100 bolas.

### Explicação do Código:

#### Orquestrador main.c:

**1. Includes:**

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "galton.h"
#include "display.h"
```

* `#include <stdio.h>`: Inclui a biblioteca padrão de entrada e saída do C. Isso fornece funções como `printf` para imprimir informações no monitor serial.
* `#include "pico/stdlib.h"`: Inclui a biblioteca padrão do SDK do Raspberry Pi Pico. Essa biblioteca oferece funções essenciais para interagir com o hardware do Pico, como inicialização de periféricos, controle de GPIO, e funcionalidades de tempo.
* `#include "galton.h"`: Inclui um arquivo de cabeçalho chamado `galton.h`. Este arquivo provavelmente contém declarações de tipos de dados (como a struct `Ball`) e protótipos de funções relacionadas à lógica da simulação da placa de Galton (como `init_ball` e `update_ball`).
* `#include "display.h"`: Inclui um arquivo de cabeçalho chamado `display.h`. Este arquivo provavelmente contém protótipos de funções responsáveis por inicializar e atualizar a visualização da simulação (como `init_display` e `update_display`). A ausência da matriz de LEDs sugere que a visualização pode ser feita no monitor serial ou em outro dispositivo de display conectado ao Pico.

**2. Definições:**  

```c
#define BUTTON_A 5
#define BUTTON_B 6
#define DEBOUNCE_MS 200
```

Este trecho define constantes necessárias para a interação com o hardware. As macros BUTTON_A e BUTTON_B especificam os pinos GPIO 5 e 6 do Raspberry Pi Pico, aos quais os botões físicos estão conectados. A constante DEBOUNCE_MS estabelece um intervalo de 200 milissegundos para o mecanismo de debounce, garantindo que apenas uma pressão de botão seja registrada por vez, mesmo em presença de ruídos elétricos típicos de botões mecânicos.  

**3. Teste da Aleatoriedade:**

```c
/*
int main() { // Função para teste de aleatoriedade durante o desenvolvimento
    stdio_init_all();
    sleep_ms(2000);  // Tempo para abrir o monitor serial
    test_randomness(100000); // Altere a quantidade de  testes
}*/
```

Como resposta a uma demanda específica do projeto, este trecho, presente em um bloco comentado no código, representa uma função main() alternativa utilizada no início do desenvolvimento para testar a qualidade do gerador de números aleatórios do projeto. A função inicializa a comunicação serial com stdio_init_all(), aguarda 2000 milissegundos (sleep_ms(2000)) para permitir a conexão com um monitor serial, e chama test_randomness(100000), que executa 100.000 iterações da função random_direction para verificar se as probabilidades de desvio à esquerda e à direita estão equilibradas (ou seja, aproximadamente 50% para cada lado com a configuração padrão). Os resultados são exibidos no monitor serial, mostrando a contagem e a porcentagem de desvios à esquerda e à direita. Embora comentado na versão final, esse teste foi importante para validar a aleatoriedade do sistema, garantindo que a distribuição binomial da simulação refletisse corretamente as probabilidades esperadas.  


Resultado dos teste de aleatoriedade realizados com as funções `test_randomness` e `random_direction` presentes em galton.c (explicado mais adiante):  
10 ensaios:  
Esquerda: 5 (50.00%), Direita: 5 (50.00%)  
Aqui, houve sorte. Geralmente, os resultados apontam 60% ou 70% para um dos lados, dado a pequena quantidade de ensaios.

100 ensaios:
Esquerda: 48 (48.00%), Direita: 52 (52.00%)  

1.000 ensaios:  
Esquerda: 491 (49.10%), Direita: 509 (50.90%)  

10.000 ensaios:  
Esquerda: 4996 (49.96%), Direita: 5004 (50.04%)  

100.000 ensaios:  
Esquerda: 49975 (49.97%), Direita: 50025 (50.02%)  

1.000.000 ensaios:  
Esquerda: 499638 (49.96%), Direita: 500362 (50.04%)
  

10.000.000 ensaios:  
Esquerda: 5000521 (50.01%), Direita: 4999479 (49.99%)  

Percebe-se que as probabilidades dos desvios para a direita ou para a esquerda tendem a 50% ao aumentar-se o número de ensaios.  

**4. Função `main()`:**

```c
int main() {
    stdio_usb_init();
    sleep_ms(2000);
    printf("Iniciando Galton Board...\n");

    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    init_display();
```

* `int main() {`: Define a função principal do programa, o ponto de entrada da execução.
* `stdio_init_all();`: Uma função fornecida pelo SDK do Pico. Ela inicializa os periféricos de entrada e saída padrão. Na maioria dos casos, isso configura a comunicação serial (UART) para que se possa usar `printf` para enviar dados para o monitor serial do seu computador.
* `sleep_ms(2000);`: Outra função do SDK do Pico. Ela pausa a execução do programa pelo número de milissegundos especificado (neste caso, 2000 ms ou 2 segundos). Isso é comumente usado para dar tempo para que o monitor serial da IDE seja conectado e esteja pronto para receber a saída antes que o programa comece a enviar dados.
* `printf("Iniciando Galton Board...\n");`: Imprime uma mensagem de inicialização no monitor serial.  
```c
gpio_init(BUTTON_A);
gpio_init(BUTTON_B);
gpio_set_dir(BUTTON_A, GPIO_IN);
gpio_set_dir(BUTTON_B, GPIO_IN);
gpio_pull_up(BUTTON_A);
gpio_pull_up(BUTTON_B);
```
Inicializa os pinos GPIO 5 e 6, ligados aos botões A e B da placa, como saída com pull up resistor.
* `init_display();`: Chama uma função definida no arquivo `display.c` (cujo protótipo está em `display.h`). Esta função inicializa o display SSD1306 e será explicada mais adiante.  

**5. Inicialização das Bolinhas:**

```c
    Ball balls[MAX_BALLS];
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
```
* `Ball balls[MAX_BALLS];`: Declara um array chamado `balls` capaz de armazenar `MAX_BALLS` (10) estruturas do tipo `Ball`(definidos em galton.h com atributos como posição x, y, estado active, e contagem de colisões). `MAX_BALLS` é uma constante que também é definida no arquivo `galton.h` que especifica o número de bolinhas que serão simuladas simultaneamente.
* `for (int i = 0; i < MAX_BALLS; i++) { ... }`: Inicia um loop que itera sobre cada elemento do array `balls`.
* `balls[i].active = false;`: Inicializa cada bolinha, definindo active = false, garantindo que nenhuma esteja ativa no início da simulação. Isso prepara o sistema para ativar bolinhas dinamicamente durante a "chuva de bolinhas".  

**6. Inicialização do Histograma e da Contagem de Bolinhas:**

```c
for (int i = 0; i < CHANNELS; i++) {
        histogram[i] = 0;
    }
    total_balls = 0;
```
* `for (int i = 0; i < CHANNELS; i++) {...}`: Inicia um laço que itera, de i = 0 até i < CHANNELS (CHANNELS é 16, definido em galton.h), sobre os elemento do array `histogram`.
* `histogram[i] = 0;`: Define o valor do elemento histogram[i] como 0. O array histogram (declarado em galton.c) armazena a contagem de bolas em cada um dos 16 bins. Este comando zera todas as contagens, inicializando o histograma para a simulação.  

* `total_balls = 0;`: Inicializa a variável global total_balls (declarada em galton.c) como 0. Essa variável rastreia o número total de bolas que caíram nos bins, usada no cálculo de estatísticas como média e desvio padrão exibidas no monitor serial a cada 100 bolas. Ela também aparece no topo do display OLED SSD1306 para informar o total de simulações.

**7. Definições Extras:**  

```c
extern float left_prob;
static uint32_t last_press_a = 0;
static uint32_t last_press_b = 0;
static bool last_state_a = true;
static bool last_state_b = true;
```

* `extern float left_prob;`: Declara que a variável left_prob (definida em galton.c) é externa, permitindo seu acesso em main.c. left_prob armazena a probabilidade de uma bolinha desviar à esquerda (ex.: 50.0f para 50%, 70.0f para 70%), ajustada pelos botões A e B.  

* `static uint32_t last_press_a = 0;`: Declara uma variável estática last_press_a do tipo uint32_t, inicializada como 0. Ela armazena o timestamp (em milissegundos) da última pressão do botão A (GPIO 5). A keyword static garante que o valor persista entre iterações do loop principal. Usada para implementar o debounce (intervalo mínimo de 200ms entre pressões).

* `static uint32_t last_press_b = 0;`: Similar a last_press_a, mas para o botão B (GPIO 6). Armazena o timestamp da última pressão do botão B, também para debounce.

* `static bool last_state_a = true;`: Declara uma variável estática last_state_a do tipo bool, inicializada como true. Representa o último estado lido do botão A (GPIO 5), onde true indica não pressionado (devido ao pull-up interno) e false indica pressionado. Usada para detectar a borda de descida (transição de true para false) e evitar múltiplas leituras durante uma pressão.

* `static bool last_state_b = true;`: Similar a last_state_a, mas para o botão B (GPIO 6). Armazena o último estado do botão B, também para detectar a borda de descida no mecanismo de debounce.

* `int tick = 0;`: Cria e inicializa um contador (tick) que rastreia o número de iterações do loop principal, incrementado a cada 50ms. Ele controla a criação de novas bolinhas a cada 250ms, essencial para a dinâmica da simulação do Tabuleiro de Galton.

**8. Loop Principal da Simulação:**

```c
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        bool state_a = gpio_get(BUTTON_A);
        bool state_b = gpio_get(BUTTON_B);
```
*`while (true) {`: Inicia o loop principal do programa Galton Board, que executa continuamente (while (true)). Nele, são executados:  

*`uint32_t now = to_ms_since_boot(get_absolute_time())`;: Obtém o tempo atual em milissegundos desde o início do programa, armazenado em now, para gerenciar debounce e temporização.  

*`bool state_a = gpio_get(BUTTON_A);`: Lê o estado do botão A (GPIO 5), retornando false (pressionado) ou true (não pressionado, devido ao pull-up).  

*`bool state_b = gpio_get(BUTTON_B);`: Lê o estado do botão B (GPIO 6), de forma semelhante.
Esses comandos monitoram os botões para ajustar as probabilidades e controlam a sincronização do loop, que atualiza a simulação a cada 50ms.  

```c
        // Detecta borda de descida para botão A (1 -> 0)
        if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {
            if (left_prob < 90.0f) {
                left_prob += 10.0f;
            }
            last_press_a = now;
        }

        // Detecta borda de descida para botão B (1 -> 0)
        if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {
            if (left_prob > 10.0f) {
                left_prob -= 10.0f;
            }
            last_press_b = now;
        }

        last_state_a = state_a;
        last_state_b = state_b;
```
Este trecho implementa a lógica de detecção de pressão dos botões A e B com debounce, ajustando as probabilidades de desvio das bolinhas.  

*`if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {`: Verifica se o botão A foi pressionado (`!state_a`), se estava não pressionado antes (`last_state_a`) e se passararm mais de 200ms desde a última pressão (`now - last_press_a > DEBOUNCE_MS`), garantindo debounce.  

*`if (left_prob < 90.0f) {`: Checa se a probabilidade de desvio à esquerda (`left_prob`) é menor que 90%, evitando ultrapassar o limite máximo.  

*`left_prob += 10.0f;`: Incrementa `left_prob` em 10% (ex.: de 50% para 60%), aumentando a chance de desvio à esquerda.  

*`last_press_a = now;`: Atualiza o timestamp da última pressão do botão A com o tempo atual (`now`), para o próximo ciclo de debounce.  

*`}`: Fecha o bloco condicional do botão A.

*`if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {`**: Verifica se o botão B foi pressionado (`!state_b`), se estava não pressionado antes (`last_state_b`) e se passaram mais de 200ms desde a última pressão, para debounce.  

*`if (left_prob > 10.0f) {`: Checa se `left_prob` é maior que 10%, evitando ultrapassar o limite mínimo.  

*`left_prob -= 10.0f;`**: Decrementa `left_prob` em 10% (ex.: de 50% para 40%), aumentando a chance de desvio à direita.  

*`last_press_b = now;`: Atualiza o timestamp da última pressão do botão B com o tempo atual.  

*`}`: Fecha o bloco condicional do botão B.  

*`last_state_a = state_a;`: Armazena o estado atual do botão A (`state_a`) em `last_state_a` para a próxima iteração, permitindo detectar a próxima borda de descida.  

*`last_state_b = state_b;`: Armazena o estado atual do botão B (`state_b`) em `last_state_b` para a próxima iteração.  

```c
        if (tick % 5 == 0) {
            for (int i = 0; i < MAX_BALLS; i++) {
                if (!balls[i].active) {
                    init_ball(&balls[i]);
                    break;
                }
            }
        }
```
Este trecho controla a criação de novas bolinhas na simulação, adicionando uma nova bolinha à simulação a cada 250ms, se houver uma bolinha inativa no array `balls` (máximo de 10 bolinhas ativas). Assim, ele garante a "chuva" contínua de bolinhas no Tabuleiro de Galton.

*`if (tick % 5 == 0) {`: Verifica se o contador `tick` (incrementado a cada 50ms) é divisível por 5, ou seja, a cada 250ms (5 × 50ms), executa o bloco para adicionar uma nova bolinha.  

*`for (int i = 0; i < MAX_BALLS; i++) {`: Inicia um laço que itera sobre o array `balls` (tamanho `MAX_BALLS = 10`), buscando uma bolinha inativa.  

*`if (!balls[i].active) {`: Checa se a bolinha na posição `i` não está ativa (`active = false`), indicando que pode ser usada.  

*`init_ball(&balls[i]);`: Chama a função `init_ball` (definida em `galton.c`) para inicializar a bolinha `balls[i]`, definindo sua posição inicial (x=64, y=0), ativando-a (`active = true`) e zerando colisões.  

*`break;`: Sai do laço `for` após ativar uma bolinha, evitando inicializar múltiplas bolinhas no mesmo ciclo.  

*`}`: Fecha o bloco condicional da verificação de bolinha inativa.  

*`}`: Fecha o laço `for`.  

*`}`: Fecha o bloco condicional de `tick`.

```c
        for (int i = 0; i < MAX_BALLS; i++) {
            if (balls[i].active) {
                update_ball(&balls[i]);
                if (!balls[i].active) {
                    register_ball_landing(&balls[i]);
                    if (total_balls % 100 == 0 && total_balls > 0) {
                        calculate_statistics();
                    }
                }
            }
        }
```
Este trecho gerencia a atualização das bolinhas ativas na simulação. Ele atualiza todas as bolinhas ativas a cada iteração do loop principal (~50ms), movendo-as e processando colisões. Quando uma bolinha atinge o fundo, ela é registrada no histograma, e a cada 100 bolinhas, estatísticas são calculadas e exibidas serialmente, atualizando a simulação do Tabuleiro de Galton.

*`for (int i = 0; i < MAX_BALLS; i++) {`: Inicia um laço que itera sobre o array `balls` (tamanho `MAX_BALLS = 10`), verificando cada bolinha.  

*`if (balls[i].active) {`: Checa se a bolinha na posição `i` está ativa (`active = true`), indicando que está caindo na simulação.  

*`update_ball(&balls[i]);`: Chama `update_ball` (em `galton.c`) para atualizar a bolinha, movendo-a verticalmente (incrementa `y`), aplicando desvios horizontais (±4 pixels) nas colisões, e desativando-a (`active = false`) se atingir o fundo (y ≥ 64).  

*`if (!balls[i].active) {`: Verifica se a bolinha foi desativada após `update_ball` (ou seja, chegou ao fundo).  

*`register_ball_landing(&balls[i]);`: Chama `register_ball_landing` (em `galton.c`) para registrar a bolinha no bin correspondente (baseado em `x`), incrementando `histogram[bin]` e `total_balls`.  

*`if (total_balls % 100 == 0 && total_balls > 0) {`: Checa se o número total de bolas (`total_balls`) é divisível por 100 e maior que 0, indicando que 100 bolas caíram desde a última estatística.  

*`calculate_statistics();`**: Chama `calculate_statistics` (em `galton.c`) para calcular e exibir via serial o total de bolas, contagem por bin, média e desvio padrão.  

*`}`: Fecha o bloco condicional de estatísticas.  

*`}`: Fecha o bloco condicional de bolinha desativada.  

*`}`: Fecha o bloco condicional de bolinha ativa.  

*`}`: Fecha o laço `for`.


```c
        for (int i = 0; i < CHANNELS; i++) {
            if (histogram[i] < 0) histogram[i] = 0;
        }
        update_display(balls, MAX_BALLS, histogram);

        tick++;
        sleep_ms(50);
    }

    return 0;
}
```
Este trecho finaliza o loop principal, garantindo a integridade do histograma, atualizando o display e controlando a temporização.  

*`for (int i = 0; i < CHANNELS; i++) {`: Inicia um laço que itera sobre os 16 bins do array `histogram` (`CHANNELS = 16`, definido em `galton.h`).  

*`if (histogram[i] < 0) histogram[i] = 0;`: Verifica se o valor de `histogram[i]` é negativo (um erro improvável, mas possível em depuração) e, se for, corrige para 0, garantindo que o histograma tenha contagens válidas.  

*`}`: Fecha o laço `for`.  

*`update_display(balls, MAX_BALLS, histogram);`: Chama `update_display` (em `display.c`) para atualizar o display OLED, desenhando as bolinhas ativas (`balls`), o histograma (`histogram`, escala `histogram[i] / 2`), o contador de bolas (`total_balls`) e as probabilidades (ex.: "60%" à esquerda, "40%" à direita).  

*`tick++;`: Incrementa o contador `tick`, que rastreia o número de iterações do loop principal, usado para temporizar a criação de novas bolinhas (a cada 250ms, quando `tick % 5 == 0`).  

*`sleep_ms(50);`: Pausa a execução por 50 milissegundos, garantindo que o loop principal execute a cada ~50ms, controlando a taxa de atualização da simulação.  

*`}`: Fecha o loop `while (true)`, que continua indefinidamente.  

*`return 0;`: Indica o término bem-sucedido da função `main` (embora nunca seja alcançado, devido ao `while (true)`).  

*`}`: Fecha a função `main`.  

#### Simulador galton.c:

**1. Includes**:

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "galton.h"
```


**2. Definições**:

```c
int histogram[CHANNELS] = {0};
int total_balls = 0;
float left_prob = 50.0f; // Probabilidade inicial: 50% esquerda
```


**3. Função random_direction()**:

```c
bool random_direction() {
    return (get_rand_32() % 100) < left_prob; // Ex.: 60% esquerda -> true se rand < 60
}
```
**4. Função test_randomness()**:

```c
void test_randomness(int trials) {
    int left = 0, right = 0;
    for (int i = 0; i < trials; i++) {
        if (random_direction() == 0) left++;
        else right++;
    }
    printf("Esquerda: %d (%.2f%%), Direita: %d (%.2f%%)\n",
            left, (float)left / trials * 100,
            right, (float)right / trials * 100);
}
```

**5. Função calculate_statistics()**:

```c
void calculate_statistics() {
    if (total_balls == 0) {
        printf("Nenhuma bola registrada.\n");
        return;
    }

    float mean = 0.0f;
    for (int i = 0; i < CHANNELS; i++) {
        mean += (i + 1) * histogram[i];
    }
    mean /= total_balls;

    float variance = 0.0f;
    for (int i = 0; i < CHANNELS; i++) {
        variance += histogram[i] * ((i + 1) - mean) * ((i + 1) - mean);
    }
    variance /= total_balls;
    float std_dev = sqrtf(variance);

    printf("Total de Bolas: %d\n", total_balls);
    printf("Bins: ");
    for (int i = 0; i < CHANNELS; i++) {
        printf("[%d]: %d ", i + 1, histogram[i]);
    }
    printf("\nMédia: %.2f\nDesvio Padrão: %.2f\n", mean, std_dev);
}
```

**6. Função init_ball()**:

```c
void init_ball(Ball *ball) {
    ball->x = SSD1306_WIDTH / 2.0f;
    ball->y = 0.0f;
    ball->active = true;
    ball->collisions = 0;
}
```

**7. Função update_ball()**:

```c
void update_ball(Ball *ball) {
    if (!ball->active) return;

    ball->y += 1.0f;
    if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {
        bool dir = random_direction();
        if (dir) {
            ball->x -= 4.0f; // Esquerda
        } else {
            ball->x += 4.0f; // Direita
        }
        ball->collisions++;
    }

    if (ball->x < 0) ball->x = 0;
    if (ball->x >= SSD1306_WIDTH) ball->x = SSD1306_WIDTH - 1;
    if (ball->y >= SSD1306_HEIGHT) {
        ball->active = false;
    }
}
```

**8. Função register_ball_landing()**:

```c
void register_ball_landing(Ball *ball) {
    int bin = (int)(ball->x / (SSD1306_WIDTH / CHANNELS));
    if (bin >= 0 && bin < CHANNELS) {
        histogram[bin]++;
        total_balls++;
    }
}
```

**9. Função get_left_probability()**:

```c
float get_left_probability() {
    return left_prob;
}
```


#### Gerenciador de Exibição display.c:

**1. Includes**:

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "display.h"
```

**1. Includes**:


#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

void clear_display_buffer() {
    memset(display_buffer, 0, BUFFER_LENGTH);
}

void ssd1306_update_display() {
    uint8_t command_buffer[2];
    
    command_buffer[0] = 0x00;
    command_buffer[1] = 0x21;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x00;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x7F;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    
    command_buffer[1] = 0x22;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x00;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    command_buffer[1] = 0x07;
    i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);
    
    uint8_t data_buffer[1025];
    data_buffer[0] = 0x40;
    memcpy(&data_buffer[1], display_buffer, BUFFER_LENGTH);
    i2c_write_blocking(i2c1, 0x3C, data_buffer, BUFFER_LENGTH + 1, false);
}

void ssd1306_setup() {
    uint8_t init_commands[] = {
        0x00, 0xAE, 0x00, 0xD5, 0x80, 0x00, 0xA8, 0x3F, 0x00, 0xD3, 0x00,
        0x00, 0x40, 0x00, 0x8D, 0x14, 0x00, 0x20, 0x00, 0x00, 0xA1, 0x00,
        0xC8, 0x00, 0xDA, 0x12, 0x00, 0x81, 0xCF, 0x00, 0xD9, 0xF1, 0x00,
        0xDB, 0x40, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xAF
    };
    i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);
}

void init_display() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);
    ssd1306_setup();
    clear_display_buffer();
    ssd1306_update_display();
    clear_display_buffer();
    ssd1306_update_display();
}

void draw_histogram(int *histogram) {
    for (int i = 0; i < CHANNELS; i++) {
        if (histogram[i] > 0) {
            int height = histogram[i] / 2; // Cada 2 bolas adicionam 1 pixel de altura
            if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;
            for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {
                for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {
                    ssd1306_set_pixel(display_buffer, x, y, true);
                }
            }
        }
    }
}

void draw_ball(Ball *ball) {
    if (ball->active) {
        ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);
    }
}

void draw_probabilities(float left_prob) {
    char left_buffer[8];
    char right_buffer[8];
    snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);
    snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);
    ssd1306_draw_string(display_buffer, 0, 28, left_buffer); // Esquerda, y=28
    ssd1306_draw_string(display_buffer, 104, 28, right_buffer); // Direita, ajustado para caber
}

void update_display(Ball *balls, int ball_count, int *histogram) {
    clear_display_buffer();
    for (int i = 0; i < ball_count; i++) {
        draw_ball(&balls[i]);
    }
    draw_histogram(histogram);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Bolas: %d", total_balls);
    ssd1306_draw_string(display_buffer, 0, 0, buffer);
    draw_probabilities(get_left_probability());
    ssd1306_update_display();
}

void draw_counter(int total_balls) {
}