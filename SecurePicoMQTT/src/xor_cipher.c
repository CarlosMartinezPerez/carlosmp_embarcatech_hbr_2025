#include "include/xor_cipher.h"

void xor_apply(const uint8_t *input, uint8_t *output, size_t length, uint8_t key) {
    for (size_t i = 0; i < length; ++i) {
        output[i] = input[i] ^ key;
    }
    // Opcional: adicionar caractere nulo se tratando como string
    output[length] = '\0';
}
