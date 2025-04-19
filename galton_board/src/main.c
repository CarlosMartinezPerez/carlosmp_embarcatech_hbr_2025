#include <stdio.h>
#include "pico/stdlib.h"
#include "galton.h"
#include "display.h"

/*
int main() { // Função para teste de aleatoriedade durante o desenvolvimento
    stdio_init_all();
    sleep_ms(2000);  // Tempo para abrir o monitor serial
    test_randomness(100000); // Altere a quantidade de  testes
}*/

int main() {
    stdio_usb_init();
    sleep_ms(2000);
    printf("Iniciando Galton Board...\n");

    init_display();

    Ball balls[MAX_BALLS];
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }

    for (int i = 0; i < CHANNELS; i++) {
        histogram[i] = 0;
    }
    total_balls = 0;

    int tick = 0;
    while (true) {
        if (tick % 5 == 0) {
            for (int i = 0; i < MAX_BALLS; i++) {
                if (!balls[i].active) {
                    init_ball(&balls[i]);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_BALLS; i++) {
            if (balls[i].active) {
                update_ball(&balls[i]);
                if (!balls[i].active) {
                    register_ball_landing(&balls[i]);
                    if (total_balls % 100 == 0 && total_balls > 0) {
                        calculate_statistics();
                    }
                }
            }
        }

        for (int i = 0; i < CHANNELS; i++) {
            if (histogram[i] < 0) histogram[i] = 0;
        }
        update_display(balls, MAX_BALLS, histogram);

        tick++;
        sleep_ms(50);
    }

    return 0;
}