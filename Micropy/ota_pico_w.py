import network
import socket
import time
import uio
from machine import Pin

# Configurações Wi-Fi
SSID = "VIVOFIBRA-8991_EXT"
PASSWORD = "cajuca1801"

# Configura LED vermelho no GPIO 13
led_red = Pin(13, Pin.OUT)
led_red.off()  # Garante estado inicial desligado

def connect_wifi(ssid, password):
    """Conecta à rede Wi-Fi e pisca LED vermelho ao conectar"""
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
        # Piscada longa no LED vermelho (2 segundos ON, depois OFF)
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
    """Inicia servidor TCP na porta 80"""
    addr = socket.getaddrinfo("0.0.0.0", 80)[0][-1]
    s = socket.socket()
    s.bind(addr)
    s.listen(1)
    print("Servidor TCP iniciado na porta 80")
    return s

def receive_file(sock):
    """Recebe arquivo Python via TCP e salva"""
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
    """Executa o arquivo Python recebido"""
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

# Programa principal
try:
    print("Iniciando OTA...")
    
    # Conecta à rede Wi-Fi
    wlan, connected = connect_wifi(SSID, PASSWORD)
    
    if connected:
        server_socket = start_server()
        last_wifi_check = time.time()
        
        while True:
            # Verifica status do Wi-Fi a cada 10 segundos
            if time.time() - last_wifi_check >= 10:
                if wlan.isconnected():
                    print("Wi-Fi ativo")
                    led_red.on()
                    time.sleep(0.5)  # Piscada curta: 0,5s ON
                    led_red.off()
                    time.sleep(0.5)  # 0,5s OFF
                else:
                    print("Wi-Fi desconectado")
                last_wifi_check = time.time()
            
            # Configura timeout para não bloquear no accept()
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
                if e.args[0] == 110:  # ETIMEDOUT
                    continue  # Timeout normal, continua o loop
                else:
                    raise  # Outros erros são propagados
    
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

# end
