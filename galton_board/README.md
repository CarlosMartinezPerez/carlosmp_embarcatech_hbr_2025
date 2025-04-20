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

## Como Executar
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

## Análise do Código:

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

Como parte de resposta a uma demanda do projeto, este trecho, presente em um bloco comentado no código, representa uma função main() alternativa utilizada no início do desenvolvimento para testar a qualidade do gerador de números aleatórios do projeto. A função inicializa a comunicação serial com stdio_init_all(), aguarda 2000 milissegundos (sleep_ms(2000)) para permitir a conexão com um monitor serial, e chama test_randomness(100000), que executa 100.000 iterações da função random_direction para verificar se as probabilidades de desvio à esquerda e à direita estão equilibradas (ex.: ~50% para cada lado com a configuração padrão). Os resultados são exibidos no monitor serial, mostrando a contagem e a porcentagem de desvios à esquerda e à direita. Embora comentado na versão final, esse teste foi essencial para validar a aleatoriedade do sistema, garantindo que a distribuição binomial da simulação refletisse corretamente as probabilidades esperadas.  


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

**Inicialização das Bolinhas:**

```c
    Ball balls[MAX_BALLS];
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
```
* `Ball balls[MAX_BALLS];`: Declara um array chamado `balls` capaz de armazenar `MAX_BALLS` (10) estruturas do tipo `Ball`(definidos em galton.h com atributos como posição x, y, estado active, e contagem de colisões). `MAX_BALLS` é uma constante que também é definida no arquivo `galton.h` que especifica o número de bolinhas que serão simuladas simultaneamente.
* `for (int i = 0; i < MAX_BALLS; i++) { ... }`: Inicia um loop que itera sobre cada elemento do array `balls`.
* `balls[i].active = false;`: Inicializa cada bolinha, definindo active = false, garantindo que nenhuma esteja ativa no início da simulação. Isso prepara o sistema para ativar bolinhas dinamicamente durante a "chuva de bolinhas".  

**Inicialização do Histograma e da Contagem de Bolinhas:**

```c
for (int i = 0; i < CHANNELS; i++) {
        histogram[i] = 0;
    }
```
* `for (int i = 0; i < CHANNELS; i++) {...}`: Inicia um laço que itera, de i = 0 até i < CHANNELS (CHANNELS é 16, definido em galton.h), sobre os elemento do array `histogram`.
* `histogram[i] = 0;`: Define o valor do elemento histogram[i] como 0. O array histogram (declarado em galton.c) armazena a contagem de bolas em cada um dos 16 bins. Este comando zera todas as contagens, inicializando o histograma para a simulação.  

* `total_balls = 0;`: Inicializa a variável global total_balls (declarada em galton.c) como 0. Essa variável rastreia o número total de bolas que caíram nos bins, usada no cálculo de estatísticas como média e desvio padrão exibidas no monitor serial a cada 100 bolas. Ela também aparece no topo do display OLED SSD1306 para informar o total de simulações.

**Definições Extras:**  

* `extern float left_prob;`: Declara que a variável left_prob (definida em galton.c) é externa, permitindo seu acesso em main.c. left_prob armazena a probabilidade de uma bolinha desviar à esquerda (ex.: 50.0f para 50%, 70.0f para 70%), ajustada pelos botões A e B.  

* `static uint32_t last_press_a = 0;`: Declara uma variável estática last_press_a do tipo uint32_t, inicializada como 0. Ela armazena o timestamp (em milissegundos) da última pressão do botão A (GPIO 5). A keyword static garante que o valor persista entre iterações do loop principal. Usada para implementar o debounce (intervalo mínimo de 200ms entre pressões).

* `static uint32_t last_press_b = 0;`: Similar a last_press_a, mas para o botão B (GPIO 6). Armazena o timestamp da última pressão do botão B, também para debounce.

* `static bool last_state_a = true;`: Declara uma variável estática last_state_a do tipo bool, inicializada como true. Representa o último estado lido do botão A (GPIO 5), onde true indica não pressionado (devido ao pull-up interno) e false indica pressionado. Usada para detectar a borda de descida (transição de true para false) e evitar múltiplas leituras durante uma pressão.

* `static bool last_state_b = true;^: Similar a last_state_a, mas para o botão B (GPIO 6). Armazena o último estado do botão B, também para detectar a borda de descida no mecanismo de debounce.

* `int tick = 0;`: Cria e inicializa um contador (tick) que rastreia o número de iterações do loop principal, incrementado a cada 50ms. Ele controla a criação de novas bolinhas a cada 250ms, essencial para a dinâmica da simulação do Tabuleiro de Galton.

**Loop Principal de Simulação:**

```c
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        bool state_a = gpio_get(BUTTON_A);
        bool state_b = gpio_get(BUTTON_B);

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

        if (tick % 5 == 0) {
            for (int i = 0; i < MAX_BALLS; i++) {
                if (!balls[i].active) {
                    init_ball(&balls[i]);
                    break;
                }
            }
        }

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

