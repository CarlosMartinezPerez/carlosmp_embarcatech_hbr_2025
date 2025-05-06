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

* **`#include`** `<stdio.h>`: Inclui a biblioteca padrão de entrada e saída do C. Isso fornece funções como `printf` para imprimir informações no monitor serial.

* *`#include`* `<stdio.h>`: Inclui a biblioteca padrão de entrada e saída do C. Isso fornece funções como `printf` para imprimir informações no monitor serial.

* `#include <stdio.h>`: Inclui a biblioteca padrão de entrada e saída do C. Isso fornece funções como `printf` para imprimir informações no monitor serial.

