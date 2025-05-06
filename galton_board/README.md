# GALTON BOARD

Este projeto implementa uma simulação digital de um **Tabuleiro de Galton** (Galton Board) utilizando um microcontrolador Raspberry Pi Pico e um display OLED SSD1306 (128x64 pixels), todos presentes na placa BitDogLab. A simulação modela a queda de bolinhas através de fileiras de pinos, resultando em uma distribuição binomial, que é visualizada em um histograma no display e analisada via saída serial. A distribuição das bolinhas no histograma, para um número suficientemente grande de ensaios, se aproxima cada vez mais, da forma de uma curva gaussiana (ou normal, denotada por N(0,1) se na sua forma padrão, conforme estabelece o Teorema Central do Limite). Este [vídeo](https://www.youtube.com/watch?v=gffrqLjbuIY&t=18s) mostra o projeto funcionando na BitDogLab.  

## 1. O que é um Tabuleiro de Galton?

O Tabuleiro de Galton, também conhecido como Quincunx, é um dispositivo inventado por Sir Francis Galton no século XIX para demonstrar o teorema central do limite e a distribuição binomial. Consiste em uma placa vertical com fileiras de pinos dispostos em padrão triangular. Bolinhas são liberadas do topo desde o centro do tabuleiro e, ao colidirem com os pinos, desviam aleatoriamente para a esquerda ou direita com igual probabilidade (50%). Ao final, as bolinhas se acumulam em compartimentos (bins) na base, formando uma curva em forma de sino que se aproxima da forma da distribuição normal para um grande número de bolinhas liberadas (ensaios).

A beleza do Tabuleiro de Galton está em sua capacidade de ilustrar conceitos estatísticos fundamentais de forma visual e intuitiva, sendo amplamente utilizado em estudos de Probabilidade, Estatística e Física. Um exemplo bastante ilustrativo dessa demonstração pode ser visto [neste vídeo](https://www.youtube.com/watch?v=L4RMRq1FUcg), em que um Tabuleiro de Galton é posto em ação, destacando a formação da curva em forma de sino com milhares de bolinhas. O conceito subjacente é que muitas vezes se pode obter um resultado previsível mesmo tratando com eventos completamente aleatórios.

A distribuição binomial é uma distribuição de probabilidade que descreve a probabilidade de um certo número de sucessos ("S") em um número fixo de tentativas ("n") independentes, onde cada tentativa tem apenas dois resultados possíveis (sucesso ou fracasso) com a mesma probabilidade ("p" e "1-p") em cada tentativa. Newton e Pascal tiveram papel relevente no desenvolvimento da teoria dos coeficientes binomiais, que são fundamentais para a distribuição binomial.

Para descrever a distribuição binomila, imagine um jogo com dois resultados possíveis: ganhar (sucesso) ou perder (fracasso). Se o jofo for jogardo n vezes, a distribuição binomial pode dizer qual a probabilidade de ganhar x vezes (onde x é o número de sucessos). A probabilidade de se obter exatamente x sucessos em n tentativas é dada por: P(x) = (nCx) * p^x * (1-p)^(n-x), onde nCx é o coeficiente binomial (combinação de n elementos tomados x a x), que representa o número de maneiras de se obter x sucessos entre n tentativas.

Blaise Pascal criou o triângulo de Pascal, uma representação visual dos coeficientes binomiais. Cada número no triângulo é a soma dos dois números acima dele, e os coeficientes binomiais podem ser lidos diretamente das linhas do triângulo. Isaac Newton expandiu o trabalho de Pascal e desenvolveu o teorema do binômio de Newton, que fornece uma fórmula para expandir uma potência de um binômio (x + y)^n. Essa fórmula usa os coeficientes binomiais, que são os mesmos do triângulo de Pascal. Os coeficientes binomiais são fundamentais para calcular probabilidades em situações de distribuição binomial, como em testes de hipóteses ou análises de dados. Newton e Pascal contribuíram significativamente para o desenvolvimento e compreensão destes coeficientes.

## 2. A Simulação Digital (Digital Twin)

A simulação digital desenvolvida neste projeto recria o comportamento de um Tabuleiro de Galton em um display OLED de 128x64 pixels, funcionando como um "gêmeo digital" do dispositivo físico. A seguir, descrevem-se os detalhes da implementação:

### Estrutura da Simulação
- **Display e Bins**: Os 128 pixels horizontais do display são divididos em 16 bins, cada um com 8 pixels de largura (128 ÷ 16 = 8). Esses bins, localizados na base do display, acumulam as bolinhas após sua queda.
- **Pinos e Fileiras**: Acima dos 16 bins, há 15 fileiras de pinos dispostos em um padrão triangular (15 pinos na primeira fileira logo acima dos bins, 14 na fileira acima desta, e assim por diante).
- **Geometria dos Pinos**: Os pinos formam triângulos equiláteros com lados de 8 pixels. A distância vertical entre fileiras é calculada como a altura de um triângulo equilátero de lado 8, ou 4 vezes a raiz quadrada de 3, aproximadamente 6,93 (7 pixels), mas a simulação abstrai isso para manter a lógica simples, mesmo porque, não caberiam 15 fileiras de 7 pixels na altura do display.
- **Comportamento das Bolinhas**: Cada bolinha começa no centro do tabuleiro (x = 64) e sofre 15 colisões, desviando aleatoriamente ±4 pixels (para a esquerda ou para a direita) por colisão, com 50% de chance para cada direção. Após 15 colisões, a bolinha cai em um dos 16 bins.
- **Casos Extremos**:
  - Máximo à direita: \( 64 + 15 x 4 = 124 \) (bin 16).
  - Máximo à esquerda: \( 64 - 15 x 4 = 4 \) (bin 1).
- **Visualização**: As bolinhas são exibidas como pontos no display durante a queda, e o histograma na base é desenhado com quadrados, em que cada 2 bolinhas incrementam 1 pixel de altura (escala ajustada para evitar preenchimento muito rápido da tela).
- **Saída Serial**: A cada 100 bolas, o programa exibe no Monitor Serial:
  - Total de bolas.
  - Quantidade de bolas em cada bin.
  - Média da distribuição.
  - Desvio padrão da distribuição.
- Controle de Probabilidades: Dois botões, conectados aos pinos GPIO 5 (botão A) e GPIO 6 (botão B), permitem ajustar dinamicamente as probabilidades de desvio das bolinhas. Inicialmente, a probabilidade é 50% para esquerda e 50% para direita. Cada pressão do botão A aumenta a probabilidade de desvio à esquerda em 10% (e reduz a da direita em 10%), até um máximo de 90% esquerda / 10% direita. O botão B faz o oposto, aumentando a probabilidade de desvio à direita até 10% esquerda / 90% direita. As porcentagens escolhidas com os botões são exibidas nas laterais do display, permitindo visualizar o impacto dessas alterações na distribuição binomial, que se torna assimétrica conforme as probabilidades mudam.

### Características
- A simulação utiliza o gerador de números aleatórios do Pico (`get_rand_32`) para garantir desvios aleatórios.
- Até 10 bolinhas podem estar ativas simultaneamente, caindo em uma "chuva" contínua.
- Repare que a probabilidade teórica de uma bolinha cair nos bins extremos (1 ou 16) é meio elevado a 15, vezes 100% ou 0,003%, aproximadamente. Só existe uma maneira de uma bolinha atingir o bin 16, ela tem que desviar à direita nas 15 colisões. Já para chegar aos bins 8 ou 9 (centrais), a bolinha tem 6435 (15C7 ou 15C8) caminhos diferentes para fazer. O cálculo dessa probabilidade é dado pela combinação de 15, tomado 8 a 8 (ou 7 a 7, dá na mesma), vezes meio elevado a 15, vezes 100% ou 6435 vezes 0,003%, aproximadamente 19.64\%. A diferença nessas probabilidades (da bolinha cair nos bins 8 ou 9 ou de cair nos bins 1 ou 16) reflete bem a natureza binomial da simulação.

## 3. Saída do Monitor Serial

Veja a saída do monitor serial após a queda de 200 bolinhas no tabuleiro:

---- Opened the serial port COM4 ----  
Iniciando Galton Board...  
Total de Bolas: 100  
Bins: [1]: 0 [2]: 0 [3]: 0 [4]: 2 [5]: 4 [6]: 11 [7]: 8 [8]: 22 [9]: 25 [10]: 17 [11]: 5 [12]: 4 [13]: 2 [14]: 0 [15]: 0 [16]: 0   
Média: 8.50  
Desvio Padrão: 1.86  

Total de Bolas: 200  
Bins: [1]: 0 [2]: 0 [3]: 0 [4]: 2 [5]: 6 [6]: 23 [7]: 25 [8]: 41 [9]: 46 [10]: 34 [11]: 13 [12]: 6 [13]: 4 [14]: 0 [15]: 0 [16]: 0   
Média: 8.50  
Desvio Padrão: 1.80  


## 4. Análise do Código

### Modularização
O código é organizado em módulos para promover clareza, reusabilidade e manutenibilidade. A modularização separa a lógica da simulação, a interface do display e a inicialização do sistema, oferecendo as seguintes vantagens:
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
   - Loop principal: Atualiza bolinhas, registra quedas e faz o refresh do display a cada 50ms.  

**display.c / display.h**:  
   - Gerencia o display OLED SSD1306.  
   - Funções principais:  
     - `init_display`: Configura o display.  
     - `draw_histogram`: Desenha o histograma com altura proporcional às bolas (escala: 1 pixel por 2 bolas).  
     - `update_display`: Atualiza o display com bolinhas, total de bolas, histograma e probabilidades.  

**CMakeLists.txt**:  
O arquivo CMakeLists.txt é usado para configurar o processo de compilação do projeto, definindo as instruções para o CMake, uma ferramenta de construção, especificando:  
   - O nome do projeto e a versão do CMake necessária.  
   - Os arquivos-fonte (main.c, galton.c, display.c) a serem compilados.  
   - As bibliotecas do Pico SDK (ex.: pico_stdlib, hardware_i2c) a serem vinculadas.  
   - Configurações como suporte a USB e UART para saída serial.  
   - Geração do executável para o Raspberry Pi Pico.  

### Detalhes Técnicos
- **Display**: O SSD1306 é controlado via I2C, com buffer para renderizar bolinhas (pixels individuais), histograma (retângulos por bin) e textos.
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
   - Conecte o display aos pinos I2C do Pico (SDA ao GPIO 14, SCL ao GPIO 15).
3. **Compilação**:
   - Clone o repositório.
   - Execute `cmake` e `make` no diretório `build`.
   - Carregue o firmware no Pico via USB.
4. **Saída**:
   - Visualize a chuva de bolinhas e o histograma no display.
   - Conecte-se a um monitor serial para as ver estatísticas a cada 100 bolas.  


### Explicação do Código:


#### Orquestrador main.c:

**1. Inclusões:**

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
    test_randomness(100000); // Altere a quantidade de ensaios
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
    sleep_ms(3000); // Tempo para abrir o Monitor Serial
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
* `sleep_ms(3000);`: Outra função do SDK do Pico. Ela pausa a execução do programa pelo número de milissegundos especificado (neste caso, 2000 ms ou 2 segundos). Isso é comumente usado para dar tempo para que o monitor serial da IDE seja conectado e esteja pronto para receber a saída antes que o programa comece a enviar dados.
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
* `while (true) {`: Inicia o loop principal do programa Galton Board, que executa continuamente (while (true)). Nele, são executados:  

* `uint32_t now = to_ms_since_boot(get_absolute_time())`;: Obtém o tempo atual em milissegundos desde o início do programa, armazenado em now, para gerenciar debounce e temporização.  

* `bool state_a = gpio_get(BUTTON_A);`: Lê o estado do botão A (GPIO 5), retornando false (pressionado) ou true (não pressionado, devido ao pull-up).  

* `bool state_b = gpio_get(BUTTON_B);`: Lê o estado do botão B (GPIO 6), de forma semelhante.
Esses comandos monitoram os botões para ajustar as probabilidades e controlam a sincronização do loop, que atualiza a simulação a cada 50ms.  

```c
        if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {
            if (left_prob < 90.0f) {
                left_prob += 10.0f;
            }
            last_press_a = now;
        }

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

* `if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {`: Verifica se o botão A foi pressionado (`!state_a`), se estava não pressionado antes (`last_state_a`) e se passararm mais de 200ms desde a última pressão (`now - last_press_a > DEBOUNCE_MS`), garantindo debounce.  

* `if (left_prob < 90.0f) {`: Checa se a probabilidade de desvio à esquerda (`left_prob`) é menor que 90%, evitando ultrapassar o limite máximo.  

* `left_prob += 10.0f;`: Incrementa `left_prob` em 10% (ex.: de 50% para 60%), aumentando a chance de desvio à esquerda.  

* `last_press_a = now;`: Atualiza o timestamp da última pressão do botão A com o tempo atual (`now`), para o próximo ciclo de debounce.  

* `}`: Fecha o bloco condicional do botão A.

* `if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {`: Verifica se o botão B foi pressionado (`!state_b`), se estava não pressionado antes (`last_state_b`) e se passaram mais de 200ms desde a última pressão, para debounce.  

* `if (left_prob > 10.0f) {`: Checa se `left_prob` é maior que 10%, evitando ultrapassar o limite mínimo.  

* `left_prob -= 10.0f;`: Decrementa `left_prob` em 10% (ex.: de 50% para 40%), aumentando a chance de desvio à direita.  

* `last_press_b = now;`: Atualiza o timestamp da última pressão do botão B com o tempo atual.  

* `}`: Fecha o bloco condicional do botão B.  

* `last_state_a = state_a;`: Armazena o estado atual do botão A (`state_a`) em `last_state_a` para a próxima iteração, permitindo detectar a próxima borda de descida.  

* `last_state_b = state_b;`: Armazena o estado atual do botão B (`state_b`) em `last_state_b` para a próxima iteração.  

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

* `if (tick % 5 == 0) {`: Verifica se o contador `tick` (incrementado a cada 50ms) é divisível por 5, ou seja, a cada 250ms (5 × 50ms), executa o bloco para adicionar uma nova bolinha.  

* `for (int i = 0; i < MAX_BALLS; i++) {`: Inicia um laço que itera sobre o array `balls` (tamanho `MAX_BALLS = 10`), buscando uma bolinha inativa.  

* `if (!balls[i].active) {`: Checa se a bolinha na posição `i` não está ativa (`active = false`), indicando que pode ser usada.  

* `init_ball(&balls[i]);`: Chama a função `init_ball` (definida em `galton.c`) para inicializar a bolinha `balls[i]`, definindo sua posição inicial (x=64, y=0), ativando-a (`active = true`) e zerando colisões.  

* `break;`: Sai do laço `for` após ativar uma bolinha, evitando inicializar múltiplas bolinhas no mesmo ciclo.  

* `}`: Fecha o bloco condicional da verificação de bolinha inativa.  

* `}`: Fecha o laço `for`.  

* `}`: Fecha o bloco condicional de `tick`.

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

* `for (int i = 0; i < MAX_BALLS; i++) {`: Inicia um laço que itera sobre o array `balls` (tamanho `MAX_BALLS = 10`), verificando cada bolinha.  

* `if (balls[i].active) {`: Checa se a bolinha na posição `i` está ativa (`active = true`), indicando que está caindo na simulação.  

* `update_ball(&balls[i]);`: Chama `update_ball` (em `galton.c`) para atualizar a bolinha, movendo-a verticalmente (incrementa `y`), aplicando desvios horizontais (±4 pixels) nas colisões, e desativando-a (`active = false`) se atingir o fundo (y ≥ 64).  

* `if (!balls[i].active) {`: Verifica se a bolinha foi desativada após `update_ball` (ou seja, chegou ao fundo).  

* `register_ball_landing(&balls[i]);`: Chama `register_ball_landing` (em `galton.c`) para registrar a bolinha no bin correspondente (baseado em `x`), incrementando `histogram[bin]` e `total_balls`.  

* `if (total_balls % 100 == 0 && total_balls > 0) {`: Checa se o número total de bolas (`total_balls`) é divisível por 100 e maior que 0, indicando que 100 bolas caíram desde a última estatística.  

* `calculate_statistics();`: Chama `calculate_statistics` (em `galton.c`) para calcular e exibir via serial o total de bolas, contagem por bin, média e desvio padrão.  

* `}`: Fecha o bloco condicional de estatísticas.  

* `}`: Fecha o bloco condicional de bolinha desativada.  

* `}`: Fecha o bloco condicional de bolinha ativa.  

* `}`: Fecha o laço `for`.


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

* `for (int i = 0; i < CHANNELS; i++) {`: Inicia um laço que itera sobre os 16 bins do array `histogram` (`CHANNELS = 16`, definido em `galton.h`).  

* `if (histogram[i] < 0) histogram[i] = 0;`: Verifica se o valor de `histogram[i]` é negativo (um erro improvável, mas possível em depuração) e, se for, corrige para 0, garantindo que o histograma tenha contagens válidas.  

* `}`: Fecha o laço `for`.  

* `update_display(balls, MAX_BALLS, histogram);`: Chama `update_display` (em `display.c`) para atualizar o display OLED, desenhando as bolinhas ativas (`balls`), o histograma (`histogram`, escala `histogram[i] / 2`), o contador de bolas (`total_balls`) e as probabilidades (ex.: "60%" à esquerda, "40%" à direita).  

* `tick++;`: Incrementa o contador `tick`, que rastreia o número de iterações do loop principal, usado para temporizar a criação de novas bolinhas (a cada 250ms, quando `tick % 5 == 0`).  

* `sleep_ms(50);`: Pausa a execução por 50 milissegundos, garantindo que o loop principal execute a cada ~50ms, controlando a taxa de atualização da simulação.  

* `}`: Fecha o loop `while (true)`, que continua indefinidamente.  

* `return 0;`: Indica o término bem-sucedido da função `main` (embora nunca seja alcançado, devido ao `while (true)`).  

* `}`: Fecha a função `main`.  


#### Simulador galton.c:

**1. Inclusões**:

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "galton.h"
```
Este trecho define as bibliotecas necessárias para a implementação da lógica da simulação, como aleatoriedade, cálculos estatísticos e interação com hardware.  

* `#include <stdio.h>`: Inclui a biblioteca padrão de entrada/saída, permitindo o uso de funções como `printf` para exibir estatísticas na saída serial (ex.: em `calculate_statistics`).  
* `#include <stdlib.h>`: Inclui a biblioteca padrão, fornecendo funções como `get_rand_32` (usada indiretamente via `pico/rand.h`) para geração de números aleatórios.  
* `#include <math.h>`: Inclui a biblioteca matemática, fornecendo funções como `sqrtf` para cálculos de desvio padrão em `calculate_statistics`.  
* `#include "pico/stdlib.h"`: Inclui a biblioteca padrão do Pico SDK, fornecendo funções como `sleep_ms` e acesso a GPIO, usadas em outras partes do projeto.  
* `#include "pico/rand.h"`: Inclui a biblioteca de geração de números aleatórios do Pico SDK, fornecendo `get_rand_32` para a função `random_direction`, essencial para simular desvios aleatórios.  
* `#include "galton.h"`: Inclui o arquivo de cabeçalho `galton.h`, que define constantes (ex.: `MAX_BALLS`, `CHANNELS`), a estrutura `Ball`, e protótipos de funções usadas em `galton.c` (ex.: `init_ball`, `update_ball`).  

**2. Definições**:

```c
int histogram[CHANNELS] = {0};
int total_balls = 0;
float left_prob = 50.0f;
```
Este trecho define e inicializa variáveis globais para a simulação.

* `int histogram[CHANNELS] = {0};`: Declara e inicializa um array global `histogram` com tamanho `CHANNELS` (16, definido em `galton.h`), preenchendo todos os elementos com 0. Ele armazena a contagem de bolas que caem em cada bin do Tabuleiro de Galton.
* `int total_balls = 0;`: Declara e inicializa a variável global `total_balls` como 0. Ela rastreia o número total de bolas que atingiram os bins, usada para estatísticas e exibição no display.
* `float left_prob = 50.0f; Declara e inicializa a variável global `left_prob` como 50.0f (50%), definindo a probabilidade inicial de uma bolinha desviar à esquerda em cada colisão. É ajustada dinamicamente pelos botões A e B em `main.c`.  

**3. Função random_direction()**:

```c
bool random_direction() {
    return (get_rand_32() % 100) < left_prob;
}
```
Este trecho define a função `random_direction` que determina aleatoriamente a direção de desvio de uma bolinha.

* `bool random_direction() {`: Declara a função `random_direction`, que retorna um valor booleano (`true` para desvio à esquerda, `false` para direita).  
* `return (get_rand_32() % 100) < left_prob;`: Gera um número aleatório de 0 a 99 usando `get_rand_32() % 100` (onde `get_rand_32` é do `pico/rand.h`) e compara com `left_prob` (ex.: 50.0f para 50%, 60.0f para 60%). Retorna `true` se o número for menor que `left_prob`, indicando desvio à esquerda (ex.: para 60%, `true` se `get_rand_32() % 100` < 60). Caso contrário, retorna `false` (desvio à direita).
* `}`: Fecha a função.  

**4. Função test_randomness()**:

```c
void test_randomness(int trials) {
    int left = 0, right = 0;
    for (int i = 0; i < trials; i++) {
        if (random_direction() == 0) right++;
        else left++;
    }
    printf("Esquerda: %d (%.2f%%), Direita: %d (%.2f%%)\n",
            left, (float)left / trials * 100,
            right, (float)right / trials * 100);
}
```
Este trecho define a função `test_randomness` para testar a distribuição de desvios aleatórios.

* `void test_randomness(int trials) {`: Declara a função `test_randomness`, que recebe um parâmetro `trials` (número de simulações) e não retorna valor, apenas imprime os resultados no monitor serial.    
* `int left = 0, right = 0;`: Declara e inicializa duas variáveis locais, `left` e `right`, para contar os desvios à esquerda e à direita, respectivamente.  
* `for (int i = 0; i < trials; i++) {`: Inicia um laço que executa `trials` iterações para testar a função `random_direction`.  
* `if (random_direction() == 0) right++;`: Chama `random_direction`; se retornar `false` (0, desvio à direita), incrementa `right`.  
* `else left++;`: Se `random_direction` retornar `true` (desvio à esquerda), incrementa `left`.  
* `}`: Fecha o laço `for`.
* `printf("Esquerda: %d (%.2f%%), Direita: %d (%.2f%%)\n", ...);`: Exibe via serial o número de desvios à esquerda (`left`) e à direita (`right`), com suas porcentagens calculadas como `(float)left / trials * 100` e `(float)right / trials * 100`, formatadas com duas casas decimais.  
* `}`: Fecha a função.  

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
Este trecho define a função `calculate_statistics`, que calcula e exibe no monitor serial, estatísticas da simulação, a cada 100 bolinhas.  

* `void calculate_statistics() {`: Declara a função `calculate_statistics`, que não retorna valor, usada para processar dados do histograma.
* `if (total_balls == 0) {`: Verifica se nenhuma bola foi registrada (`total_balls == 0`).
* `printf("Nenhuma bola registrada.\n");`: Exibe uma mensagem via serial se não houver bolas.
* `return;`: Sai da função se `total_balls` for 0, evitando cálculos inválidos.
* `float mean = 0.0f;`: Inicializa a variável `mean` (média) como 0.0.
* `for (int i = 0; i < CHANNELS; i++) {`: Itera sobre os 16 bins (`CHANNELS = 16`).
* `mean += (i + 1) * histogram[i];`: Acumula a soma ponderada dos bins (bin `i+1` multiplicado pela contagem `histogram[i]`) para calcular a média.
* `}`: Fecha o laço.
* `mean /= total_balls;`: Divide a soma pelo total de bolas para obter a média.
* `float variance = 0.0f;`: Inicializa a variável `variance` (variância) como 0.0.
* `for (int i = 0; i < CHANNELS; i++) {`: Itera novamente sobre os bins.
* `variance += histogram[i] * ((i + 1) - mean) * ((i + 1) - mean);`: Acumula a soma dos quadrados das diferenças entre cada bin (`i+1`) e a média, ponderada por `histogram[i]`, para calcular a variância.
* `}`: Fecha o laço.
* `variance /= total_balls;`: Divide a soma pelo total de bolas para obter a variância.
* `float std_dev = sqrtf(variance);`: Calcula o desvio padrão como a raiz quadrada da variância, usando `sqrtf` de `math.h`.
* `printf("Total de Bolas: %d\n", total_balls);`: Exibe o número total de bolas via serial.
* `printf("Bins: ");`: Inicia a exibição das contagens por bin.
* `for (int i = 0; i < CHANNELS; i++) {`: Itera sobre os bins para exibir contagens.
* `printf("[%d]: %d ", i + 1, histogram[i]);`: Exibe o número do bin (`i+1`) e sua contagem (`histogram[i]`).
* `}`: Fecha o laço.
* `printf("\nMédia: %.2f\nDesvio Padrão: %.2f\n", mean, std_dev);`: Exibe a média e o desvio padrão, formatados com duas casas decimais.

**6. Função init_ball()**:

```c
void init_ball(Ball *ball) {
    ball->x = SSD1306_WIDTH / 2.0f;
    ball->y = 0.0f;
    ball->active = true;
    ball->collisions = 0;
}
```
Este trecho define a função `init_ball`, que inicializa uma bolinha para a simulação.

* `void init_ball(Ball *ball) {`: Declara a função `init_ball`, que recebe um ponteiro para uma estrutura `Ball` (definida em `galton.h`) e não retorna valor.
* `ball->x = SSD1306_WIDTH / 2.0f;`: Define a coordenada horizontal `x` da bolinha como o centro do display (`SSD1306_WIDTH = 128`, então `x = 64.0f`), ponto inicial de queda.
* `ball->y = 0.0f;`: Define a coordenada vertical `y` como 0, posicionando a bolinha no topo do display.
* `ball->active = true;`: Marca a bolinha como ativa (`active = true`), indicando que está em movimento na simulação.
* `ball->collisions = 0;`: Inicializa o contador de colisões como 0, rastreando quantas vezes a bolinha desviou (máximo de 15).
* `}`: Fecha a função.

**7. Função update_ball()**:

```c
void update_ball(Ball *ball) {
    if (!ball->active) return;

    ball->y += 1.0f;
    if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {
        bool dir = random_direction();
        if (dir) {
            ball->x -= 4.0f;
        } else {
            ball->x += 4.0f;
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
Este trecho define a função `update_ball`, que atualiza o estado de uma bolinha durante a simulação.

* `void update_ball(Ball *ball) {`: Declara a função `update_ball`, que recebe um ponteiro para uma estrutura `Ball` e não retorna valor.
* `if (!ball->active) return;`: Verifica se a bolinha está inativa (`active = false`); se for, sai da função sem fazer alterações.
* `ball->y += 1.0f;`: Incrementa a coordenada vertical `y` em 1 pixel, movendo a bolinha para baixo.
* `if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {`: Checa se a bolinha ainda pode colidir (`collisions < 15`) e se atingiu a altura da próxima colisão (calculada como `(collisions + 1) * (64 / 15) ≈ 4.27` pixels).
* `bool dir = random_direction();`: Chama `random_direction` para decidir aleatoriamente a direção do desvio (`true` para esquerda, `false` para direita), com base em `left_prob`.
* `if (dir) {`: Verifica se o desvio é à esquerda (`dir = true`).
* `ball->x -= 4.0f;`: Move a bolinha 4 pixels à esquerda (subtrai de `x`).
* `} else {`: Caso contrário (desvio à direita, `dir = false`).
* `ball->x += 4.0f;`: Move a bolinha 4 pixels à direita (adiciona a `x`).
* `}`: Fecha o bloco condicional de direção.
* `ball->collisions++;`: Incrementa o contador de colisões, registrando a colisão atual.
* `}`: Fecha o bloco condicional de colisão.
* `if (ball->x < 0) ball->x = 0;`: Garante que `x` não seja negativo, limitando a bolinha à borda esquerda do display.
* `if (ball->x >= SSD1306_WIDTH) ball->x = SSD1306_WIDTH - 1;`: Garante que `x` não exceda a largura do display (`SSD1306_WIDTH = 128`), limitando à borda direita.
* `if (ball->y >= SSD1306_HEIGHT) {`: Verifica se a bolinha atingiu ou ultrapassou o fundo do display (`SSD1306_HEIGHT = 64`).
* `ball->active = false;`: Desativa a bolinha (`active = false`), indicando que terminou sua trajetória.
* `}`: Fecha o bloco condicional de altura.
* `}`: Fecha a função.

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
Este trecho define a função `register_ball_landing`, que registra a posição final de uma bolinha no histograma.

* `void register_ball_landing(Ball *ball) {`: Declara a função `register_ball_landing`, que recebe um ponteiro para uma estrutura `Ball` e não retorna valor.
* `int bin = (int)(ball->x / (SSD1306_WIDTH / CHANNELS));`: Calcula o índice do bin (0 a 15) onde a bolinha caiu, dividindo sua posição `x` pela largura de cada bin (`SSD1306_WIDTH / CHANNELS = 128 / 16 = 8`) e convertendo para inteiro.
* `if (bin >= 0 && bin < CHANNELS) {`: Verifica se o índice do bin é válido (entre 0 e 15, onde `CHANNELS = 16`), evitando acessos fora do array `histogram`.
* `histogram[bin]++;`: Incrementa a contagem no bin correspondente (`histogram[bin]`), registrando a bolinha no histograma.
* `total_balls++;`: Incrementa a variável global `total_balls`, atualizando o contador total de bolas que caíram.
* `}`: Fecha o bloco condicional.
* `}`: Fecha a função.

**9. Função get_left_probability()**:

```c
float get_left_probability() {
    return left_prob;
}
```
Este trecho define a função `get_left_probability`, que retorna a probabilidade de desvio à esquerda.

* `float get_left_probability() {`: Declara a função `get_left_probability`, que retorna um valor do tipo `float` e não recebe parâmetros.
* `return left_prob;`: Retorna o valor da variável global `left_prob` (definida em `galton.c`), que armazena a probabilidade de uma bolinha desviar à esquerda (ex.: 50.0f para 50%, 60.0f para 60%).
* `}`: Fecha a função.


#### Gerenciador de Exibição display.c:

**1. Inclusões**:

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "display.h"
```
Este trecho importa as bibliotecas necessárias para gerenciar o display OLED SSD1306.

* `#include <stdio.h>`: Inclui a biblioteca padrão de entrada/saída, permitindo o uso de funções como `snprintf` para formatar strings (ex.: "Bolas: %d").
* `#include <string.h>`: Inclui a biblioteca para manipulação de strings, fornecendo `memset` para limpar o buffer do display.
* `#include "pico/stdlib.h"`: Inclui a biblioteca padrão do Pico SDK, fornecendo funções como `gpio_set_function` para configurar pinos I2C.
* `#include "hardware/i2c.h"`: Inclui a biblioteca do Pico SDK para comunicação I2C, usada para enviar comandos e dados ao display SSD1306.
* `#include "ssd1306.h"`: Inclui o cabeçalho da biblioteca SSD1306, que define constantes e funções para interagir com o display (ex.: `ssd1306_set_pixel`).
* `#include "ssd1306_i2c.h"`: Inclui o cabeçalho específico para comunicação I2C com o SSD1306, fornecendo funções como `ssd1306_draw_string`.
* `#include "display.h"`: Inclui o cabeçalho `display.h`, que define protótipos das funções de `display.c` (ex.: `init_display`, `update_display`) e constantes relacionadas.

**2. Definições**:

```c
#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
```
Este trecho define constantes e variáveis para gerenciar o buffer do display OLED SSD1306.

* `#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)`: Define a constante `BUFFER_LENGTH` como o tamanho do buffer do display, calculado como `SSD1306_WIDTH` (128) × `SSD1306_HEIGHT` (64) ÷ 8. Como o SSD1306 usa 1 bit por pixel e 8 bits por byte, o buffer tem 1024 bytes (128 × 64 ÷ 8).
* `static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];`: Declara uma variável estática `display_buffer`, um array de `uint8_t` com tamanho `BUFFER_LENGTH` (1024 bytes). Ela armazena o estado dos pixels do display (1 bit por pixel), usada para desenhar bolinhas, histograma e texto antes de enviar ao SSD1306.

**3. Função clear_display_buffer()**:

```c
void clear_display_buffer() {
    memset(display_buffer, 0, BUFFER_LENGTH);
}
```

**4. Função ssd1306_update_display()**:

```c
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
```
Este trecho define a função `ssd1306_update_display`, que atualiza o display OLED SSD1306 com o conteúdo do buffer.

* `void ssd1306_update_display() {`: Declara a função que atualiza o display, sem retorno.
* `uint8_t command_buffer[2];`: Declara um array de 2 bytes para armazenar comandos I2C.
* `command_buffer[0] = 0x00;`: Define o byte de controle como 0x00 (indica comandos).
* `command_buffer[1] = 0x21;`: Define o comando 0x21 (seta intervalo de colunas).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando via I2C (endereço 0x3C) para iniciar a configuração de colunas.
* `command_buffer[1] = 0x00;`: Define a coluna inicial como 0.
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando para coluna inicial.
* `command_buffer[1] = 0x7F;`: Define a coluna final como 127 (128 colunas).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando para coluna final.
* `command_buffer[1] = 0x22;`: Define o comando 0x22 (seta intervalo de páginas).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando para iniciar configuração de páginas.
* `command_buffer[1] = 0x00;`: Define a página inicial como 0.
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando para página inicial.
* `command_buffer[1] = 0x07;`: Define a página final como 7 (8 páginas, 64 linhas).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Envia o comando para página final.
* `uint8_t data_buffer[1025];`: Declara um array de 1025 bytes para dados do display (1024 bytes do buffer + 1 byte de controle).
* `data_buffer[0] = 0x40;`: Define o byte de controle como 0x40 (indica dados).
* `memcpy(&data_buffer[1], display_buffer, BUFFER_LENGTH);`: Copia o conteúdo do `display_buffer` (1024 bytes) para `data_buffer`, começando do índice 1.
* `i2c_write_blocking(i2c1, 0x3C, data_buffer, BUFFER_LENGTH + 1, false);`: Envia os 1025 bytes (controle + dados) via I2C para o display, atualizando a tela com o conteúdo do `display_buffer`.
* `}`: Fecha a função.

**5. Função ssd1306_setup()**:

```c
void ssd1306_setup() {
    uint8_t init_commands[] = {
        0x00, 0xAE, 0x00, 0xD5, 0x80, 0x00, 0xA8, 0x3F, 0x00, 0xD3, 0x00,
        0x00, 0x40, 0x00, 0x8D, 0x14, 0x00, 0x20, 0x00, 0x00, 0xA1, 0x00,
        0xC8, 0x00, 0xDA, 0x12, 0x00, 0x81, 0xCF, 0x00, 0xD9, 0xF1, 0x00,
        0xDB, 0x40, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xAF
    };
    i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);
}
```
Este trecho define a função `ssd1306_setup`, que inicializa o display OLED SSD1306.

* `void ssd1306_setup() {`: Declara a função `ssd1306_setup`, que não retorna valor, usada para configurar o display.
* `uint8_t init_commands[] = { ... };`: Declara um array de bytes contendo uma sequência de comandos de inicialização para o SSD1306, incluindo:
  - `0x00`: Byte de controle para comandos.
  - `0xAE`: Desliga o display.
  - `0xD5, 0x80`: Configura o clock do display.
  - `0xA8, 0x3F`: Define multiplexação (64 linhas).
  - `0xD3, 0x00`: Define offset de exibição.
  - `0x40`: Define linha inicial.
  - `0x8D, 0x14`: Ativa bomba de carga.
  - `0x20, 0x00`: Define modo de endereçamento horizontal.
  - `0xA1`: Define segmentação de colunas.
  - `0xC8`: Define direção de varredura.
  - `0xDA, 0x12`: Configura pinos de hardware.
  - `0x81, 0xCF`: Ajusta contraste.
  - `0xD9, 0xF1`: Define pré-carga.
  - `0xDB, 0x40`: Ajusta voltagem VCOM.
  - `0xA4`: Ativa exibição do buffer.
  - `0xA6`: Define modo normal (não invertido).
  - `0xAF`: Liga o display.
* `i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);`: Envia a sequência de comandos via I2C (interface `i2c1`, endereço 0x3C) para o SSD1306, configurando o display com as definições acima.
* `}`: Fecha a função.

**6. Função init_display()**:

```c
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
```
Este trecho define a função `init_display`, que inicializa o display OLED SSD1306 e sua comunicação I2C.

* `void init_display() {`: Declara a função `init_display`, que não retorna valor, usada para configurar o display.
* `i2c_init(i2c1, 400 * 1000);`: Inicializa a interface I2C (`i2c1`) com uma velocidade de 400 kHz (400 * 1000 Hz).
* `gpio_set_function(14, GPIO_FUNC_I2C);`: Configura o pino GPIO 14 como SDA (dados) para comunicação I2C.
* `gpio_set_function(15, GPIO_FUNC_I2C);`: Configura o pino GPIO 15 como SCL (clock) para comunicação I2C.
* `gpio_pull_up(14);`: Ativa o resistor de pull-up interno no pino 14 (SDA), necessário para a operação I2C.
* `gpio_pull_up(15);`: Ativa o resistor de pull-up interno no pino 15 (SCL), também para I2C.
* `ssd1306_setup();`: Chama `ssd1306_setup` para enviar comandos de inicialização ao SSD1306, configurando parâmetros como resolução e contraste.
* `clear_display_buffer();`: Chama `clear_display_buffer` para zerar o `display_buffer`, limpando o conteúdo do display.
* `ssd1306_update_display();`: Chama `ssd1306_update_display` para enviar o buffer zerado ao display, garantindo uma tela limpa.
* `clear_display_buffer();`: Zera o buffer novamente, preparando para a próxima atualização.
* `ssd1306_update_display();`: Envia o buffer zerado ao display mais uma vez, assegurando que a tela esteja completamente limpa.
* `}`: Fecha a função.

**7. Função draw_histogram()**:

```c
void draw_histogram(int *histogram) {
    for (int i = 0; i < CHANNELS; i++) {
        if (histogram[i] > 0) {
            int height = histogram[i] / 2;
            if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;
            for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {
                for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {
                    ssd1306_set_pixel(display_buffer, x, y, true);
                }
            }
        }
    }
}
```
Este trecho define a função `draw_histogram`, que desenha o histograma no display OLED SSD1306.

* `void draw_histogram(int *histogram) {`: Declara a função `draw_histogram`, que recebe um ponteiro para o array `histogram` e não retorna valor.
* `for (int i = 0; i < CHANNELS; i++) {`: Itera sobre os 16 bins (`CHANNELS = 16`) do histograma.
* `if (histogram[i] > 0) {`: Verifica se o bin `i` tem bolas registradas (`histogram[i] > 0`).
* `int height = histogram[i] / 2;`: Calcula a altura do histograma para o bin `i`, dividindo a contagem de bolas por 2 (escala para ajustar a exibição).
* `if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;`: Limita a altura máxima a 54 pixels (`SSD1306_HEIGHT = 64` - 10), reservando espaço para texto no topo.
* `for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {`: Itera pelas linhas do display, de `y = 64 - height` até `y = 63`, para desenhar a barra do histograma.
* `for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {`: Itera pelas colunas do bin `i`, de `x = i * 8` a `x = (i + 1) * 8 - 2` (`CHANNEL_WIDTH = 8`), desenhando a largura da barra (7 pixels para evitar sobreposição).
* `ssd1306_set_pixel(display_buffer, x, y, true);`: Define o pixel em `(x, y)` como ativado (`true`) no `display_buffer`, desenhando parte da barra do histograma.
* `}`: Fecha o laço de colunas.
* `}`: Fecha o laço de linhas.
* `}`: Fecha o bloco condicional do bin.
* `}`: Fecha a função.

**8. Função draw_ball()**:

```c
void draw_ball(Ball *ball) {
    if (ball->active) {
        ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);
    }
}
```
Este trecho define a função `draw_ball`, que desenha uma bolinha ativa no display OLED SSD1306.

* `void draw_ball(Ball *ball) {`: Declara a função `draw_ball`, que recebe um ponteiro para uma estrutura `Ball` (definida em `galton.h`) e não retorna valor.
* `if (ball->active) {`: Verifica se a bolinha está ativa (`active = true`), indicando que está caindo e deve ser desenhada.
* `ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);`: Define um pixel no `display_buffer` como ativado (`true`) nas coordenadas `(x, y)` da bolinha, convertendo `ball->x` e `ball->y` (tipo `float`) para `int`. Isso desenha a bolinha como um único pixel no display.
* `}`: Fecha o bloco condicional.
* `}`: Fecha a função.

**9. Função draw_probabilities()**:
```c
void draw_probabilities(float left_prob) {
    char left_buffer[8];
    char right_buffer[8];
    snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);
    snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);
    ssd1306_draw_string(display_buffer, 0, 28, left_buffer);
    ssd1306_draw_string(display_buffer, 104, 28, right_buffer);
}
```
Este trecho define a função `draw_probabilities`, que exibe as probabilidades de desvio no display OLED SSD1306.

* `void draw_probabilities(float left_prob) {`: Declara a função `draw_probabilities`, que recebe a probabilidade de desvio à esquerda (`left_prob`) e não retorna valor.
* `char left_buffer[8];`: Declara um array de 8 caracteres para armazenar a string da probabilidade à esquerda.
* `char right_buffer[8];`: Declara um array de 8 caracteres para a probabilidade à direita.
* `snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);`: Formata `left_prob` (ex.: 60.0f) como uma string sem decimais com símbolo de porcentagem (ex.: "60%") e armazena em `left_buffer`.
* `snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);`: Formata a probabilidade à direita (`100.0f - left_prob`, ex.: 40.0f) como string (ex.: "40%") e armazena em `right_buffer`.
* `ssd1306_draw_string(display_buffer, 0, 28, left_buffer);`: Desenha a string de `left_buffer` (ex.: "60%") no `display_buffer` na posição (x=0, y=28), à esquerda do display.
* `ssd1306_draw_string(display_buffer, 104, 28, right_buffer);`: Desenha a string de `right_buffer` (ex.: "40%") na posição (x=104, y=28), à direita, ajustada para caber no display de 128 pixels.
* `}`: Fecha a função.

**10. Função update_display()**:

```c
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
```
Este trecho define a função `update_display`, que atualiza o display OLED SSD1306 com todos os elementos visuais da simulação.

* `void update_display(Ball *balls, int ball_count, int *histogram) {`: Declara a função `update_display`, que recebe o array de bolinhas (`balls`), o número de bolinhas (`ball_count`), o histograma (`histogram`) e não retorna valor.
* `clear_display_buffer();`: Chama `clear_display_buffer` para zerar o `display_buffer`, limpando o conteúdo anterior do display.
* `for (int i = 0; i < ball_count; i++) {`: Itera sobre as `ball_count` bolinhas (máximo `MAX_BALLS = 10`).
* `draw_ball(&balls[i]);`: Chama `draw_ball` para desenhar a bolinha `balls[i]` no `display_buffer` como um pixel, se estiver ativa.
* `}`: Fecha o laço.
* `draw_histogram(histogram);`: Chama `draw_histogram` para desenhar o histograma no `display_buffer`, com barras proporcionais às contagens em `histogram` (escala `histogram[i] / 2`).
* `char buffer[16];`: Declara um array de 16 caracteres para armazenar a string do contador de bolas.
* `snprintf(buffer, sizeof(buffer), "Bolas: %d", total_balls);`: Formata `total_balls` como uma string (ex.: "Bolas: 100") e armazena em `buffer`.
* `ssd1306_draw_string(display_buffer, 0, 0, buffer);`: Desenha a string de `buffer` (ex.: "Bolas: 100") no `display_buffer` na posição (x=0, y=0), topo do display.
* `draw_probabilities(get_left_probability());`: Chama `draw_probabilities` com a probabilidade atual (`get_left_probability`), desenhando as porcentagens (ex.: "60%" à esquerda, "40%" à direita) no `display_buffer`.
* `ssd1306_update_display();`: Chama `ssd1306_update_display` para enviar o `display_buffer` ao SSD1306 via I2C, atualizando o display com todos os elementos.
* `}`: Fecha a função.


__________________________________________________________________________________________________________________________________________

# GALTON BOARD

This project implements a digital simulation of a **Galton Board** using a Raspberry Pi Pico microcontroller and an SSD1306 OLED display (128x64 pixels), all present on the BitDogLab board. The simulation models the fall of balls through rows of pins, resulting in a binomial distribution, which is visualized as a histogram on the display and analyzed via serial output. The distribution of balls in the histogram, for a sufficiently large number of trials, increasingly approximates the shape of a Gaussian (or normal) curve, denoted as N(0,1) in its standard form, as established by the Central Limit Theorem. This [video](https://www.youtube.com/watch?v=gffrqLjbuIY&t=18s) shows the project running on the BitDogLab.

## 1. What is a Galton Board?

The Galton Board, also known as a Quincunx, is a device invented by Sir Francis Galton in the 19th century to demonstrate the Central Limit Theorem and the binomial distribution. It consists of a vertical board with rows of pins arranged in a triangular pattern. Balls are released from the top at the center of the board and, upon colliding with the pins, randomly deflect to the left or right with equal probability (50%). At the bottom, the balls accumulate in compartments (bins), forming a bell-shaped curve that approximates the shape of a normal distribution for a large number of released balls (trials).

The beauty of the Galton Board lies in its ability to visually and intuitively illustrate fundamental statistical concepts, making it widely used in studies of Probability, Statistics, and Physics. A highly illustrative example of this demonstration can be seen in [this video](https://www.youtube.com/watch?v=L4RMRq1FUcg), where a Galton Board is put into action, highlighting the formation of the bell-shaped curve with thousands of balls. The underlying concept is that predictable outcomes can often be obtained even when dealing with completely random events.

The binomial distribution is a probability distribution that describes the likelihood of a certain number of successes ("S") in a fixed number of independent trials ("n"), where each trial has only two possible outcomes (success or failure) with the same probability ("p" and "1-p") for each trial. Newton and Pascal played significant roles in developing the theory of binomial coefficients, which are fundamental to the binomial distribution.

To describe the binomial distribution, imagine a game with two possible outcomes: winning (success) or losing (failure). If the game is played n times, the binomial distribution can indicate the probability of winning x times (where x is the number of successes). The probability of obtaining exactly x successes in n trials is given by: P(x) = (nCx) * p^x * (1-p)^(n-x), where nCx is the binomial coefficient (combination of n elements taken x at a time), representing the number of ways to achieve x successes among n trials.

Blaise Pascal created Pascal's Triangle, a visual representation of binomial coefficients. Each number in the triangle is the sum of the two numbers above it, and the binomial coefficients can be read directly from the triangle's rows. Isaac Newton expanded on Pascal's work and developed Newton's Binomial Theorem, which provides a formula for expanding a power of a binomial (x + y)^n. This formula uses the binomial coefficients, which are the same as those in Pascal's Triangle. Binomial coefficients are fundamental for calculating probabilities in binomial distribution scenarios, such as hypothesis testing or data analysis. Newton and Pascal significantly contributed to the development and understanding of these coefficients.

## 2. The Digital Simulation (Digital Twin)

The digital simulation developed in this project recreates the behavior of a Galton Board on a 128x64 pixel OLED display, functioning as a "digital twin" of the physical device. The implementation details are described below:

### Simulation Structure
- **Display and Bins**: The 128 horizontal pixels of the display are divided into 16 bins, each 8 pixels wide (128 ÷ 16 = 8). These bins, located at the bottom of the display, collect the balls after their fall.
- **Pins and Rows**: Above the 16 bins, there are 15 rows of pins arranged in a triangular pattern (15 pins in the first row just above the bins, 14 in the row above, and so on).
- **Pin Geometry**: The pins form equilateral triangles with 8-pixel sides. The vertical distance between rows is calculated as the height of an equilateral triangle with an 8-pixel side, or 4 times the square root of 3, approximately 6.93 (7 pixels), but the simulation abstracts this to keep the logic simple, as 15 rows of 7 pixels would not fit in the display's height.
- **Ball Behavior**: Each ball starts at the center of the board (x = 64) and undergoes 15 collisions, randomly deflecting ±4 pixels (left or right) per collision, with a 50% chance for each direction. After 15 collisions, the ball lands in one of the 16 bins.
- **Extreme Cases**:
  - Maximum right: \( 64 + 15 \times 4 = 124 \) (bin 16).
  - Maximum left: \( 64 - 15 \times 4 = 4 \) (bin 1).
- **Visualization**: Balls are displayed as dots on the screen during their fall, and the histogram at the bottom is drawn with rectangles, where every 2 balls increment 1 pixel of height (scaled to avoid filling the screen too quickly).
- **Serial Output**: Every 100 balls, the program displays on the Serial Monitor:
  - Total number of balls.
  - Number of balls in each bin.
  - Mean of the distribution.
  - Standard deviation of the distribution.
- **Probability Control**: Two buttons, connected to GPIO pins 5 (Button A) and 6 (Button B), allow dynamic adjustment of the balls' deflection probabilities. Initially, the probability is 50% left and 50% right. Each press of Button A increases the left deflection probability by 10% (and reduces the right by 10%), up to a maximum of 90% left / 10% right. Button B does the opposite, increasing the right deflection probability up to 10% left / 90% right. The selected percentages are displayed on the sides of the display, allowing visualization of the impact of these changes on the binomial distribution, which becomes asymmetric as probabilities change.

### Features
- The simulation uses the Pico's random number generator (`get_rand_32`) to ensure random deflections.
- Up to 10 balls can be active simultaneously, falling in a continuous "rain."
- Note that the theoretical probability of a ball landing in the extreme bins (1 or 16) is (1/2)^15 × 100%, or approximately 0.003%. There is only one way for a ball to reach bin 16: it must deflect right in all 15 collisions. In contrast, to reach bins 8 or 9 (central), a ball has 6435 (15C7 or 15C8) different paths. The probability calculation is given by the combination of 15 taken 8 at a time (or 7 at a time, which yields the same result), times (1/2)^15, times 100%, or 6435 × 0.003%, approximately 19.64%. The difference in these probabilities (landing in bins 8 or 9 versus bins 1 or 16) clearly reflects the binomial nature of the simulation.

## 3. Serial Monitor Output

Below is the serial monitor output after 200 balls have fallen on the board:

---- Opened the serial port COM4 ----  
Starting Galton Board...  
Total Balls: 100  
Bins: [1]: 0 [2]: 0 [3]: 0 [4]: 2 [5]: 4 [6]: 11 [7]: 8 [8]: 22 [9]: 25 [10]: 17 [11]: 5 [12]: 4 [13]: 2 [14]: 0 [15]: 0 [16]: 0   
Mean: 8.50  
Standard Deviation: 1.86  

Total Balls: 200  
Bins: [1]: 0 [2]: 0 [3]: 0 [4]: 2 [5]: 6 [6]: 23 [7]: 25 [8]: 41 [9]: 46 [10]: 34 [11]: 13 [12]: 6 [13]: 4 [14]: 0 [15]: 0 [16]: 0   
Mean: 8.50  
Standard Deviation: 1.80  

## 4. Code Analysis

### Modularization
The code is organized into modules to promote clarity, reusability, and maintainability. Modularization separates the simulation logic, display interface, and system initialization, offering the following advantages:
- **Clarity**: Each module has a single responsibility, making it easier to understand.
- **Maintenance**: Changes in one module (e.g., adjusting the histogram scale) do not affect others.
- **Reusability**: Functions like `random_direction` can be reused in other projects.
- **Debugging**: Isolating errors in specific modules speeds up fixes.

### Code Structure
The project consists of six main files:

**galton.c / galton.h**:  
   - Contains the logic of the Galton Board simulation.  
   - Main functions:  
     - `init_ball`: Initializes a ball at the center (x = 64, y = 0).  
     - `update_ball`: Moves the ball vertically and performs horizontal deflections (±4 pixels) in 15 collisions.  
     - `register_ball_landing`: Registers the ball in a bin and updates the histogram.  
     - `calculate_statistics`: Calculates and displays (every 100 balls) the total balls, count per bin, mean, and standard deviation.  
     - `test_randomness`: Tests the randomness of the generator (`random_direction`), useful for debugging.  
   - Global variables: `histogram` (ball count per bin) and `total_balls` (total accumulated).

**main.c**:  
   - Orchestrates the simulation, initializing the display, managing up to 10 active balls, and calling `calculate_statistics` every 100 balls.  
   - Main loop: Updates balls, registers landings, and refreshes the display every 50ms.

**display.c / display.h**:  
   - Manages the SSD1306 OLED display.  
   - Main functions:  
     - `init_display`: Configures the display.  
     - `draw_histogram`: Draws the histogram with height proportional to the balls (scale: 1 pixel per 2 balls).  
     - `update_display`: Updates the display with balls, total balls, histogram, and probabilities.

**CMakeLists.txt**:  
The CMakeLists.txt file is used to configure the project’s build process, defining instructions for CMake, a build tool, specifying:  
   - The project name and required CMake version.  
   - Source files (main.c, galton.c, display.c) to be compiled.  
   - Pico SDK libraries (e.g., pico_stdlib, hardware_i2c) to be linked.  
   - Configurations such as USB and UART support for serial output.  
   - Generation of the executable for the Raspberry Pi Pico.

### Technical Details
- **Display**: The SSD1306 is controlled via I2C, with a buffer to render balls (individual pixels), histogram (rectangles per bin), and text.
- **Histogram Scale**: Adjusted to `histogram[i] / 2` for slow growth, preventing rapid screen filling.
- **Serial Output**: Via USB, configured with `stdio_usb_init`, displaying statistics every 100 balls for analysis.
- **Randomness**: The `get_rand_32` ensures uniform deflections (50% left, 50% right), validated by the `test_randomness` function.

### Expected Results
The simulation produces a binomial distribution, with a mean close to 8.5 (center of the 16 bins) and a standard deviation close to 2, reflecting the 15 collisions with 50% probability. After 975 balls, for example, the following was observed:
- Mean: ~8.49
- Standard Deviation: ~1.87
- Peak in bins 8-9, with zero balls in the extremes (bins 1 and 16), consistent with the theoretical probability of 0.003%.

### How to Run
1. **Hardware**:
   - Raspberry Pi Pico (or Pico W).
   - SSD1306 OLED display (128x64, connected via I2C).
2. **Setup**:
   - Install the Pico SDK (version 2.1.1 or higher).
   - Connect the display to the Pico’s I2C pins (SDA to GPIO 14, SCL to GPIO 15).
3. **Compilation**:
   - Clone the repository.
   - Run `cmake` and `make` in the `build` directory.
   - Upload the firmware to the Pico via USB.
4. **Output**:
   - View the ball rain and histogram on the display.
   - Connect to a serial monitor to see statistics every 100 balls.

### Code Explanation:

#### Orchestrator main.c:

**1. Includes:**

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "galton.h"
#include "display.h"
```

* `#include <stdio.h>`: Includes the standard C input/output library, providing functions like `printf` for printing information to the serial monitor.
* `#include "pico/stdlib.h"`: Includes the standard Pico SDK library, offering essential functions for interacting with the Pico hardware, such as peripheral initialization, GPIO control, and timing functions.
* `#include "galton.h"`: Includes the `galton.h` header file, which likely contains declarations for data types (e.g., the `Ball` struct) and function prototypes related to the Galton Board simulation logic (e.g., `init_ball`, `update_ball`).
* `#include "display.h"`: Includes the `display.h` header file, which likely contains prototypes for functions responsible for initializing and updating the simulation visualization (e.g., `init_display`, `update_display`). The absence of an LED matrix suggests visualization is done via the serial monitor or another display device connected to the Pico.

**2. Definitions:**

```c
#define BUTTON_A 5
#define BUTTON_B 6
#define DEBOUNCE_MS 200
```

This section defines constants necessary for hardware interaction. The `BUTTON_A` and `BUTTON_B` macros specify GPIO pins 5 and 6 on the Raspberry Pi Pico, to which physical buttons are connected. The `DEBOUNCE_MS` constant sets a 200-millisecond interval for the debounce mechanism, ensuring only one button press is registered at a time, even in the presence of typical electrical noise from mechanical buttons.

**3. Randomness Test:**

```c
/*
int main() { // Function for testing randomness during development
    stdio_init_all();
    sleep_ms(2000);  // Time to open the serial monitor
    test_randomness(100000); // Adjust the number of trials
}*/
```

In response to a specific project requirement, this commented section represents an alternative `main()` function used early in development to test the quality of the project’s random number generator. The function initializes serial communication with `stdio_init_all()`, waits 2000 milliseconds (`sleep_ms(2000)`) to allow connection to a serial monitor, and calls `test_randomness(100000)`, which runs 100,000 iterations of the `random_direction` function to verify if the left and right deflection probabilities are balanced (i.e., approximately 50% each with the default setting). Results are displayed on the serial monitor, showing the count and percentage of left and right deflections. Though commented out in the final version, this test was crucial for validating the system’s randomness, ensuring the binomial distribution of the simulation accurately reflected the expected probabilities.

Results of randomness tests conducted with the `test_randomness` and `random_direction` functions in `galton.c` (explained later):  
10 trials:  
Left: 5 (50.00%), Right: 5 (50.00%)  
Here, luck played a role. Typically, results show 60% or 70% for one side due to the small number of trials.

100 trials:  
Left: 48 (48.00%), Right: 52 (52.00%)  

1,000 trials:  
Left: 491 (49.10%), Right: 509 (50.90%)  

10,000 trials:  
Left: 4996 (49.96%), Right: 5004 (50.04%)  

100,000 trials:  
Left: 49975 (49.97%), Right: 50025 (50.02%)  

1,000,000 trials:  
Left: 499638 (49.96%), Right: 500362 (50.04%)

10,000,000 trials:  
Left: 5000521 (50.01%), Right: 4999479 (49.99%)  

It is evident that the probabilities of deflections to the right or left tend toward 50% as the number of trials increases.

**4. `main()` Function:**

```c
int main() {
    stdio_usb_init();
    sleep_ms(3000); // Time to open the Serial Monitor
    printf("Starting Galton Board...\n");

    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    init_display();
```

* `int main() {`: Defines the program’s main function, the entry point of execution.
* `stdio_usb_init();`: A Pico SDK function that initializes standard input/output peripherals, typically configuring USB serial communication to allow `printf` to send data to the computer’s serial monitor.
* `sleep_ms(3000);`: A Pico SDK function that pauses program execution for the specified milliseconds (3000 ms or 3 seconds), commonly used to allow time for the IDE’s serial monitor to connect before the program starts sending data.
* `printf("Starting Galton Board...\n");`: Prints an initialization message to the serial monitor.
```c
gpio_init(BUTTON_A);
gpio_init(BUTTON_B);
gpio_set_dir(BUTTON_A, GPIO_IN);
gpio_set_dir(BUTTON_B, GPIO_IN);
gpio_pull_up(BUTTON_A);
gpio_pull_up(BUTTON_B);
```
Initializes GPIO pins 5 and 6, connected to buttons A and B on the board, as inputs with pull-up resistors.
* `init_display();`: Calls a function defined in `display.c` (prototyped in `display.h`) to initialize the SSD1306 display, explained later.

**5. Ball Initialization:**

```c
    Ball balls[MAX_BALLS];
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
```

* `Ball balls[MAX_BALLS];`: Declares an array named `balls` capable of storing `MAX_BALLS` (10) structures of type `Ball` (defined in `galton.h` with attributes like x, y position, active state, and collision count). `MAX_BALLS` is a constant defined in `galton.h` specifying the number of balls simulated simultaneously.
* `for (int i = 0; i < MAX_BALLS; i++) { ... }`: Starts a loop iterating over each element of the `balls` array.
* `balls[i].active = false;`: Initializes each ball, setting `active = false`, ensuring no balls are active at the start of the simulation. This prepares the system to dynamically activate balls during the "ball rain."

**6. Histogram and Ball Count Initialization:**

```c
for (int i = 0; i < CHANNELS; i++) {
        histogram[i] = 0;
    }
    total_balls = 0;
```

* `for (int i = 0; i < CHANNELS; i++) {...}`: Starts a loop iterating from `i = 0` to `i < CHANNELS` (`CHANNELS` is 16, defined in `galton.h`) over the elements of the `histogram` array.
* `histogram[i] = 0;`: Sets the value of `histogram[i]` to 0. The `histogram` array (declared in `galton.c`) stores the count of balls in each of the 16 bins. This command resets all counts, initializing the histogram for the simulation.

* `total_balls = 0;`: Initializes the global variable `total_balls` (declared in `galton.c`) to 0. This variable tracks the total number of balls that have landed in the bins, used for calculating statistics like mean and standard deviation displayed on the serial monitor every 100 balls. It also appears at the top of the SSD1306 OLED display to indicate the total number of simulations.

**7. Extra Definitions:**

```c
extern float left_prob;
static uint32_t last_press_a = 0;
static uint32_t last_press_b = 0;
static bool last_state_a = true;
static bool last_state_b = true;
```

* `extern float left_prob;`: Declares that the `left_prob` variable (defined in `galton.c`) is external, allowing access in `main.c`. `left_prob` stores the probability of a ball deflecting left (e.g., 50.0f for 50%, 70.0f for 70%), adjusted by buttons A and B.

* `static uint32_t last_press_a = 0;`: Declares a static variable `last_press_a` of type `uint32_t`, initialized to 0. It stores the timestamp (in milliseconds) of the last press of button A (GPIO 5). The `static` keyword ensures the value persists between iterations of the main loop. Used for implementing debounce (minimum 200ms interval between presses).

* `static uint32_t last_press_b = 0;`: Similar to `last_press_a`, but for button B (GPIO 6). Stores the timestamp of the last press of button B, also for debounce.

* `static bool last_state_a = true;`: Declares a static variable `last_state_a` of type `bool`, initialized to `true`. Represents the last read state of button A (GPIO 5), where `true` indicates not pressed (due to the internal pull-up) and `false` indicates pressed. Used to detect the falling edge (transition from `true` to `false`) and avoid multiple readings during a press.

* `static bool last_state_b = true;`: Similar to `last_state_a`, but for button B (GPIO 6). Stores the last state of button B, also for detecting the falling edge in the debounce mechanism.

* `int tick = 0;`: Creates and initializes a counter (`tick`) that tracks the number of main loop iterations, incremented every 50ms. It controls the creation of new balls every 250ms, essential for the dynamics of the Galton Board simulation.

**8. Main Simulation Loop:**

```c
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        bool state_a = gpio_get(BUTTON_A);
        bool state_b = gpio_get(BUTTON_B);
```

* `while (true) {`: Starts the main program loop for the Galton Board, which runs continuously (`while (true)`). Within it, the following are executed:

* `uint32_t now = to_ms_since_boot(get_absolute_time());`: Obtains the current time in milliseconds since the program started, stored in `now`, for managing debounce and timing.

* `bool state_a = gpio_get(BUTTON_A);`: Reads the state of button A (GPIO 5), returning `false` (pressed) or `true` (not pressed, due to the pull-up).

* `bool state_b = gpio_get(BUTTON_B);`: Reads the state of button B (GPIO 6), similarly.
These commands monitor the buttons to adjust probabilities and control the loop’s synchronization, which updates the simulation every 50ms.

```c
        if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {
            if (left_prob < 90.0f) {
                left_prob += 10.0f;
            }
            last_press_a = now;
        }

        if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {
            if (left_prob > 10.0f) {
                left_prob -= 10.0f;
            }
            last_press_b = now;
        }

        last_state_a = state_a;
        last_state_b = state_b;
```

This section implements the logic for detecting button A and B presses with debounce, adjusting the ball deflection probabilities.

* `if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {`: Checks if button A is pressed (`!state_a`), was not pressed before (`last_state_a`), and more than 200ms have passed since the last press (`now - last_press_a > DEBOUNCE_MS`), ensuring debounce.

* `if (left_prob < 90.0f) {`: Checks if the left deflection probability (`left_prob`) is less than 90%, preventing it from exceeding the maximum limit.

* `left_prob += 10.0f;`: Increments `left_prob` by 10% (e.g., from 50% to 60%), increasing the chance of left deflection.

* `last_press_a = now;`: Updates the timestamp of the last button A press with the current time (`now`), for the next debounce cycle.

* `}`: Closes the button A conditional block.

* `if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {`: Checks if button B is pressed (`!state_b`), was not pressed before (`last_state_b`), and more than 200ms have passed since the last press, for debounce.

* `if (left_prob > 10.0f) {`: Checks if `left_prob` is greater than 10%, preventing it from falling below the minimum limit.

* `left_prob -= 10.0f;`: Decrements `left_prob` by 10% (e.g., from 50% to 40%), increasing the chance of right deflection.

* `last_press_b = now;`: Updates the timestamp of the last button B press with the current time.

* `}`: Closes the button B conditional block.

* `last_state_a = state_a;`: Stores the current state of button A (`state_a`) in `last_state_a` for the next iteration, allowing detection of the next falling edge.

* `last_state_b = state_b;`: Stores the current state of button B (`state_b`) in `last_state_b` for the next iteration.

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

This section controls the creation of new balls in the simulation, adding a new ball every 250ms if there is an inactive ball in the `balls` array (maximum of 10 active balls). This ensures the continuous "rain" of balls in the Galton Board.

* `if (tick % 5 == 0) {`: Checks if the `tick` counter (incremented every 50ms) is divisible by 5, i.e., every 250ms (5 × 50ms), executing the block to add a new ball.

* `for (int i = 0; i < MAX_BALLS; i++) {`: Starts a loop iterating over the `balls` array (size `MAX_BALLS = 10`), looking for an inactive ball.

* `if (!balls[i].active) {`: Checks if the ball at index `i` is not active (`active = false`), indicating it can be used.

* `init_ball(&balls[i]);`: Calls the `init_ball` function (defined in `galton.c`) to initialize the ball `balls[i]`, setting its initial position (x=64, y=0), activating it (`active = true`), and resetting collisions.

* `break;`: Exits the `for` loop after activating one ball, preventing multiple balls from being initialized in the same cycle.

* `}`: Closes the inactive ball check conditional.

* `}`: Closes the `for` loop.

* `}`: Closes the `tick` conditional block.

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

This section manages the updating of active balls in the simulation. It updates all active balls every main loop iteration (~50ms), moving them and processing collisions. When a ball reaches the bottom, it is registered in the histogram, and every 100 balls, statistics are calculated and displayed serially, updating the Galton Board simulation.

* `for (int i = 0; i < MAX_BALLS; i++) {`: Starts a loop iterating over the `balls` array (size `MAX_BALLS = 10`), checking each ball.

* `if (balls[i].active) {`: Checks if the ball at index `i` is active (`active = true`), indicating it is falling in the simulation.

* `update_ball(&balls[i]);`: Calls `update_ball` (in `galton.c`) to update the ball, moving it vertically (increments `y`), applying horizontal deflections (±4 pixels) at collisions, and deactivating it (`active = false`) if it reaches the bottom (y ≥ 64).

* `if (!balls[i].active) {`: Checks if the ball was deactivated after `update_ball` (i.e., it reached the bottom).

* `register_ball_landing(&balls[i]);`: Calls `register_ball_landing` (in `galton.c`) to register the ball in the corresponding bin (based on `x`), incrementing `histogram[bin]` and `total_balls`.

* `if (total_balls % 100 == 0 && total_balls > 0) {`: Checks if the total number of balls (`total_balls`) is divisible by 100 and greater than 0, indicating 100 balls have landed since the last statistics.

* `calculate_statistics();`: Calls `calculate_statistics` (in `galton.c`) to calculate and display via serial the total balls, count per bin, mean, and standard deviation.

* `}`: Closes the statistics conditional block.

* `}`: Closes the deactivated ball conditional block.

* `}`: Closes the active ball conditional block.

* `}`: Closes the `for` loop.

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

This section completes the main loop, ensuring histogram integrity, updating the display, and controlling timing.

* `for (int i = 0; i < CHANNELS; i++) {`: Starts a loop iterating over the 16 bins of the `histogram` array (`CHANNELS = 16`, defined in `galton.h`).

* `if (histogram[i] < 0) histogram[i] = 0;`: Checks if the value of `histogram[i]` is negative (an unlikely error, but possible during debugging) and, if so, corrects it to 0, ensuring valid histogram counts.

* `}`: Closes the `for` loop.

* `update_display(balls, MAX_BALLS, histogram);`: Calls `update_display` (in `display.c`) to update the OLED display, drawing active balls (`balls`), the histogram (`histogram`, scaled `histogram[i] / 2`), the ball counter (`total_balls`), and probabilities (e.g., "60%" left, "40%" right).

* `tick++;`: Increments the `tick` counter, tracking the number of main loop iterations, used to time the creation of new balls (every 250ms, when `tick % 5 == 0`).

* `sleep_ms(50);`: Pauses execution for 50 milliseconds, ensuring the main loop runs every ~50ms, controlling the simulation’s update rate.

* `}`: Closes the `while (true)` loop, which runs indefinitely.

* `return 0;`: Indicates successful termination of the `main` function (though never reached due to the `while (true)`).

* `}`: Closes the `main` function.

#### Simulator galton.c:

**1. Includes:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "galton.h"
```

This section defines the libraries necessary for implementing the simulation logic, including randomness, statistical calculations, and hardware interaction.

* `#include <stdio.h>`: Includes the standard input/output library, enabling functions like `printf` for displaying statistics on the serial output (e.g., in `calculate_statistics`).
* `#include <stdlib.h>`: Includes the standard library, providing functions like `get_rand_32` (used indirectly via `pico/rand.h`) for random number generation.
* `#include <math.h>`: Includes the math library, providing functions like `sqrtf` for standard deviation calculations in `calculate_statistics`.
* `#include "pico/stdlib.h"`: Includes the Pico SDK standard library, providing functions like `sleep_ms` and GPIO access, used elsewhere in the project.
* `#include "pico/rand.h"`: Includes the Pico SDK’s random number generation library, providing `get_rand_32` for the `random_direction` function, essential for simulating random deflections.
* `#include "galton.h"`: Includes the `galton.h` header file, defining constants (e.g., `MAX_BALLS`, `CHANNELS`), the `Ball` structure, and function prototypes used in `galton.c` (e.g., `init_ball`, `update_ball`).

**2. Definitions:**

```c
int histogram[CHANNELS] = {0};
int total_balls = 0;
float left_prob = 50.0f;
```

This section defines and initializes global variables for the simulation.

* `int histogram[CHANNELS] = {0};`: Declares and initializes a global array `histogram` with size `CHANNELS` (16, defined in `galton.h`), filling all elements with 0. It stores the count of balls landing in each bin of the Galton Board.
* `int total_balls = 0;`: Declares and initializes the global variable `total_balls` to 0. It tracks the total number of balls that have landed in the bins, used for statistics and display.
* `float left_prob = 50.0f;`: Declares and initializes the global variable `left_prob` to 50.0f (50%), setting the initial probability of a ball deflecting left at each collision. It is dynamically adjusted by buttons A and B in `main.c`.

**3. `random_direction()` Function:**

```c
bool random_direction() {
    return (get_rand_32() % 100) < left_prob;
}
```

This section defines the `random_direction` function, which randomly determines a ball’s deflection direction.

* `bool random_direction() {`: Declares the `random_direction` function, returning a boolean (`true` for left deflection, `false` for right).
* `return (get_rand_32() % 100) < left_prob;`: Generates a random number from 0 to 99 using `get_rand_32() % 100` (where `get_rand_32` is from `pico/rand.h`) and compares it with `left_prob` (e.g., 50.0f for 50%, 60.0f for 60%). Returns `true` if the number is less than `left_prob`, indicating left deflection (e.g., for 60%, `true` if `get_rand_32() % 100` < 60). Otherwise, returns `false` (right deflection).
* `}`: Closes the function.

**4. `test_randomness()` Function:**

```c
void test_randomness(int trials) {
    int left = 0, right = 0;
    for (int i = 0; i < trials; i++) {
        if (random_direction() == 0) right++;
        else left++;
    }
    printf("Left: %d (%.2f%%), Right: %d (%.2f%%)\n",
            left, (float)left / trials * 100,
            right, (float)right / trials * 100);
}
```

This section defines the `test_randomness` function to test the distribution of random deflections.

* `void test_randomness(int trials) {`: Declares the `test_randomness` function, taking a `trials` parameter (number of simulations) and returning no value, only printing results to the serial monitor.
* `int left = 0, right = 0;`: Declares and initializes two local variables, `left` and `right`, to count left and right deflections, respectively.
* `for (int i = 0; i < trials; i++) {`: Starts a loop executing `trials` iterations to test the `random_direction` function.
* `if (random_direction() == 0) right++;`: Calls `random_direction`; if it returns `false` (0, right deflection), increments `right`.
* `else left++;`: If `random_direction` returns `true` (left deflection), increments `left`.
* `}`: Closes the `for` loop.
* `printf("Left: %d (%.2f%%), Right: %d (%.2f%%)\n", ...);`: Displays via serial the number of left (`left`) and right (`right`) deflections, with their percentages calculated as `(float)left / trials * 100` and `(float)right / trials * 100`, formatted to two decimal places.
* `}`: Closes the function.

**5. `calculate_statistics()` Function:**

```c
void calculate_statistics() {
    if (total_balls == 0) {
        printf("No balls recorded.\n");
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

    printf("Total Balls: %d\n", total_balls);
    printf("Bins: ");
    for (int i = 0; i < CHANNELS; i++) {
        printf("[%d]: %d ", i + 1, histogram[i]);
    }
    printf("\nMean: %.2f\nStandard Deviation: %.2f\n", mean, std_dev);
}
```

This section defines the `calculate_statistics` function, which calculates and displays simulation statistics on the serial monitor every 100 balls.

* `void calculate_statistics() {`: Declares the `calculate_statistics` function, which returns no value and processes histogram data.
* `if (total_balls == 0) {`: Checks if no balls have been recorded (`total_balls == 0`).
* `printf("No balls recorded.\n");`: Displays a message via serial if no balls are present.
* `return;`: Exits the function if `total_balls` is 0, avoiding invalid calculations.
* `float mean = 0.0f;`: Initializes the `mean` variable (mean) to 0.0.
* `for (int i = 0; i < CHANNELS; i++) {`: Iterates over the 16 bins (`CHANNELS = 16`).
* `mean += (i + 1) * histogram[i];`: Accumulates the weighted sum of bins (bin `i+1` multiplied by the count `histogram[i]`) to calculate the mean.
* `}`: Closes the loop.
* `mean /= total_balls;`: Divides the sum by the total number of balls to obtain the mean.
* `float variance = 0.0f;`: Initializes the `variance` variable (variance) to 0.0.
* `for (int i = 0; i < CHANNELS; i++) {`: Iterates over the bins again.
* `variance += histogram[i] * ((i + 1) - mean) * ((i + 1) - mean);`: Accumulates the sum of squared differences between each bin (`i+1`) and the mean, weighted by `histogram[i]`, to calculate variance.
* `}`: Closes the loop.
* `variance /= total_balls;`: Divides the sum by the total number of balls to obtain the variance.
* `float std_dev = sqrtf(variance);`: Calculates the standard deviation as the square root of the variance, using `sqrtf` from `math.h`.
* `printf("Total Balls: %d\n", total_balls);`: Displays the total number of balls via serial.
* `printf("Bins: ");`: Starts displaying the bin counts.
* `for (int i = 0; i < CHANNELS; i++) {`: Iterates over the bins to display counts.
* `printf("[%d]: %d ", i + 1, histogram[i]);`: Displays the bin number (`i+1`) and its count (`histogram[i]`).
* `}`: Closes the loop.
* `printf("\nMean: %.2f\nStandard Deviation: %.2f\n", mean, std_dev);`: Displays the mean and standard deviation, formatted to two decimal places.

**6. `init_ball()` Function:**

```c
void init_ball(Ball *ball) {
    ball->x = SSD1306_WIDTH / 2.0f;
    ball->y = 0.0f;
    ball->active = true;
    ball->collisions = 0;
}
```

This section defines the `init_ball` function, which initializes a ball for the simulation.

* `void init_ball(Ball *ball) {`: Declares the `init_ball` function, taking a pointer to a `Ball` structure (defined in `galton.h`) and returning no value.
* `ball->x = SSD1306_WIDTH / 2.0f;`: Sets the ball’s horizontal coordinate `x` to the center of the display (`SSD1306_WIDTH = 128`, so `x = 64.0f`), the initial drop point.
* `ball->y = 0.0f;`: Sets the vertical coordinate `y` to 0, positioning the ball at the top of the display.
* `ball->active = true;`: Marks the ball as active (`active = true`), indicating it is in motion in the simulation.
* `ball->collisions = 0;`: Initializes the collision counter to 0, tracking how many times the ball has deflected (maximum 15).
* `}`: Closes the function.

**7. `update_ball()` Function:**

```c
void update_ball(Ball *ball) {
    if (!ball->active) return;

    ball->y += 1.0f;
    if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {
        bool dir = random_direction();
        if (dir) {
            ball->x -= 4.0f;
        } else {
            ball->x += 4.0f;
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

This section defines the `update_ball` function, which updates a ball’s state during the simulation.

* `void update_ball(Ball *ball) {`: Declares the `update_ball` function, taking a pointer to a `Ball` structure and returning no value.
* `if (!ball->active) return;`: Checks if the ball is inactive (`active = false`); if so, exits the function without changes.
* `ball->y += 1.0f;`: Increments the vertical coordinate `y` by 1 pixel, moving the ball downward.
* `if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {`: Checks if the ball can still collide (`collisions < 15`) and if it has reached the height of the next collision (calculated as `(collisions + 1) * (64 / 15) ≈ 4.27` pixels).
* `bool dir = random_direction();`: Calls `random_direction` to randomly decide the deflection direction (`true` for left, `false` for right), based on `left_prob`.
* `if (dir) {`: Checks if the deflection is left (`dir = true`).
* `ball->x -= 4.0f;`: Moves the ball 4 pixels left (subtracts from `x`).
* `} else {`: Otherwise (right deflection, `dir = false`).
* `ball->x += 4.0f;`: Moves the ball 4 pixels right (adds to `x`).
* `}`: Closes the direction conditional block.
* `ball->collisions++;`: Increments the collision counter, recording the current collision.
* `}`: Closes the collision conditional block.
* `if (ball->x < 0) ball->x = 0;`: Ensures `x` is not negative, limiting the ball to the display’s left edge.
* `if (ball->x >= SSD1306_WIDTH) ball->x = SSD1306_WIDTH - 1;`: Ensures `x` does not exceed the display width (`SSD1306_WIDTH = 128`), limiting to the right edge.
* `if (ball->y >= SSD1306_HEIGHT) {`: Checks if the ball has reached or exceeded the bottom of the display (`SSD1306_HEIGHT = 64`).
* `ball->active = false;`: Deactivates the ball (`active = false`), indicating its trajectory is complete.
* `}`: Closes the height conditional block.
* `}`: Closes the function.

**8. `register_ball_landing()` Function:**

```c
void register_ball_landing(Ball *ball) {
    int bin = (int)(ball->x / (SSD1306_WIDTH / CHANNELS));
    if (bin >= 0 && bin < CHANNELS) {
        histogram[bin]++;
        total_balls++;
    }
}
```

This section defines the `register_ball_landing` function, which records a ball’s final position in the histogram.

* `void register_ball_landing(Ball *ball) {`: Declares the `register_ball_landing` function, taking a pointer to a `Ball` structure and returning no value.
* `int bin = (int)(ball->x / (SSD1306_WIDTH / CHANNELS));`: Calculates the bin index (0 to 15) where the ball landed, dividing its `x` position by the width of each bin (`SSD1306_WIDTH / CHANNELS = 128 / 16 = 8`) and converting to an integer.
* `if (bin >= 0 && bin < CHANNELS) {`: Checks if the bin index is valid (between 0 and 15, where `CHANNELS = 16`), preventing out-of-bounds access to the `histogram` array.
* `histogram[bin]++;`: Increments the count in the corresponding bin (`histogram[bin]`), recording the ball in the histogram.
* `total_balls++;`: Increments the global `total_balls` variable, updating the total count of landed balls.
* `}`: Closes the conditional block.
* `}`: Closes the function.

**9. `get_left_probability()` Function:**

```c
float get_left_probability() {
    return left_prob;
}
```

This section defines the `get_left_probability` function, which returns the left deflection probability.

* `float get_left_probability() {`: Declares the `get_left_probability` function, returning a `float` and taking no parameters.
* `return left_prob;`: Returns the value of the global `left_prob` variable (defined in `galton.c`), storing the probability of a ball deflecting left (e.g., 50.0f for 50%, 60.0f for 60%).
* `}`: Closes the function.

#### Display Manager display.c:

**1. Includes:**

```c
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "display.h"
```

This section imports the libraries necessary for managing the SSD1306 OLED display.

* `#include <stdio.h>`: Includes the standard input/output library, enabling functions like `snprintf` for formatting strings (e.g., "Balls: %d").
* `#include <string.h>`: Includes the string manipulation library, providing `memset` for clearing the display buffer.
* `#include "pico/stdlib.h"`: Includes the Pico SDK standard library, providing functions like `gpio_set_function` for configuring I2C pins.
* `#include "hardware/i2c.h"`: Includes the Pico SDK library for I2C communication, used to send commands and data to the SSD1306 display.
* `#include "ssd1306.h"`: Includes the SSD1306 library header, defining constants and functions for interacting with the display (e.g., `ssd1306_set_pixel`).
* `#include "ssd1306_i2c.h"`: Includes the header specific to I2C communication with the SSD1306, providing functions like `ssd1306_draw_string`.
* `#include "display.h"`: Includes the `display.h` header, defining prototypes for `display.c` functions (e.g., `init_display`, `update_display`) and related constants.

**2. Definitions:**

```c
#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
```

This section defines constants and variables for managing the SSD1306 OLED display buffer.

* `#define BUFFER_LENGTH (SSD1306_WIDTH * SSD1306_HEIGHT / 8)`: Defines the constant `BUFFER_LENGTH` as the display buffer size, calculated as `SSD1306_WIDTH` (128) × `SSD1306_HEIGHT` (64) ÷ 8. Since the SSD1306 uses 1 bit per pixel and 8 bits per byte, the buffer is 1024 bytes (128 × 64 ÷ 8).
* `static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];`: Declares a static variable `display_buffer`, a `uint8_t` array with size `BUFFER_LENGTH` (1024 bytes). It stores the pixel states of the display (1 bit per pixel), used to draw balls, histogram, and text before sending to the SSD1306.

**3. `clear_display_buffer()` Function:**

```c
void clear_display_buffer() {
    memset(display_buffer, 0, BUFFER_LENGTH);
}
```

**4. `ssd1306_update_display()` Function:**

```c
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
```

This section defines the `ssd1306_update_display` function, which updates the SSD1306 OLED display with the buffer’s contents.

* `void ssd1306_update_display() {`: Declares the function that updates the display, with no return.
* `uint8_t command_buffer[2];`: Declares a 2-byte array to store I2C commands.
* `command_buffer[0] = 0x00;`: Sets the control byte to 0x00 (indicating commands).
* `command_buffer[1] = 0x21;`: Sets the command 0x21 (sets column range).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the command via I2C (address 0x3C) to start column configuration.
* `command_buffer[1] = 0x00;`: Sets the starting column to 0.
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the starting column command.
* `command_buffer[1] = 0x7F;`: Sets the ending column to 127 (128 columns).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the ending column command.
* `command_buffer[1] = 0x22;`: Sets the command 0x22 (sets page range).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the command to start page configuration.
* `command_buffer[1] = 0x00;`: Sets the starting page to 0.
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the starting page command.
* `command_buffer[1] = 0x07;`: Sets the ending page to 7 (8 pages, 64 rows).
* `i2c_write_blocking(i2c1, 0x3C, command_buffer, 2, false);`: Sends the ending page command.
* `uint8_t data_buffer[1025];`: Declares a 1025-byte array for display data (1024 buffer bytes + 1 control byte).
* `data_buffer[0] = 0x40;`: Sets the control byte to 0x40 (indicating data).
* `memcpy(&data_buffer[1], display_buffer, BUFFER_LENGTH);`: Copies the `display_buffer` contents (1024 bytes) to `data_buffer`, starting at index 1.
* `i2c_write_blocking(i2c1, 0x3C, data_buffer, BUFFER_LENGTH + 1, false);`: Sends the 1025 bytes (control + data) via I2C to the display, updating the screen with the `display_buffer` contents.
* `}`: Closes the function.

**5. `ssd1306_setup()` Function:**

```c
void ssd1306_setup() {
    uint8_t init_commands[] = {
        0x00, 0xAE, 0x00, 0xD5, 0x80, 0x00, 0xA8, 0x3F, 0x00, 0xD3, 0x00,
        0x00, 0x40, 0x00, 0x8D, 0x14, 0x00, 0x20, 0x00, 0x00, 0xA1, 0x00,
        0xC8, 0x00, 0xDA, 0x12, 0x00, 0x81, 0xCF, 0x00, 0xD9, 0xF1, 0x00,
        0xDB, 0x40, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xAF
    };
    i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);
}
```

This section defines the `ssd1306_setup` function, which initializes the SSD1306 OLED display.

* `void ssd1306_setup() {`: Declares the `ssd1306_setup` function, which returns no value and configures the display.
* `uint8_t init_commands[] = { ... };`: Declares a byte array containing a sequence of initialization commands for the SSD1306, including:
  - `0x00`: Control byte for commands.
  - `0xAE`: Turns off the display.
  - `0xD5, 0x80`: Configures the display clock.
  - `0xA8, 0x3F`: Sets multiplexing (64 rows).
  - `0xD3, 0x00`: Sets display offset.
  - `0x40`: Sets the starting line.
  - `0x8D, 0x14`: Enables charge pump.
  - `0x20, 0x00`: Sets horizontal addressing mode.
  - `0xA1`: Sets column segmentation.
  - `0xC8`: Sets scan direction.
  - `0xDA, 0x12`: Configures hardware pins.
  - `0x81, 0xCF`: Adjusts contrast.
  - `0xD9, 0xF1`: Sets pre-charge.
  - `0xDB, 0x40`: Adjusts VCOM voltage.
  - `0xA4`: Enables buffer display.
  - `0xA6`: Sets normal mode (non-inverted).
  - `0xAF`: Turns on the display.
* `i2c_write_blocking(i2c1, 0x3C, init_commands, sizeof(init_commands), false);`: Sends the command sequence via I2C (interface `i2c1`, address 0x3C) to the SSD1306, configuring the display with the above settings.
* `}`: Closes the function.

**6. `init_display()` Function:**

```c
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
```

This section defines the `init_display` function, which initializes the SSD1306 OLED display and its I2C communication.

* `void init_display() {`: Declares the `init_display` function, which returns no value and configures the display.
* `i2c_init(i2c1, 400 * 1000);`: Initializes the I2C interface (`i2c1`) at 400 kHz (400 * 1000 Hz).
* `gpio_set_function(14, GPIO_FUNC_I2C);`: Configures GPIO pin 14 as SDA (data) for I2C communication.
* `gpio_set_function(15, GPIO_FUNC_I2C);`: Configures GPIO pin 15 as SCL (clock) for I2C communication.
* `gpio_pull_up(14);`: Enables the internal pull-up resistor on pin 14 (SDA), required for I2C operation.
* `gpio_pull_up(15);`: Enables the internal pull-up resistor on pin 15 (SCL), also for I2C.
* `ssd1306_setup();`: Calls `ssd1306_setup` to send initialization commands to the SSD1306, configuring settings like resolution and contrast.
* `clear_display_buffer();`: Calls `clear_display_buffer` to zero the `display_buffer`, clearing the display’s contents.
* `ssd1306_update_display();`: Calls `ssd1306_update_display` to send the zeroed buffer to the display, ensuring a clean screen.
* `clear_display_buffer();`: Zeros the buffer again, preparing for the next update.
* `ssd1306_update_display();`: Sends the zeroed buffer to the display again, ensuring the screen is completely clear.
* `}`: Closes the function.

**7. `draw_histogram()` Function:**

```c
void draw_histogram(int *histogram) {
    for (int i = 0; i < CHANNELS; i++) {
        if (histogram[i] > 0) {
            int height = histogram[i] / 2;
            if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;
            for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {
                for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {
                    ssd1306_set_pixel(display_buffer, x, y, true);
                }
            }
        }
    }
}
```

This section defines the `draw_histogram` function, which draws the histogram on the SSD1306 OLED display.

* `void draw_histogram(int *histogram) {`: Declares the `draw_histogram` function, taking a pointer to the `histogram` array and returning no value.
* `for (int i = 0; i < CHANNELS; i++) {`: Iterates over the 16 bins (`CHANNELS = 16`) of the histogram.
* `if (histogram[i] > 0) {`: Checks if bin `i` has recorded balls (`histogram[i] > 0`).
* `int height = histogram[i] / 2;`: Calculates the histogram height for bin `i`, dividing the ball count by 2 (scaled for display).
* `if (height > SSD1306_HEIGHT - 10) height = SSD1306_HEIGHT - 10;`: Limits the maximum height to 54 pixels (`SSD1306_HEIGHT = 64` - 10), reserving space for text at the top.
* `for (int y = SSD1306_HEIGHT - height; y < SSD1306_HEIGHT; y++) {`: Iterates over the display rows, from `y = 64 - height` to `y = 63`, to draw the histogram bar.
* `for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH - 1; x++) {`: Iterates over the columns of bin `i`, from `x = i * 8` to `x = (i + 1) * 8 - 2` (`CHANNEL_WIDTH = 8`), drawing the bar’s width (7 pixels to avoid overlap).
* `ssd1306_set_pixel(display_buffer, x, y, true);`: Sets the pixel at `(x, y)` to active (`true`) in the `display_buffer`, drawing part of the histogram bar.
* `}`: Closes the column loop.
* `}`: Closes the row loop.
* `}`: Closes the bin conditional block.
* `}`: Closes the function.

**8. `draw_ball()` Function:**

```c
void draw_ball(Ball *ball) {
    if (ball->active) {
        ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);
    }
}
```

This section defines the `draw_ball` function, which draws an active ball on the SSD1306 OLED display.

* `void draw_ball(Ball *ball) {`: Declares the `draw_ball` function, taking a pointer to a `Ball` structure (defined in `galton.h`) and returning no value.
* `if (ball->active) {`: Checks if the ball is active (`active = true`), indicating it is falling and should be drawn.
* `ssd1306_set_pixel(display_buffer, (int)ball->x, (int)ball->y, true);`: Sets a pixel in the `display_buffer` to active (`true`) at the ball’s coordinates `(x, y)`, converting `ball->x` and `ball->y` (type `float`) to `int`. This draws the ball as a single pixel on the display.
* `}`: Closes the conditional block.
* `}`: Closes the function.

**9. `draw_probabilities()` Function:**

```c
void draw_probabilities(float left_prob) {
    char left_buffer[8];
    char right_buffer[8];
    snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);
    snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);
    ssd1306_draw_string(display_buffer, 0, 28, left_buffer);
    ssd1306_draw_string(display_buffer, 104, 28, right_buffer);
}
```

This section defines the `draw_probabilities` function, which displays deflection probabilities on the SSD1306 OLED display.

* `void draw_probabilities(float left_prob) {`: Declares the `draw_probabilities` function, taking the left deflection probability (`left_prob`) and returning no value.
* `char left_buffer[8];`: Declares an 8-character array to store the left probability string.
* `char right_buffer[8];`: Declares an 8-character array for the right probability string.
* `snprintf(left_buffer, sizeof(left_buffer), "%.0f%%", left_prob);`: Formats `left_prob` (e.g., 60.0f) as a string with no decimals and a percentage sign (e.g., "60%") and stores it in `left_buffer`.
* `snprintf(right_buffer, sizeof(right_buffer), "%.0f%%", 100.0f - left_prob);`: Formats the right probability (`100.0f - left_prob`, e.g., 40.0f) as a string (e.g., "40%") and stores it in `right_buffer`.
* `ssd1306_draw_string(display_buffer, 0, 28, left_buffer);`: Draws the `left_buffer` string (e.g., "60%") in the `display_buffer` at position (x=0, y=28), on the left side of the display.
* `ssd1306_draw_string(display_buffer, 104, 28, right_buffer);`: Draws the `right_buffer` string (e.g., "40%") at position (x=104, y=28), on the right side, adjusted to fit the 128-pixel display.
* `}`: Closes the function.

**10. `update_display()` Function:**

```c
void update_display(Ball *balls, int ball_count, int *histogram) {
    clear_display_buffer();
    for (int i = 0; i < ball_count; i++) {
        draw_ball(&balls[i]);
    }
    draw_histogram(histogram);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Balls: %d", total_balls);
    ssd1306_draw_string(display_buffer, 0, 0, buffer);
    draw_probabilities(get_left_probability());
    ssd1306_update_display();
}
```

This section defines the `update_display` function, which updates the SSD1306 OLED display with all visual elements of the simulation.

* `void update_display(Ball *balls, int ball_count, int *histogram) {`: Declares the `update_display` function, taking the array of balls (`balls`), the number of balls (`ball_count`), the histogram (`histogram`), and returning no value.
* `clear_display_buffer();`: Calls `clear_display_buffer` to zero the `display_buffer`, clearing the previous display contents.
* `for (int i = 0; i < ball_count; i++) {`: Iterates over the `ball_count` balls (maximum `MAX_BALLS = 10`).
* `draw_ball(&balls[i]);`: Calls `draw_ball` to draw the ball `balls[i]` in the `display_buffer` as a pixel, if active.
* `}`: Closes the loop.
* `draw_histogram(histogram);`: Calls `draw_histogram` to draw the histogram in the `display_buffer`, with bars proportional to the counts in `histogram` (scaled `histogram[i] / 2`).
* `char buffer[16];`: Declares a 16-character array to store the ball counter string.
* `snprintf(buffer, sizeof(buffer), "Balls: %d", total_balls);`: Formats `total_balls` as a string (e.g., "Balls: 100") and stores it in `buffer`.
* `ssd1306_draw_string(display_buffer, 0, 0, buffer);`: Draws the `buffer` string (e.g., "Balls: 100") in the `display_buffer` at position (x=0, y=0), at the top of the display.
* `draw_probabilities(get_left_probability());`: Calls `draw_probabilities` with the current probability (`get_left_probability`), drawing the percentages (e.g., "60%" left, "40%" right) in the `display_buffer`.
* `ssd1306_update_display();`: Calls `ssd1306_update_display` to send the `display_buffer` to the SSD1306 via I2C, updating the display with all elements.
* `}`: Closes the function.