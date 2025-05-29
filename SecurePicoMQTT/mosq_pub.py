import time
import random
import subprocess

XOR_KEY = 42

topicos = [
    "escola/sala12/temperatura",
    "escola/sala7/temperatura",
    "escola/sala10/temperatura",
    "escola/sala3/temperatura",
    "escola/sala3/temperatura"
]

ultima_msg = ""

for i in range(5):
    if i >= 4 and ultima_msg:
        mensagem_clara = ultima_msg
        print(f"[{i+1}/5] Reenviando mensagem: {mensagem_clara}")
    else:
        temperatura = round(25.0 + random.randint(0, 50) / 10.0, 2)
        timestamp = int(time.monotonic() * 1000)
        mensagem_clara = f"T={temperatura:.1f} {timestamp}"
        ultima_msg = mensagem_clara
        print(f"[{i+1}/5] Nova mensagem clara: {mensagem_clara}")

    criptografada_bytes = bytes([ord(c) ^ XOR_KEY for c in mensagem_clara])
    mensagem_hex = criptografada_bytes.hex()
    print(f"[{i+1}/5] Enviando criptografada (hex): {mensagem_hex}")

    comando = [
        "mosquitto_pub",
        "-h", "192.168.15.104",
        "-t", topicos[i],
        "-m", mensagem_hex,
        "-u", "CarlosMP",
        "-P", "cajuca1801"
    ]
    print((mensagem_clara))
    print(comando)

    subprocess.run(comando)
    time.sleep(2)
