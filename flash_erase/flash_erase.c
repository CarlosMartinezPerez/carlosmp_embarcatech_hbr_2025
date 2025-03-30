#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define FLASH_TARGET_OFFSET 0x1FF000
#define FLASH_TARGET_ADDR   (XIP_BASE + FLASH_TARGET_OFFSET)

void dump_flash(uint32_t address, size_t size) {
    const uint8_t *ptr = (const uint8_t *)address;
    for (size_t i = 0; i < size; i++) {
        if (i % 16 == 0) {
            printf("\n%08X: ", address + i);
        }
        printf("%02X ", ptr[i]);
    }
    printf("\n");
}

int main() {
    stdio_init_all();
    sleep_ms(3000); // Aguarda para estabilizar a comunicação serial

    printf("Conteúdo da Flash antes de apagar:");
    dump_flash(FLASH_TARGET_ADDR, FLASH_SECTOR_SIZE);

    printf("\nApagando setor da Flash no endereço: 0x%08X\n", FLASH_TARGET_ADDR);
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_ADDR - XIP_BASE, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);

    printf("\nConteúdo da Flash após apagar:");
    dump_flash(FLASH_TARGET_ADDR, FLASH_SECTOR_SIZE);

    printf("\nOperação concluída!\n");
    return 0;
}
