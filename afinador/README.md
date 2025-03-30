# Afinador Eletrônico na BitDogLab

[![GitHub Repository](https://img.shields.io/badge/GitHub-Repository-blue?logo=github)](https://github.com/CarlosMartinezPerez/afinador/tree/main)

Bem-vindo ao repositório do **Afinador Eletrônico** desenvolvido para a **BitDogLab**! Este projeto é um afinador eletrônico baseado no microcontrolador Raspberry Pi Pico, que utiliza os recursos da placa e o algoritmo Yin para detectar a frequência fundamental da onda sonora emitida pelo violão.

## 🎵 Sobre o Projeto

O **Afinador Eletrônico** foi projetado para demonstrar a aplicação dos conhecimentos obtidos na capacitação EMBARCATECH a atender ao projeto final da primeira fase do curso. Ele funciona detectando automaticamente a frequência emitida pelo instrumento musical e compara-a com as notas padrão (E2, A2, D3, G3, B3, E4). O sistema fornece feedback visual através do LED RGB e do display oled. Além disso, emite sinais sonoros para indicar o pressionamento dos botões de seleção de nota. Você encontrará dois arquivos com o programa fonte do projeto, em linguagem C. Um sem nehum comentário (afinador.c) e outro com comentários detalhados (afinadorcomentado.c) para sua informação sobre os detalhes do código. Ao seguir as instruções do CMakeLists.txt, o compilador estará trabalhando com o arquivo fonte sem comentários.

### Características Principais:
- **Detecção Automática de Frequências**: Utiliza o algoritmo **YIN** para identificar a frequência fundamental da nota tocada.
- **Interface Visual**: Um display OLED exibe informações sobre a nota atual e o estado da afinação (abaixo, acima ou afinado).
- **Feedback Sonoro e Visual**: 
  - LED **vermelho**: Indica que a nota está abaixo do alvo.
  - LED **verde**: Indica que a nota está afinada.
  - LED **azul**: Indica que a nota está acima do alvo.
  - **Beep sonoro**: Feedback adicional para confirmação de pressionamento de botões e das notas limites alcançadas.
  
### Componentes Utilizados:
- **Raspberry Pi Pico** (Microcontrolador RP2040)
- **Display OLED SSD1306** (I2C)
- **Microfone eletreto**
- **LED RGB** (Vermelho, Verde e Azul)
- **Botões físicos** para navegação entre as notas
- **Buzzer** para feedback sonoro

---

## 🛠️ Instalação e Configuração

### Pré-requisitos:
Antes de começar, certifique-se de ter os seguintes itens instalados:
- **Raspberry Pi Pico SDK**: Para compilar e carregar o código no microcontrolador.
- **Ferramentas de Desenvolvimento C/C++**: GCC, Make, etc.
- **Bibliotecas Necessárias**: As bibliotecas `pico-sdk`, `ssd1306` e `yin` estão incluídas neste repositório.

### Passos para Configuração:

1. **Clone o Repositório**:
   ```bash
   git clone https://github.com/CarlosMartinezPerez/afinador.git
   cd afinador
   ```

2. **Configurar o Ambiente de Desenvolvimento**:
   Certifique-se de configurar corretamente o ambiente de desenvolvimento para o Raspberry Pi Pico. Você pode encontrar mais detalhes no [guia oficial](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf).

3. **Compilar o Código**:
   Use o comando `make` para compilar o código-fonte:
   ```bash
   make
   ```

4. **Carregar o Código no Raspberry Pi Pico**:
   Conecte o Raspberry Pi Pico ao seu computador enquanto mantém o botão `BOOTSEL` pressionado. Em seguida, copie o arquivo `.uf2` gerado para o dispositivo.

---

## 🎸 Como Usar

1. **Ligue o Dispositivo**: Após conectar o afinador à fonte de energia, o display OLED mostrará a interface inicial.
   
2. **Selecione a Nota Alvo**:
   - Pressione o **Botão A** para diminuir a nota alvo.
   - Pressione o **Botão B** para aumentar a nota alvo.
   
3. **Toque a Nota no Instrumento**: O afinador detectará automaticamente a frequência do som captado pelo microfone e comparará com a nota selecionada.

4. **Observe o Feedback**:
   - **LEDs RGB**: Indicam se a nota está abaixo (vermelho), afinada (verde) ou acima (azul).
   - **Display OLED**: Mostra a nota alvo e o status da afinação (abaixo, acima ou afinado).

5. **Ajuste o Instrumento**: Continue ajustando até que o LED verde seja aceso e o display indique "AFINADO".

---

## 🔧 Personalização

Você pode personalizar o comportamento do afinador modificando os seguintes parâmetros no código:

- **Tolerância de Afinação**: Altere a constante `TOLERANCIA` no código para ajustar a precisão da detecção de afinação.
- **Notas Suportadas**: Modifique o array `frequencias[]` para incluir outras notas musicais conforme necessário.
- **Frequência de Amostragem e Quantidade de Amostras**: Ajuste as constantes `FREQUENCIA_AMOSTRAGEM` e `AMOSTRAS` para alterar a taxa de amostragem do sinal captado pelo microfone.
- **Threshold**: Llimiar do algoritmo Yin, que influencia a detecção da frequência fundamental e depende do microfone utilizado, do ruído no ambiente e do estado das cordas do instrumento.

---

## 📚 Estrutura do Código

O código é organizado em várias funções principais:

- **Inicialização de Hardware**:
  - `inicializar_adc_microfone()`: Inicializa o microfone para captura de áudio.
  - `inicializar_pwm_buzzer()`: Configura o buzzer para feedback sonoro.
  - `inicializar_oled_i2c()`: Inicializa o display OLED para exibir informações.
  - `inicializar_led_rgb()`: Configura os LEDs RGB para feedback visual.

- **Funções de Operação**:
  - `amostrar_microfone()`: Captura amostras de áudio do microfone.
  - `emitir_beep()`: Emite um beep sonoro como feedback.
  - `definirNotaAlvo()`: Permite ao usuário selecionar a nota alvo usando os botões.
  - `definir_estado_led_rgb()`: Controla o estado dos LEDs RGB com base no status da afinação.

---

### 🚀 Possibilidades Futuras de Desenvolvimento  

O **Afinador Eletrônico** apresenta diversas oportunidades para melhorias e expansões futuras. Uma possibilidade interessante seria a integração de conexão via cabo P10, permitindo a afinação de instrumentos elétricos como guitarra ou contrabaixo. Além disso, poderia ser implementado um modo de afinação personalizada, onde o usuário poderia definir suas próprias frequências-alvo para instrumentos não convencionais ou para treino vocal e de ouvido absoluto e relativo. Também seria viável expandir o suporte para outras escalas musicais, como pentatônicas ou microtonais, ampliando o uso do afinador para músicos experimentais. Por fim, a inclusão de filtros digitais para o sinal captado pelo microfone garantiria maior precisão em ambientes com diferentes níveis de ruído, tornando o dispositivo ainda mais robusto e versátil. Todas essas possibilidades, apesar de adicionarem melhorias importantes, estão além do escopo do projeto final do curso.

---

## 🤝 Contribuições

Contribuições são bem-vindas! Se você deseja melhorar este projeto ou encontrou algum problema, sinta-se à vontade para abrir uma **Issue** ou enviar um **Pull Request**.

---

## 📜 Licença

Este projeto está licenciado sob a **MIT License**. Consulte o arquivo [LICENSE](LICENSE) para obter mais informações.

---

## 🙏 Agradecimentos

Gostaria de agradecer à comunidade do Raspberry Pi Pico e aos criadores das bibliotecas utilizadas neste projeto. Especialmente:

- Toda a equipe da capacitação EMBARCATECH, em especial ao professor **Fabiano Fruet**.
- **SSD1306 Library**: Para controle do display OLED.
- **YIN Algorithm**: Para detecção de frequências fundamentais de sinais.

Se você gostou deste projeto, considere dar uma ⭐ no repositório!

---

> **Desenvolvido por Carlos Martinez Perez**  
> _Contato: [profcarlosmp@outlook.com.br](mailto:profcarlosmp@outlook.com.br)  
> _Repositório: [https://github.com/CarlosMartinezPerez/afinador](https://github.com/CarlosMartinezPerez/afinador)_