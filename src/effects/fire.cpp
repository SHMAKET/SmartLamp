// Fire 2018 effect - advanced fire simulation using Perlin noise
// Based on: https://gist.github.com/StefanPetrick/819e873492f344ebebac5bcd2fdd8aa8
// Uses: brightness, speed, k_factor (controls flame color/tint)

#include "../effectBase.h"
#include "../constants.h"

#define NUM_LAYERS 2
#define CENTER_X_MAJOR (WIDTH / 2 + (WIDTH % 2))
#define CENTER_Y_MAJOR (HEIGHT / 2 + (HEIGHT % 2))

// Internal state for Fire2018 effect (frame-rate independent)
struct FireState {
    uint32_t noise32_x[NUM_LAYERS];
    uint32_t noise32_y[NUM_LAYERS];
    uint32_t noise32_z[NUM_LAYERS];
    uint32_t scale32_x[NUM_LAYERS];
    uint32_t scale32_y[NUM_LAYERS];
    uint8_t noise3d[NUM_LAYERS][WIDTH][HEIGHT];
    uint8_t heat[NUM_LEDS];  // Heat buffer

    // Frame-rate independent accumulators (advance these by dt)
    uint32_t t_ms_accum;     // overall time accumulator in ms (scaled by params.speed)
    uint32_t ctrl_time1;     // time accumulator for ctrl1 noise
    uint32_t ctrl_time2;     // time accumulator for ctrl2 noise

    FireState() {
        memset(noise32_x, 0, sizeof(noise32_x));
        memset(noise32_y, 0, sizeof(noise32_y));
        memset(noise32_z, 0, sizeof(noise32_z));
        memset(scale32_x, 0, sizeof(scale32_x));
        memset(scale32_y, 0, sizeof(scale32_y));
        memset(noise3d, 0, sizeof(noise3d));
        memset(heat, 0, sizeof(heat));
        t_ms_accum = 0;
        ctrl_time1 = 0;
        ctrl_time2 = 0;
    }
};

