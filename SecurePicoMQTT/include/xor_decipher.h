#ifndef XOR_DECIPHER_H
#define XOR_DECIPHER_H

#include <stdint.h>
#include <stddef.h>

// Função para criptografar ou descriptografar um buffer usando XOR com uma chave
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);

// Alias para descriptografar (mesma operação)
static inline void xor_decrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key) {
    xor_encrypt(input, output, len, key);
}

#endif // XOR_DECIPHER_H
