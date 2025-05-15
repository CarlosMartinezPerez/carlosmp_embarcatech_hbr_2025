# Projeto OTA para Raspberry Pi Pico W

## Visão Geral

Este projeto implementa uma funcionalidade **OTA (Over-The-Air)** na **Raspberry Pi Pico W**, permitindo que a placa receba e execute códigos Python enviados via Wi-Fi. O sistema utiliza o módulo Wi-Fi integrado (CYW43439) para conectar-se a uma rede Wi-Fi, iniciar um servidor TCP, e receber arquivos de código de um cliente remoto. O projeto inclui indicadores visuais com LEDs para sinalizar o status da conexão Wi-Fi e a execução do código recebido.

### O que o projeto faz?
- **Conexão Wi-Fi**: A Pico W se conecta à rede Wi-Fi especificada (`Nome-da-rede`).
- **Sinalização de conexão**: Um LED vermelho (GPIO 13) pisca longamente (2 segundos) ao estabelecer a conexão Wi-Fi e pisca brevemente (0,5s ON, 0,5s OFF) a cada 10 segundos para confirmar que o Wi-Fi está ativo.
- **Servidor TCP**: Inicia um servidor na porta 80 para receber arquivos Python de um cliente remoto.
- **Execução OTA**: Salva o código recebido como `received.py` no sistema de arquivos da Pico W e o executa usando `exec()`.
- **Sinalização de execução**: Um LED azul (GPIO 12) pisca cinco vezes (cinco ciclos ON → OFF) quando o código de teste é executado, indicando sucesso na recepção e execução.
- **Operação sem USB**: Após configuração inicial, a placa pode ser desconectada do USB e alimentada por bateria, mantendo a funcionalidade OTA via Wi-Fi.

## Como funciona?

### Componentes de hardware
- **Raspberry Pi Pico W**: Microcontrolador com módulo Wi-Fi CYW43439.
- **LED vermelho**: Conectado ao GPIO 13 (anodo ao GPIO 13, cátodo ao GND, com resistor de ~220Ω).
- **LED azul**: Conectado ao GPIO 12 (anodo ao GPIO 12, cátodo ao GND, com resistor de ~220Ω).
- **Bateria**: Fonte de alimentação (ex.: 3.3V ou 5V via VSYS) para operação sem USB.
- **Cabo USB**: Para configuração inicial e depuração.
- **Computador**: Para carregar o firmware, enviar o código OTA, e monitorar via serial.

### Componentes de software
- **MicroPython**: Firmware para Raspberry Pi Pico W (versão recomendada: `rp2-pico-w-20250426-v1.23.0.uf2`).
- **ota_pico_w.py**: Script MicroPython que gerencia a conexão Wi-Fi, servidor TCP, e execução OTA.
- **send_code.py**: Script Python (executado no computador) que envia o código de teste via Wi-Fi.
- **VS Code**: Ferramentas para carregar o código e monitorar a saída serial.

### Fluxo de funcionamento
1. **Inicialização**:
   - A Pico W é inicializada com o firmware MicroPython.
   - O script `ota_pico_w.py` é carregado e executado.
2. **Conexão Wi-Fi**:
   - Conecta à rede Wifi.
   - Pisca o LED vermelho por 2 segundos ao conectar.
   - Verifica o status do Wi-Fi a cada 10 segundos, piscando o LED vermelho brevemente se ativo.
3. **Servidor TCP**:
   - Inicia um servidor na porta 80, aguardando conexões de clientes.
   - Recebe arquivos Python enviados via TCP, salvando-os como `received.py`.
4. **Execução OTA**:
   - Executa o código recebido com `exec()`, que, no caso do código de teste, pisca o LED azul (GPIO 12) cinco vezes (cinco ciclos ON → OFF de 0,5s cada).
5. **Operação com bateria**:
   - Após configuração, a placa pode ser desconectada do USB e alimentada por bateria, mantendo o Wi-Fi e o servidor ativos.

## Como fazer funcionar?

