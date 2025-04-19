#ifndef DISPLAY_H
#define DISPLAY_H

#include "galton.h"

void init_display();
void draw_histogram(int *histogram);
void draw_ball(Ball *ball);
void update_display(Ball *balls, int ball_count, int *histogram);
void draw_counter(int total_balls);

#endif