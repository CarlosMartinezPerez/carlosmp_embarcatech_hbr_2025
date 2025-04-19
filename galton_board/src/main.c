#include <stdio.h>
#include "pico/stdlib.h"
#include "galton.h"
#include "display.h"

#define BUTTON_A 5
#define BUTTON_B 6
#define DEBOUNCE_MS 200

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

    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

    init_display();

    Ball balls[MAX_BALLS];
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }

    for (int i = 0; i < CHANNELS; i++) {
        histogram[i] = 0;
    }
    total_balls = 0;

    extern float left_prob;
    static uint32_t last_press_a = 0;
    static uint32_t last_press_b = 0;
    static bool last_state_a = true;
    static bool last_state_b = true;

    int tick = 0;
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        bool state_a = gpio_get(BUTTON_A);
        bool state_b = gpio_get(BUTTON_B);

        // Detecta borda de descida para botão A (1 -> 0)
        if (!state_a && last_state_a && (now - last_press_a) > DEBOUNCE_MS) {
            if (left_prob < 90.0f) {
                left_prob += 10.0f;
            }
            last_press_a = now;
        }

        // Detecta borda de descida para botão B (1 -> 0)
        if (!state_b && last_state_b && (now - last_press_b) > DEBOUNCE_MS) {
            if (left_prob > 10.0f) {
                left_prob -= 10.0f;
            }
            last_press_b = now;
        }

        last_state_a = state_a;
        last_state_b = state_b;

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