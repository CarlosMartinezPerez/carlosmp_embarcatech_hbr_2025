#ifndef GALTON_H
#define GALTON_H

#define MAX_BALLS 10
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define CHANNELS 16
#define CHANNEL_WIDTH (SSD1306_WIDTH / CHANNELS)

typedef struct {
    float x, y;
    bool active;
    int collisions;
} Ball;

extern int histogram[CHANNELS];
extern int total_balls;

void test_randomness(int trials);
void calculate_statistics();
void init_ball(Ball *ball);
void update_ball(Ball *ball);
void register_ball_landing(Ball *ball);

#endif