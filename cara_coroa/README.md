# Coin Flip Test with `rand()` in C

This repository contains a small C program that simulates a coin flip to test the `rand()` function from the standard library for the absence of bias in "Heads" or "Tails" outcomes. It is part of the studies for the galton_board project.

## Source Code

The main file `sorteio_moeda.c` contains the C code that:

1.  Initializes the random number generator with the current time (`srand(time(NULL))`).
2.  Performs a series of tests with different numbers of flips (10, 100, 1000, 10000, 100000, 1 million, and 10 million).
3.  For each test, counts the number of times "Heads" (represented by 0) and "Tails" (represented by 1) are flipped using `rand() % 2`.
4.  Calculates and prints the percentage of each outcome for each test.
5.  Includes support for Raspberry Pi Pico, utilizing `pico/stdlib.h` for serial I/O initialization and a small initial delay.

## Test Results

The following are the results obtained by running the program with the different numbers of flips:

--- Test with 10 raffles ---
Heads: 7 (70.00%)
Tails: 3 (30.00%)

--- Test with 100 raffles ---
Heads: 60 (60.00%)
Tails: 40 (40.00%)

--- Test with 1000 raffles ---
Heads: 515 (51.50%)
Tails: 485 (48.50%)

--- Test with 10000 raffles ---
Heads: 4975 (49.75%)
Tails: 5025 (50.25%)

--- Test with 100000 raffles ---
Heads: 49657 (49.66%)
Tails: 50343 (50.34%)

--- Test with 1000000 raffles ---
Heads: 500325 (50.03%)
Tails: 499675 (49.97%)

--- Test with 10000000 raffles ---
Heads: 4998719 (49.99%)
Tails: 5001281 (50.01%)


## Observations

* As expected, as the number of flips increases, the percentages of "Heads" and "Tails" tend to approach 50%, indicating that the `rand()` function (when used with the modulo operator `% 2`) provides a reasonably uniform distribution for this type of simulation, without an apparent bias.
* The code includes support for Raspberry Pi Pico, utilizing the `pico/stdlib.h` library to initialize serial communication and introduce a small delay at the beginning. The output is directed to the serial monitor when run on the Pico.
* To compile and run in a standard environment (non-Pico), you can use a C compiler like GCC. For the Raspberry Pi Pico, a development environment with the Pico SDK configured is necessary.

## How to Use

**To compile and run in a standard environment (e.g., Linux, macOS, Windows with GCC):**

1.  Save the code as `sorteio_moeda.c`.
2.  Open a terminal or command prompt.
3.  Compile the code: `gcc sorteio_moeda.c -o sorteio_moeda`
4.  Run the program: `./sorteio_moeda`

**To use on the Raspberry Pi Pico:**

1.  Ensure you have the Raspberry Pi Pico development environment set up.
2.  Compile the code using the specific Pico SDK process (usually involving `cmake` and `make`).
3.  Upload the generated `.uf2` file to your Raspberry Pi Pico.
4.  Open the serial monitor in your IDE to view the output.

This is a simple test to illustrate the randomness of the `rand()` function for a basic task. For applications requiring cryptographically secure randomness, more robust methods should be considered.

________________________________________________________________________________________

# Teste de Sorteio Cara ou Coroa com `rand()` em C

Este repositório contém um pequeno programa em C que simula o lançamento de uma moeda para testar a função `rand()` da biblioteca padrão quanto à ausência de viés em sorteios de "Cara" ou "Coroa". Ele é parte dos estudos para o projeto galton_board.

## Código Fonte

O arquivo principal `sorteio_moeda.c` contém o código em C que:

1.  Inicializa o gerador de números aleatórios com o tempo atual (`srand(time(NULL))`).
2.  Realiza uma série de testes com diferentes números de sorteios (10, 100, 1000, 10000, 100000, 1 milhão e 10 milhões).
3.  Para cada teste, conta o número de vezes que "Cara" (representado por 0) e "Coroa" (representado por 1) são sorteados usando `rand() % 2`.
4.  Calcula e imprime a porcentagem de cada resultado para cada teste.
5.  Inclui suporte para Raspberry Pi Pico, utilizando `pico/stdlib.h` para inicialização de I/O serial e um pequeno atraso inicial.

## Resultados dos Testes

A seguir estão os resultados obtidos ao executar o programa com os diferentes números de sorteios:

--- Teste com 10 sorteios ---
Caras: 7 (70.00%)
Coroas: 3 (30.00%)

--- Teste com 100 sorteios ---
Caras: 60 (60.00%)
Coroas: 40 (40.00%)

--- Teste com 1000 sorteios ---
Caras: 515 (51.50%)
Coroas: 485 (48.50%)

--- Teste com 10000 sorteios ---
Caras: 4975 (49.75%)
Coroas: 5025 (50.25%)

--- Teste com 100000 sorteios ---
Caras: 49657 (49.66%)
Coroas: 50343 (50.34%)

--- Teste com 1000000 sorteios ---
Caras: 500325 (50.03%)
Coroas: 499675 (49.97%)

--- Teste com 10000000 sorteios ---
Caras: 4998719 (49.99%)
Coroas: 5001281 (50.01%)

## Observações

* Como esperado, à medida que o número de sorteios aumenta, as porcentagens de "Cara" e "Coroa" tendem a se aproximar de 50%, indicando que a função `rand()` (quando usada com o operador módulo `% 2`) fornece uma distribuição razoavelmente uniforme para este tipo de simulação, sem um viés aparente.
* O código inclui suporte para Raspberry Pi Pico, utilizando a biblioteca `pico/stdlib.h` para inicializar a comunicação serial e introduzir um pequeno atraso no início. A saída é direcionada para o monitor serial quando executado no Pico.
* Para compilar e executar em um ambiente padrão (não-Pico), você pode usar um compilador C como o GCC. Para o Raspberry Pi Pico, um ambiente de desenvolvimento com o SDK Pico configurado é necessário.

## Como Usar

**Para compilar e executar em um ambiente padrão (ex: Linux, macOS, Windows com GCC):**

1.  Salve o código como `sorteio_moeda.c`.
2.  Abra um terminal ou prompt de comando.
3.  Compile o código: `gcc sorteio_moeda.c -o sorteio_moeda`
4.  Execute o programa: `./sorteio_moeda`

**Para usar no Raspberry Pi Pico:**

1.  Certifique-se de ter o ambiente de desenvolvimento do Raspberry Pi Pico configurado.
2.  Compile o código usando o processo específico do SDK Pico (geralmente envolvendo `cmake` e `make`).
3.  Faça o upload do arquivo `.uf2` gerado para o seu Raspberry Pi Pico.
4.  Abra o monitor serial na sua IDE para visualizar a saída.

Este é um teste simples para ilustrar a aleatoriedade da função `rand()` para uma tarefa básica. Para aplicações que exigem aleatoriedade criptograficamente segura, métodos mais robustos devem ser considerados.
