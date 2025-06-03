import time
import random
import subprocess

XOR_KEY = 42

# Tópicos solicitados, com repetição no último
topicos = [
    "escola/sala158/temperatura",
    "escola/sala203/temperatura",
    "escola/sala98/temperatura",
    "escola/sala112/temperatura",
    "escola/sala112/temperatura"  # mensagem repetida (replay)
]

ultima_msg = ""

for i in range(5):
    if i == 4 and ultima_msg:
        mensagem_clara = ultima_msg
        print(f"[{i+1}/5] Reenviando mensagem: {mensagem_clara}")
    else:
        temperatura = round(25.0 + random.randint(0, 50) / 10.0, 1)  # T=%.1f
        timestamp = int(time.monotonic() * 1000)
        mensagem_clara = f"T={temperatura:.1f} {timestamp}"
        ultima_msg = mensagem_clara
        print(f"[{i+1}/5] Nova mensagem clara: {mensagem_clara}")

    # XOR + conversão para hex string (como seu xor_encrypt_to_hex em C)
    criptografada_bytes = bytes([ord(c) ^ XOR_KEY for c in mensagem_clara])
    mensagem_hexstr = criptografada_bytes.hex()
    print(f"[{i+1}/5] Enviando criptografada (hex): {mensagem_hexstr}")

    comando = [
        "mosquitto_pub",
        "-h", "192.168.15.101",         # Altere se o broker tiver outro IP
        "-t", topicos[i],
        "-m", mensagem_hexstr,
        "-u", "aluno",               # Altere conforme suas credenciais
        "-P", "senha123"
    ]

    subprocess.run(comando)
    time.sleep(2)
