#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Aplica uma cifra XOR simples para criptografar ou descriptografar dados.
 *
 * Esta função realiza uma operação XOR byte a byte entre os dados de entrada
 * e uma chave de 1 byte. Por ser uma operação reversível, a mesma função pode
 * ser usada tanto para cifrar quanto para decifrar os dados.
 *
 * @param input   Ponteiro para os dados de entrada (texto claro ou cifrado).
 * @param output  Ponteiro para o buffer de saída (deve ter pelo menos 'len' bytes).
 * @param len     Tamanho dos dados em bytes.
 * @param key     Chave de 1 byte (valor entre 0 e 255).
 */
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);

#endif // XOR_CIPHER_H
