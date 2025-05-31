#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H

#include <stddef.h>
#include <stdint.h>

// Cifra/des-cifra uma mensagem com XOR usando a chave fornecida.
// Pode ser usada tanto para criptografar quanto para descriptografar.
void xor_apply(const uint8_t *input, uint8_t *output, size_t length, uint8_t key);

#endif