void effect_fire(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    // Initialize state on first frame
    if (ctx.state == nullptr) {
        ctx.state = new FireState();
    }

    FireState* state = (FireState*)ctx.state;

    // Speed parameter controls animation speed (0-255)
    // 128 = normal speed
    float speed_factor = params.speed / 128.0f;

    // Advance accumulators by dt scaled with speed_factor -> frame-rate independent
    // Use 64-bit intermediates where needed
    uint32_t delta_ms = (uint32_t)((float)dt * speed_factor);
    state->t_ms_accum += delta_ms;
    // Advance ctrl times with different multipliers (matching previous usage of 11* and 13*)
    state->ctrl_time1 += (uint32_t)((uint64_t)11 * (uint64_t)delta_ms);
    state->ctrl_time2 += (uint32_t)((uint64_t)13 * (uint64_t)delta_ms);

    // Compute controls using the accumulated control times
    uint16_t ctrl1 = inoise16(state->ctrl_time1, 0, 0);
    uint16_t ctrl2 = inoise16(state->ctrl_time2, 100000, 100000);
    uint16_t ctrl  = (uint16_t)(((uint32_t)ctrl1 + (uint32_t)ctrl2) / 2);

    // --- Layer 0 (heatmap) parameters ---
    {
        const uint16_t speed = 25; // original code used 25
        // noise32_x depends on ctrl (as original). Recompute deterministically from ctrl:
        // Keep original relationship: noise32_x = 3 * ctrl * speed
        // Use 64-bit intermediates to avoid overflow before cast.
        uint64_t tmp_x = (uint64_t)3 * (uint64_t)ctrl * (uint64_t)speed;
        state->noise32_x[0] = (uint32_t)tmp_x;

        // noise32_y and noise32_z depend on elapsed time -> now use accumulated time
        uint64_t tmp_y = (uint64_t)20 * (uint64_t)state->t_ms_accum * (uint64_t)speed;
        uint64_t tmp_z = (uint64_t)5  * (uint64_t)state->t_ms_accum * (uint64_t)speed;
        state->noise32_y[0] = (uint32_t)tmp_y;
        state->noise32_z[0] = (uint32_t)tmp_z;

        state->scale32_x[0] = (uint32_t)((uint32_t)ctrl1 / 2);
        state->scale32_y[0] = (uint32_t)((uint32_t)ctrl2 / 2);
    }

    // Calculate noise data for layer 0 (heatmap)
    uint8_t layer = 0;
    for (uint8_t i = 0; i < WIDTH; i++) {
        uint32_t ioffset = state->scale32_x[layer] * (i - CENTER_X_MAJOR);
        for (uint8_t j = 0; j < HEIGHT; j++) {
            uint32_t joffset = state->scale32_y[layer] * (j - CENTER_Y_MAJOR);
            uint16_t data = (inoise16(state->noise32_x[layer] + ioffset,
                                    state->noise32_y[layer] + joffset,
                                    state->noise32_z[layer]) + 1);
            state->noise3d[layer][i][j] = data >> 8;
        }
    }

    // --- Layer 1 (brightness mask) parameters ---
    {
        const uint16_t speed = 20;
        uint64_t tmp_x = (uint64_t)3 * (uint64_t)ctrl * (uint64_t)speed;
        state->noise32_x[1] = (uint32_t)tmp_x;

        uint64_t tmp_y = (uint64_t)20 * (uint64_t)state->t_ms_accum * (uint64_t)speed;
        uint64_t tmp_z = (uint64_t)5  * (uint64_t)state->t_ms_accum * (uint64_t)speed;
        state->noise32_y[1] = (uint32_t)tmp_y;
        state->noise32_z[1] = (uint32_t)tmp_z;

        state->scale32_x[1] = (uint32_t)((uint32_t)ctrl1 / 2);
        state->scale32_y[1] = (uint32_t)((uint32_t)ctrl2 / 2);
    }

    // Calculate noise data for layer 1 (brightness mask)
    layer = 1;
    for (uint8_t i = 0; i < WIDTH; i++) {
        uint32_t ioffset = state->scale32_x[layer] * (i - CENTER_X_MAJOR);
        for (uint8_t j = 0; j < HEIGHT; j++) {
            uint32_t joffset = state->scale32_y[layer] * (j - CENTER_Y_MAJOR);
            uint16_t data = (inoise16(state->noise32_x[layer] + ioffset,
                                    state->noise32_y[layer] + joffset,
                                    state->noise32_z[layer]) + 1);
            state->noise3d[layer][i][j] = data >> 8;
        }
    }

    // Draw lowest line - seed the fire (use layer 0)
    for (uint8_t x = 0; x < WIDTH; x++) {
        state->heat[XY(x, HEIGHT - 1)] = state->noise3d[0][WIDTH - 1 - x][CENTER_Y_MAJOR - 1];
    }

    // Copy everything one line up
    for (uint8_t y = 0; y < HEIGHT - 1; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            state->heat[XY(x, y)] = state->heat[XY(x, y + 1)];
        }
    }

    // Dim based on noise
    for (uint8_t y = 0; y < HEIGHT - 1; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            uint8_t dim = state->noise3d[0][x][y];
            // High value = high flames
            dim = (uint8_t)((float)dim / 1.7f);
            dim = 255 - dim;
            state->heat[XY(x, y)] = scale8(state->heat[XY(x, y)], dim);
        }
    }

    // k_factor controls flame color/tint (0 = red/orange, higher = more yellow/green)
    // Map k_factor (-128..127) to color scale (0..100)
    float color_scale = (params.k_factor + 128) / 255.0f * 100.0f;

    // Render to LED array with color mapping
    for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            uint8_t heat_value = state->heat[XY(x, y)];

            // Map heat to color (red base + green component based on k_factor)
            uint8_t red = heat_value;
            uint8_t green = (uint8_t)((float)heat_value * color_scale * 0.01f);
            uint8_t blue = 0;

            leds[XY(x, HEIGHT - 1 - y)] = CRGB(red, green, blue);

            // Apply brightness mask from second noise layer
            leds[XY(x, HEIGHT - 1 - y)].nscale8(state->noise3d[1][x][y]);
        }
    }

    // Apply global brightness TODO: move to effectManager
    FastLED.setBrightness(params.brightness);
}
