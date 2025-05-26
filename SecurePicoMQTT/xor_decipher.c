#include "xor_decipher.h"

void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; i++) {
        output[i] = input[i] ^ key;
    }
}
