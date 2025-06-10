#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H

#include <stddef.h>
#include <stdint.h>

// Cifra/des-cifra uma mensagem com XOR usando a chave fornecida.
void xor_apply(const uint8_t *input, uint8_t *output, size_t length, uint8_t key);

// Criptografa dados com XOR e converte para string hexadecimal
void xor_encrypt_to_hex(const uint8_t *input, char *output_hex, size_t length, uint8_t key);

// Decodifica string hexadecimal e decifra com XOR
void xor_decrypt_from_hex(const char *input_hex, uint8_t *output, uint8_t key);

#endif
