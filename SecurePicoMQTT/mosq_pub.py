# Script para testar um subscriber MQTT enviando 5 mensagens criptografadas com XOR para diferentes tópicos
import time  # Importa a biblioteca time para gerar timestamps
import random  # Importa a biblioteca random para gerar temperaturas aleatórias
import subprocess  # Importa a biblioteca subprocess para executar o comando mosquitto_pub

# Define a chave XOR (42) usada para criptografar as mensagens
XOR_KEY = 42

# Lista de tópicos MQTT para publicação, com o último tópico repetido para simular reenvio
topicos = [
    "escola/sala12/temperatura",
    "escola/sala7/temperatura",
    "escola/sala10/temperatura",
    "escola/sala3/temperatura",
    "escola/sala3/temperatura"
]

# Variável para armazenar a última mensagem clara, usada no reenvio
ultima_msg = ""

# Loop para gerar e publicar 5 mensagens MQTT
for i in range(5):
    # Na última iteração (i >= 4), reutiliza a última mensagem para simular um ataque de replay
    if i >= 4 and ultima_msg:
        mensagem_clara = ultima_msg
        # Exibe que a mensagem está sendo reenviada
        print(f"[{i+1}/5] Reenviando mensagem: {mensagem_clara}")
    else:
        # Gera uma temperatura aleatória entre 25.0 e 30.0 com incrementos de 0.1
        temperatura = round(25.0 + random.randint(0, 50) / 10.0, 2)
        # Gera um timestamp em milissegundos usando time.monotonic()
        timestamp = int(time.monotonic() * 1000)
        # Formata a mensagem no formato "T=temperatura timestamp"
        mensagem_clara = f"T={temperatura:.1f} {timestamp}"
        # Armazena a mensagem para possível reenvio
        ultima_msg = mensagem_clara
        # Exibe a mensagem clara gerada
        print(f"[{i+1}/5] Nova mensagem clara: {mensagem_clara}")

    # Aplica criptografia XOR em cada caractere da mensagem clara, gerando bytes
    criptografada_bytes = bytes([ord(c) ^ XOR_KEY for c in mensagem_clara])
    # Converte a mensagem criptografada para formato hexadecimal
    mensagem_hex = criptografada_bytes.hex()
    # Exibe a mensagem criptografada em formato hexadecimal
    print(f"[{i+1}/5] Enviando criptografada (hex): {mensagem_hex}")

    # Monta o comando para publicar a mensagem no broker MQTT usando mosquitto_pub
    comando = [
        "mosquitto_pub",  # Comando para publicar mensagens MQTT
        "-h", "192.168.15.104",  # Endereço IP do broker MQTT
        "-t", topicos[i],  # Tópico MQTT para a mensagem
        "-m", mensagem_hex,  # Mensagem criptografada em formato hexadecimal
        "-u", "CarlosMP",  # Usuário para autenticação no broker
        "-P", "cajuca1801"  # Senha para autenticação no broker
    ]
    # Exibe a mensagem clara para referência
    print((mensagem_clara))
    # Exibe o comando completo para depuração
    print(comando)

    # Executa o comando mosquitto_pub para enviar a mensagem ao broker
    subprocess.run(comando)
    # Aguarda 2 segundos antes de enviar a próxima mensagem
    time.sleep(2)