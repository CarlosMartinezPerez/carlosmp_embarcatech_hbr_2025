#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"

#define FLASH_OFFSET  0x1FF000 // Altere para o offset desejado (a ser somado a 0x10000000)
#define DUMP_SIZE     4096    // Tamanho do dump

int main() {
    stdio_init_all();
    sleep_ms(3000); // Espera para conectar ao serial

    const uint8_t *flash_data = (const uint8_t *) (XIP_BASE + FLASH_OFFSET);

    printf("Dump da Flash:\n");
    
    for (int i = 0; i < DUMP_SIZE; i++) {
        // Se for o primeiro byte da linha, imprime o número da linha
        if (i % 16 == 0) {
            printf("%08X: ", FLASH_OFFSET + i);
        }

        // Imprime o byte em hexadecimal
        printf("%02X ", flash_data[i]);

        // Se for o último byte da linha, pula para a próxima linha
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    while (1){
        tight_loop_contents;
    }
}
