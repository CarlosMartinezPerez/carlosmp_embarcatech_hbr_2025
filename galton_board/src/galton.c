#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "galton.h"

int histogram[CHANNELS] = {0};
int total_balls = 0;
float left_prob = 50.0f; // Probabilidade inicial: 50% esquerda

bool random_direction() {
    return (get_rand_32() % 100) < left_prob; // Ex.: 60% esquerda -> true se rand < 60
}

void test_randomness(int trials) {
    int left = 0, right = 0;
    for (int i = 0; i < trials; i++) {
        if (random_direction() == 0) left++;
        else right++;
    }
    printf("Esquerda: %d (%.2f%%), Direita: %d (%.2f%%)\n",
            left, (float)left / trials * 100,
            right, (float)right / trials * 100);
}

void calculate_statistics() {
    if (total_balls == 0) {
        printf("Nenhuma bola registrada.\n");
        return;
    }

    float mean = 0.0f;
    for (int i = 0; i < CHANNELS; i++) {
        mean += (i + 1) * histogram[i];
    }
    mean /= total_balls;

    float variance = 0.0f;
    for (int i = 0; i < CHANNELS; i++) {
        variance += histogram[i] * ((i + 1) - mean) * ((i + 1) - mean);
    }
    variance /= total_balls;
    float std_dev = sqrtf(variance);

    printf("Total de Bolas: %d\n", total_balls);
    printf("Bins: ");
    for (int i = 0; i < CHANNELS; i++) {
        printf("[%d]: %d ", i + 1, histogram[i]);
    }
    printf("\nMédia: %.2f\nDesvio Padrão: %.2f\n", mean, std_dev);
}

void init_ball(Ball *ball) {
    ball->x = SSD1306_WIDTH / 2.0f;
    ball->y = 0.0f;
    ball->active = true;
    ball->collisions = 0;
}

void update_ball(Ball *ball) {
    if (!ball->active) return;

    ball->y += 1.0f;
    if (ball->collisions < 15 && ball->y >= (ball->collisions + 1) * (SSD1306_HEIGHT / 15.0f)) {
        bool dir = random_direction();
        if (dir) {
            ball->x -= 4.0f; // Esquerda
        } else {
            ball->x += 4.0f; // Direita
        }
        ball->collisions++;
    }

    if (ball->x < 0) ball->x = 0;
    if (ball->x >= SSD1306_WIDTH) ball->x = SSD1306_WIDTH - 1;
    if (ball->y >= SSD1306_HEIGHT) {
        ball->active = false;
    }
}

void register_ball_landing(Ball *ball) {
    int bin = (int)(ball->x / (SSD1306_WIDTH / CHANNELS));
    if (bin >= 0 && bin < CHANNELS) {
        histogram[bin]++;
        total_balls++;
    }
}

float get_left_probability() {
    return left_prob;
}