Siga estas instruções detalhadas para configurar e executar o projeto. A sequência fornecida foi validada e funciona conforme descrito.

### Pré-requisitos
- **Hardware**:
  - Raspberry Pi Pico W.
  - LEDs vermelho (GPIO 13) e azul (GPIO 12) com resistores.
  - Bateria compatível (ex.: 3.3V ou 5V via VSYS).
  - Cabo USB para configuração.
- **Software**:
  - Firmware MicroPython para Pico W ([download](https://micropython.org/download/rp2-pico-w/)).
  - VS Code com extensão MicroPython (ex.: Pico-W-Go) ou Thonny.
  - Python instalado no computador (para executar `send_code.py`).
- **Rede Wi-Fi**:
  - Acesso à rede Wifi.

### Instruções de configuração e execução

1. **Conectar a placa via USB e selecionar BOOTSEL**:
   - Conecte a Pico W ao computador via cabo USB enquanto segura o botão **BOOTSEL**.
   - A placa aparecerá como uma unidade de armazenamento chamada **RPI-RP2**.
   - Solte o botão BOOTSEL após a conexão.

2. **Carregar o MicroPython na placa**:
   - Baixe o firmware MicroPython para Pico W (ex.: `rp2-pico-w-20250426-v1.23.0.uf2`) de [micropython.org](https://micropython.org/download/rp2-pico-w/).
   - Copie o arquivo `.uf2` para a unidade **RPI-RP2**.
   - A placa reiniciará automaticamente, e a unidade desaparecerá, indicando que o firmware foi carregado.

3. **Rodar o `ota_pico_w.py` na placa**:
   - **Código**: Copie o script `ota_pico_w.py` fornecido abaixo para um arquivo local:
     ```python
     import network
     import socket
     import time
     import uio
     from machine import Pin

     # Configurações Wi-Fi
     SSID = "Seu Login wifi"
     PASSWORD = "sua senha wifi"

     # Configura LED vermelho no GPIO 13
     led_red = Pin(13, Pin.OUT)
     led_red.off()

     def connect_wifi(ssid, password):
         wlan = network.WLAN(network.STA_IF)
         wlan.active(True)
         print("Conectando à rede Wi-Fi:", ssid)
         wlan.connect(ssid, password)
         timeout = 10
         start = time.time()
         while not wlan.isconnected() and time.time() - start < timeout:
             print(".", end="")
             time.sleep(1)
         if wlan.isconnected():
             print("\nConectado! IP:", wlan.ifconfig()[0])
             led_red.on()
             print("LED vermelho ON (piscada longa)")
             time.sleep(2)
             led_red.off()
             print("LED vermelho OFF")
             return wlan, True
         else:
             print("\nFalha ao conectar")
             return wlan, False

     def start_server():
         addr = socket.getaddrinfo("0.0.0.0", 80)[0][-1]
         s = socket.socket()
         s.bind(addr)
         s.listen(1)
         print("Servidor TCP iniciado na porta 80")
         return s

     def receive_file(sock):
         print("Aguardando conexão do cliente...")
         conn, addr = sock.accept()
         print("Cliente conectado:", addr)
         file_content = ""
         while True:
             data = conn.recv(1024)
             if not data:
                 break
             file_content += data.decode()
         conn.close()
         print("Arquivo recebido (primeiros 100 caracteres):", file_content[:100])
         print("Tamanho do arquivo:", len(file_content), "bytes")
         try:
             with uio.open("received.py", "w") as f:
                 f.write(file_content)
             print("Arquivo salvo como received.py")
             return "received.py"
         except Exception as e:
             print("Erro ao salvar arquivo:", e)
             return None

     def execute_file(filename):
         try:
             with uio.open(filename, "r") as f:
                 code = f.read()
             print("Conteúdo do arquivo a executar:", code)
             exec(code)
             print("Código executado com sucesso")
         except SyntaxError as e:
             print("Erro de sintaxe no arquivo:", e)
         except Exception as e:
             print("Erro ao executar arquivo:", e)

     try:
         print("Iniciando OTA...")
         wlan, connected = connect_wifi(SSID, PASSWORD)
         if connected:
             server_socket = start_server()
             last_wifi_check = time.time()
             while True:
                 if time.time() - last_wifi_check >= 10:
                     if wlan.isconnected():
                         print("Wi-Fi ativo")
                         led_red.on()
                         time.sleep(0.5)
                         led_red.off()
                         time.sleep(0.5)
                     else:
                         print("Wi-Fi desconectado")
                     last_wifi_check = time.time()
                 server_socket.settimeout(1.0)
                 try:
                     print("Aguardando arquivo Python...")
                     filename = receive_file(server_socket)
                     if filename:
                         print("Executando arquivo...")
                         execute_file(filename)
                     else:
                         print("Nenhum arquivo recebido")
                 except OSError as e:
                     if e.args[0] == 110:
                         continue
                     else:
                         raise
     except KeyboardInterrupt:
         print("Programa interrompido pelo usuário")
     except Exception as e:
         print("Erro geral:", e)
     finally:
         if 'server_socket' in locals():
             server_socket.close()
         if 'wlan' in locals() and wlan.active():
             wlan.disconnect()
             wlan.active(False)
     ```
   - **Carregamento**:
     - Abra o VS Code com a extensão MicroPython (ex.: Pico-W-Go) ou Thonny.
     - Conecte a Pico W via USB (sem BOOTSEL).
     - Salve o código como `ota_pico_w.py` e carregue na placa usando a extensão ou Thonny (ex.: clique em "Upload" ou "Run").
     - Reinicie a placa (desconecte e reconecte o USB ou use o comando de reset).
   - **Verificação**:
     - Abra o monitor serial (baudrate 115200).
     - Você verá:
       ```
       Iniciando OTA...
       Conectando à rede Wi-Fi: Nome-da-rede
       .......
       Conectado! IP: 192.168.15.101
       LED vermelho ON (piscada longa)
       [2 segundos depois]
       LED vermelho OFF
       Servidor TCP iniciado na porta 80
       Aguardando arquivo Python...
       ```
     - O **LED vermelho** (GPIO 13) piscará por 2 segundos ao conectar.
     - A cada 10 segundos, o LED vermelho piscará brevemente (0,5s ON, 0,5s OFF) se o Wi-Fi estiver ativo, com a mensagem `"Wi-Fi ativo"` no monitor serial.

4. **Desligar o cabo USB, mantendo a placa alimentada por bateria**:
   - **Preparação**:
     - Conecte uma bateria compatível à Pico W (ex.: 3.3V diretamente nos pinos 3V3 e GND, ou 5V via VSYS com circuito regulador).
     - Certifique-se de que a bateria fornece corrente suficiente (~300mA) para o RP2040 e o módulo Wi-Fi (CYW43439).
   - **Desconexão**:
     - Após confirmar que o Wi-Fi está conectado (LED vermelho pisca por 2 segundos e o IP é exibido), desconecte o cabo USB.
     - A placa continuará funcionando com a bateria, mantendo o Wi-Fi e o servidor TCP ativos.
     - O LED vermelho piscará brevemente a cada 10 segundos, indicando que o Wi-Fi está ativo.
   - **Nota**: Sem USB, o monitor serial não estará disponível. Confie nas piscadas do LED vermelho para confirmar o status do Wi-Fi.

5. **No terminal CMD (Windows), executar `send_code.py`**:
   - **Código do cliente**:
     - Crie um arquivo `send_code.py` no seu computador com o seguinte conteúdo:
       ```python
       import socket
       import time

       HOST = "192.168.15.101"  # IP da Pico W
       PORT = 80

       code = """
       from machine import Pin
       import time
       led = Pin(12, Pin.OUT)
       led.off()
       for _ in range(5):
           led.on()
           print("LED ON")
           time.sleep(0.5)
           led.off()
           print("LED OFF")
           time.sleep(0.5)
       print("Fim do teste OTA")
       """

       with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
           s.connect((HOST, PORT))
           s.sendall(code.encode())
           time.sleep(1)
       print("Arquivo enviado")
       ```
   - **Execução**:
     - Abra o terminal CMD no Windows.
     - Navegue até o diretório onde `send_code.py` está salvo (ex.: `cd C:\caminho\para\seu\projeto`).
     - Execute: `python send_code.py`.
     - O script conectará ao servidor TCP da Pico W (IP `192.168.15.101`, porta 80) e enviará o código Python.
   - **Nota**:
     - Certifique-se de que o Python está instalado (ex.: Python 3.8+).
     - Se o IP da Pico W mudar (ex.: devido a reconexão), ajuste o `HOST` no `send_code.py` para o novo IP (visível no monitor serial antes de desconectar o USB).

6. **O LED azul pisca 5 vezes**:
   - Após executar `send_code.py`, a Pico W receberá o código, salvará como `received.py`, e o executará.
   - O código de teste faz o **LED azul** (GPIO 12) piscar cinco vezes (cinco ciclos ON por 0,5s, OFF por 0,5s), terminando desligado.
   - **Comportamento esperado**:
     - O LED azul acenderá e apagará cinco vezes, totalizando ~5 segundos.
     - Se o monitor serial estiver ativo (com USB conectado), você verá:
       ```
       Cliente conectado: ('192.168.15.100', [porta])
       Arquivo recebido (primeiros 100 caracteres): [início do código]
       Tamanho do arquivo: [ex.: 142] bytes
       Arquivo salvo como received.py
       Executando arquivo...
       Conteúdo do arquivo a executar: [código completo]
       LED ON
       LED OFF
       LED ON
       LED OFF
       LED ON
       LED OFF
       LED ON
       LED OFF
       LED ON
       LED OFF
       Fim do teste OTA
       Código executado com sucesso
       ```

### Depuração e solução de problemas
- **Wi-Fi não conecta**:
  - Verifique as credenciais da rede Wifi.
  - Confirme a intensidade do sinal Wi-Fi.
  - Reinstale o firmware MicroPython.
- **LED vermelho não pisca**:
  - Verifique a conexão do LED ao GPIO 13 (polaridade, resistor).
  - Teste o GPIO 13 com um código simples: `from machine import Pin; Pin(13, Pin.OUT).on()`.
- **LED azul não pisca cinco vezes**:
  - Confirme que o GPIO 12 tem um LED conectado.
  - Verifique o log do monitor serial (se disponível) para erros de sintaxe ou execução.
- **Caracteres estranhos no monitor serial** (ex.: `Conectando �� rede`):
  - Ajuste a codificação do terminal no VS Code para UTF-8.
  - Use Thonny, que lida melhor com codificação.
- **Erro ao executar `send_code.py`**:
  - Verifique se o Python está instalado (`python --version`).
  - Confirme o IP correto no `HOST` (ex.: `192.168.15.101`).
  - Certifique-se de que a Pico W está na mesma rede Wi-Fi que o computador.
- **Bateria não mantém Wi-Fi ativo**:
  - Use uma bateria com corrente suficiente (~300mA).
  - Teste a tensão (3.3V ou 5V via VSYS) com um multímetro.

### Segurança
- O código não inclui autenticação para arquivos recebidos, o que é arriscado em produção. Para uso real, adicione validação de código ou senhas no protocolo TCP.
- Evite expor o servidor TCP (porta 80) a redes públicas sem firewall ou autenticação.

### Possíveis melhorias
- **Reconexão automática**: Adicionar lógica para reconectar ao Wi-Fi se a conexão cair.
- **Validação de código**: Verificar o conteúdo do arquivo recebido antes de executar.
- **Servidor persistente com feedback**: Enviar confirmação ao cliente após executar o código.
- **Outros LEDs ou sensores**: Expandir para controlar mais periféricos via OTA.
