import socket

# IP da Pico W (obtido no monitor serial)
HOST = "192.168.15.105"  # Ajuste conforme o IP mostrado
PORT = 80

# Código Python a ser enviado (exemplo: pisca LED no GPIO 12)
code = """
from machine import Pin
import time

led = Pin(12, Pin.OUT)
led.off()  # Garante estado inicial desligado
for _ in range(5):  # Cinco ciclos completos ON → OFF
    led.on()
    print("LED ON")
    time.sleep(0.5)
    led.off()
    print("LED OFF")
    time.sleep(0.5)
print("Fim do teste")
"""

# Conecta ao servidor
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(code.encode())