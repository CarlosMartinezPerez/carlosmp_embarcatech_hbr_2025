#ifndef XOR_DECIPHER_H
#define XOR_DECIPHER_H

#include <stddef.h>
#include <stdint.h>

void xor_decrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);

#endif
