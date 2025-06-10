#ifndef DISPLAY_H
#define DISPLAY_H

#include "galton.h"

void init_display();
void clear_display_buffer();
void ssd1306_update_display();
void ssd1306_setup();
void draw_histogram(int *histogram);
void draw_ball(Ball *ball);
void update_display(Ball *balls, int ball_count, int *histogram);
void draw_probabilities(float left_prob);

#endif