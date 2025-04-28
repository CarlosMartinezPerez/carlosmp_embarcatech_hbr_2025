# LedBlinkModular

## Visão Geral
Este projeto é uma implementação modular para fazer o LED embutido do Raspberry Pi Pico W piscar. Escrito em C utilizando o Pico SDK, ele demonstra uma abordagem estruturada e manutenível para programação embarcada, separando o controle de hardware, a camada de abstração e a lógica da aplicação em camadas distintas. O projeto serve como um exemplo educacional para contrastar arquiteturas modular e monolítica.

## Objetivo
O objetivo principal é destacar os benefícios de uma arquitetura modular em sistemas embarcados, incluindo:
- **Separação de responsabilidades**: Divisão das funções em drivers, camada de abstração de hardware (HAL) e lógica da aplicação.
- **Reutilização**: Criação de componentes que podem ser reutilizados em outros projetos.
- **Manutenibilidade**: Simplificação de atualizações e depuração ao isolar mudanças em módulos específicos.
- **Escalabilidade**: Facilitação da adição de novas funcionalidades ou hardware sem alterar a lógica principal.

Este projeto complementa sua contraparte monolítica, [LedBlinkMonolithic](https://github.com/your-username/LedBlinkMonolithic), para ilustrar as diferenças entre as duas abordagens.

## Arquitetura Modular
O projeto adota uma arquitetura modular, organizando o código em três camadas principais:
1. **Drivers**: Código de baixo nível que interage diretamente com o hardware (por exemplo, o chip Wi-Fi CYW4343 que controla o LED embutido).
2. **HAL (Camada de Abstração de Hardware)**: Uma camada de abstração que fornece funções simples e reutilizáveis para interagir com o hardware, escondendo detalhes de baixo nível.
3. **Aplicação**: Lógica de alto nível que orquestra o comportamento do programa (por exemplo, piscar o LED).

Essa estrutura melhora a manutenibilidade ao isolar o código específico do hardware, permitindo alterações no hardware sem afetar a lógica da aplicação.

## Estrutura do Projeto
O projeto está organizado da seguinte forma:  
  
LedBlinkModular/  
├── app/  
│   └── main.c  
├── drivers/  
│   └── led_embutido.c  
├── hal/  
│   └── hal_led.c  
├── include/  
│   ├── led_embutido.h  
│   └── hal_led.h  
├── CMakeLists.txt  
├── README.md  
  
### Descrição dos Arquivos
Abaixo está uma explicação detalhada de cada arquivo de código-fonte e sua função na arquitetura modular:

#### `app/main.c`
- **Propósito**: Contém a lógica da aplicação de alto nível.
- **Funcionalidade**: Inicializa a entrada/saída padrão, configura o LED usando o HAL e implementa um loop para alternar o estado do LED a cada 500 ms.
- **Aspectos Principais**:
  - Utiliza apenas a interface do HAL (`hal_led_init` e `hal_led_toggle`), garantindo nenhuma interação direta com o hardware.
  - Trata erros de inicialização entrando em um loop infinito (extensível para indicação de erros).
  - Demonstra uma separação clara do código específico do hardware.

#### `drivers/led_embutido.c`
- **Propósito**: Implementa o controle de baixo nível do LED embutido.
- **Funcionalidade**:
  - Inicializa o chip Wi-Fi CYW4343 (necessário para controlar o LED) usando `cyw43_arch_init`.
  - Fornece uma função para definir o estado do LED (ligado ou desligado) usando `cyw43_arch_gpio_put`.
- **Aspectos Principais**:
  - Interage diretamente com a API `cyw43_arch` do Pico SDK.
  - Encapsula detalhes específicos do hardware, sendo o único módulo que interage com o chip Wi-Fi.

#### `hal/hal_led.c`
- **Propósito**: Fornece uma camada de abstração de hardware para controle do LED.
- **Funcionalidade**:
  - Oferece `hal_led_init` para inicializar o LED por meio do driver.
  - Implementa `hal_led_toggle` para alternar o estado do LED, mantendo uma variável interna de estado.
- **Aspectos Principais**:
  - Abstrai detalhes do driver, expondo uma interface simples para a aplicação.
  - Melhora a portabilidade, permitindo que a aplicação funcione com diferentes implementações de LED (por exemplo, LEDs externos) modificando apenas o driver.

#### `include/led_embutido.h`
- **Propósito**: Arquivo de cabeçalho para o driver do LED.
- **Funcionalidade**:
  - Declara as funções `led_embutido_init` e `led_embutido_set`.
  - Inclui tipos necessários (`stdbool.h`) para operações booleanas.
- **Aspectos Principais**:
  - Usa guardas de inclusão para evitar inclusões múltiplas.
  - Fornece uma interface limpa para o driver, utilizada pelo HAL.

#### `include/hal_led.h`
- **Propósito**: Arquivo de cabeçalho para o HAL.
- **Funcionalidade**:
  - Declara as funções `hal_led_init` e `hal_led_toggle`.
  - Inclui `stdbool.h` para tipos de retorno booleanos.
- **Aspectos Principais**:
  - Define a interface pública para controle do LED, utilizada pela aplicação.
  - Garante que a aplicação permaneça independente de detalhes do hardware.

#### `CMakeLists.txt`
- **Propósito**: Configura o processo de compilação.
- **Funcionalidade**:
  - Especifica os arquivos-fonte (`main.c`, `led_embutido.c`, `hal_led.c`).
  - Inclui o diretório `include/` para arquivos de cabeçalho.
  - Vincula bibliotecas do Pico SDK (`pico_stdlib`, `pico_cyw43_arch_none`).
  - Habilita saída USB e gera arquivos UF2 para programação.
- **Aspectos Principais**:
  - Garante a compilação correta para o Raspberry Pi Pico W.
  - Simplifica a configuração do projeto para outros desenvolvedores.

## Benefícios da Modularização
A arquitetura modular oferece várias vantagens em comparação com uma abordagem monolítica:
- **Isolamento**: Alterações no hardware (por exemplo, mudar para um LED externo) afetam apenas o driver, não o HAL ou a aplicação.
- **Reutilização**: O HAL e o driver podem ser reutilizados em outros projetos com mudanças mínimas.
- **Testabilidade**: Módulos individuais podem ser testados independentemente.
- **Clareza**: A separação de responsabilidades torna o código mais fácil de entender e manter.

___________________________________________________________________________________________________

# LedBlinkModular

## Overview
This project is a modular implementation designed to blink the onboard LED of the Raspberry Pi Pico W. Written in C using the Pico SDK, it demonstrates a structured, maintainable approach to embedded programming by separating hardware control, abstraction, and application logic into distinct layers. The project serves as an educational example to contrast modular and monolithic architectures.

## Purpose
The primary goal is to showcase the benefits of a modular architecture in embedded systems, including:
- **Separation of concerns**: Dividing responsibilities into drivers, hardware abstraction layer (HAL), and application logic.
- **Reusability**: Creating components that can be reused in other projects.
- **Maintainability**: Simplifying updates and debugging by isolating changes to specific modules.
- **Scalability**: Facilitating the addition of new features or hardware without altering the core logic.

This project complements its monolithic counterpart, [LedBlinkMonolithic](https://github.com/your-username/LedBlinkMonolithic), to illustrate the differences between the two approaches.

## Modular Architecture
The project adopts a modular architecture, organizing code into three main layers:
1. **Drivers**: Low-level code that directly interfaces with hardware (e.g., the CYW4343 Wi-Fi chip controlling the onboard LED).
2. **HAL (Hardware Abstraction Layer)**: An abstraction layer providing simple, reusable functions to interact with hardware, hiding low-level details.
3. **Application**: High-level logic that orchestrates the program's behavior (e.g., blinking the LED).

This structure enhances maintainability by isolating hardware-specific code, allowing changes to hardware without affecting the application logic.

## Project Structure
The project is organized as follows:  
  
LedBlinkModular/  
├── app/  
│   └── main.c  
├── drivers/  
│   └── led_embutido.c  
├── hal/  
│   └── hal_led.c  
├── include/  
│   ├── led_embutido.h  
│   └── hal_led.h  
├── CMakeLists.txt  
├── README.md  
  
### File Descriptions
Below is a detailed explanation of each source file and its role in the modular architecture:

#### `app/main.c`
- **Purpose**: Contains the high-level application logic.
- **Functionality**: Initializes the standard I/O, sets up the LED using the HAL, and implements a loop to toggle the LED every 500 ms.
- **Key Aspects**:
  - Uses only the HAL interface (`hal_led_init` and `hal_led_toggle`), ensuring no direct interaction with hardware.
  - Handles initialization errors by entering an infinite loop (extendable for error indication).
  - Demonstrates a clean separation from hardware-specific code.

#### `drivers/led_embutido.c`
- **Purpose**: Implements low-level control of the onboard LED.
- **Functionality**:
  - Initializes the CYW4343 Wi-Fi chip (required for LED control) using `cyw43_arch_init`.
  - Provides a function to set the LED state (on or off) using `cyw43_arch_gpio_put`.
- **Key Aspects**:
  - Directly interfaces with the Pico SDK's `cyw43_arch` API.
  - Encapsulates hardware-specific details, making it the only module that interacts with the Wi-Fi chip.

#### `hal/hal_led.c`
- **Purpose**: Provides a hardware abstraction layer for LED control.
- **Functionality**:
  - Offers `hal_led_init` to initialize the LED via the driver.
  - Implements `hal_led_toggle` to alternate the LED state, maintaining an internal state variable.
- **Key Aspects**:
  - Abstracts driver details, exposing a simple interface to the application.
  - Enhances portability by allowing the application to work with different LED implementations (e.g., external LEDs) by modifying only the driver.

#### `include/led_embutido.h`
- **Purpose**: Header file for the LED driver.
- **Functionality**:
  - Declares `led_embutido_init` and `led_embutido_set` functions.
  - Includes necessary types (`stdbool.h`) for boolean operations.
- **Key Aspects**:
  - Uses include guards to prevent multiple inclusions.
  - Provides a clean interface for the driver, used by the HAL.

#### `include/hal_led.h`
- **Purpose**: Header file for the HAL.
- **Functionality**:
  - Declares `hal_led_init` and `hal_led_toggle` functions.
  - Includes `stdbool.h` for boolean return types.
- **Key Aspects**:
  - Defines the public interface for LED control, used by the application.
  - Ensures the application remains independent of hardware specifics.

#### `CMakeLists.txt`
- **Purpose**: Configures the build process.
- **Functionality**:
  - Specifies source files (`main.c`, `led_embutido.c`, `hal_led.c`).
  - Includes the `include/` directory for header files.
  - Links Pico SDK libraries (`pico_stdlib`, `pico_cyw43_arch_none`).
  - Enables USB output and generates UF2 files for flashing.
- **Key Aspects**:
  - Ensures proper compilation for the Raspberry Pi Pico W.
  - Simplifies project setup for other developers.

## Benefits of Modularization
The modular architecture offers several advantages over a monolithic approach:
- **Isolation**: Changes to hardware (e.g., switching to an external LED) only affect the driver, not the HAL or application.
- **Reusability**: The HAL and driver can be reused in other projects with minimal changes.
- **Testability**: Individual modules can be tested independently.
- **Clarity**: The separation of concerns makes the codebase easier to understand and maintain.
