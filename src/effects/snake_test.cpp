// effects/test_snake.cpp
// Test effect: 5px snake moving bottom-to-top
// This effect tests XY() function correctness
// Uses: speed (controls snake movement speed)

#include "../effectBase.h"
#include "../constants.h"

#define SNAKE_LENGTH 5

struct SnakeState {
    int8_t x;
    int8_t y;

    SnakeState() : x(0), y(0) {}
};

void effect_test_snake(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    if (ctx.state == nullptr) {
        ctx.state = new SnakeState();
    }

    SnakeState* state = (SnakeState*)ctx.state;

    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // скорость
    float speed_factor = params.speed / 50.0f;
    static float acc = 0;
    acc += speed_factor;

    if (acc >= 1.0f) {
        acc -= 1.0f;

        state->x++;

        // дошли до правой границы
        if (state->x >= WIDTH) {
            state->x = 0;
            state->y++;

            // дошли до верха — падаем вниз
            if (state->y >= HEIGHT) {
                state->y = 0;
            }
        }
    }

    for (int i = 0; i < SNAKE_LENGTH; i++) {
        int8_t x = state->x - i;

        if (x < 0) {
            x += WIDTH;
        }

        uint8_t br = 255 - i * 50;
        leds[XY(x, state->y)] = CRGB(0, 0.275 * br, 0.922 * br);
    }


    FastLED.setBrightness(params.brightness);
}