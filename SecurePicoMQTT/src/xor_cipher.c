#include <stdlib.h>  // <-- Necessário para strtoul
#include "include/xor_cipher.h"
#include <stdio.h>  // para sprintf
#include <string.h> // para strlen
#include <ctype.h>  // para validação opcional

// XOR simples
void xor_apply(const uint8_t *input, uint8_t *output, size_t length, uint8_t key) {
    for (size_t i = 0; i < length; ++i) {
        output[i] = input[i] ^ key;
    }
    output[length] = '\0'; // útil se tratando como string
}

// Criptografa e converte em hexadecimal
void xor_encrypt_to_hex(const uint8_t *input, char *output_hex, size_t length, uint8_t key) {
    for (size_t i = 0; i < length; ++i) {
        uint8_t encrypted = input[i] ^ key;
        sprintf(&output_hex[i * 2], "%02X", encrypted);
    }
    output_hex[length * 2] = '\0'; // fim da string
}

// Decodifica hexadecimal e aplica XOR
void xor_decrypt_from_hex(const char *input_hex, uint8_t *output, uint8_t key) {
    size_t len = strlen(input_hex);
    if (len % 2 != 0) {
        printf("Erro: input_hex tem tamanho ímpar (%zu)\n", len);
        output[0] = '\0';  // garante string vazia
        return;
    }

    for (size_t i = 0; i < len; i += 2) {
        char byte_str[3] = { input_hex[i], input_hex[i + 1], '\0' };
        uint8_t value = (uint8_t) strtoul(byte_str, NULL, 16);
        output[i / 2] = value ^ key;
    }
    output[len / 2] = '\0';
}
