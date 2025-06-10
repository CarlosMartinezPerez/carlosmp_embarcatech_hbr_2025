#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pico/stdlib.h>

int main() {
    stdio_init_all();
    sleep_ms(3000); // Tempo para abrir o monitor serial

    srand(time(NULL));

    long long num_sorteios[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    int num_testes = sizeof(num_sorteios) / sizeof(num_sorteios[0]);

    for (int i = 0; i < num_testes; i++) {
        long long sorteios = num_sorteios[i]; // Usando long long para o número de sorteios
        long long caras = 0;                 // Usando long long para os contadores
        long long coroas = 0;                // Usando long long para os contadores

        printf("--- Teste com %lld sorteios ---\n", sorteios); // Format specifier correto para long long

        if (sorteios >= 10000) {
            printf("Processando: [");
            fflush(stdout);
        }

        for (long long j = 0; j < sorteios; j++) { // Usando long long para o índice do loop
            int resultado = rand() % 2;
            if (resultado == 0) {
                caras++;
            } else {
                coroas++;
            }

            if (sorteios >= 10000 && (j + 1) % (sorteios / 100) == 0) {
                printf("=");
                fflush(stdout);
            }
        }

        if (sorteios >= 10000) {
            printf("]\n");
        }

        double porcentagem_cara = (double)caras / sorteios * 100.0;
        double porcentagem_coroa = (double)coroas / sorteios * 100.0;

        printf("Caras: %lld (%.2f%%)\n", caras, porcentagem_cara);   // Format specifier correto para long long
        printf("Coroas: %lld (%.2f%%)\n", coroas, porcentagem_coroa); // Format specifier correto para long long
        printf("\n");

        // Pequeno atraso após cada teste (pode ajudar)
        sleep_ms(100);
    }

    return 0;
}
