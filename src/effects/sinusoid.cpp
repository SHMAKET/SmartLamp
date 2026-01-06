// Original: https://github.com/gunner47/GyverLamp/blob/master/firmware/GyverLamp_v1.4/effects.ino
// Read more: https://www.youtube.com/watch?v=mubH-w_gwdA
//
// Uses: brightness, speed, k_factor
// k_factor: selects which variant (0-8, mapped from -128..127)

#include "../effectBase.h"
#include "../constants.h"

// Helper macro for square root (optimize based on your needs)
#define SQRT_VARIANT(x) sqrtf(x)

struct Sinusoid3State {
    float emitter_x;
    float emitter_y;
    float speedfactor;
    
    Sinusoid3State() {
        emitter_x = WIDTH * 0.5f;
        emitter_y = HEIGHT * 0.5f;
        speedfactor = 0.0f;
    }
};

void effect_sinusoid(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    // Initialize state on first frame
    if (ctx.state == nullptr) {
        ctx.state = new Sinusoid3State();
    }
    
    Sinusoid3State* state = (Sinusoid3State*)ctx.state;
    
    // Map k_factor (-128..127) to variant selection (0-8)
    uint8_t variant = map(params.k_factor, -128, 127, 0, 8);
    
    // Calculate parameters
    float e_s3_size = 3.0f * params.speed / 100.0f + 2.0f;  // amplitude of curves
    state->speedfactor = 0.00145f * params.speed + 0.015f;
    
    uint32_t time_shift = ctx.elapsed_ms & 0xFFFFFF;  // overflow protection
    
    // Scale calculations (map speed to different scales for variants)
    uint16_t _scale = ((params.speed % 9) * 10 + 80) << 7;
    float _scale2 = float(params.speed % 9) * 0.2f + 0.4f;
    uint16_t _scale3 = (params.speed % 9) * 1638 + 3276;
    
    // Calculate three moving centers using Lissajous curves
    float center1x = float(e_s3_size * sin16(state->speedfactor * 72.0874f * time_shift)) / 32767.0f;
    float center1y = float(e_s3_size * cos16(state->speedfactor * 98.301f  * time_shift)) / 32767.0f;
    float center2x = float(e_s3_size * sin16(state->speedfactor * 68.8107f * time_shift)) / 32767.0f;
    float center2y = float(e_s3_size * cos16(state->speedfactor * 65.534f  * time_shift)) / 32767.0f;
    float center3x = float(e_s3_size * sin16(state->speedfactor * 134.3447f * time_shift)) / 32767.0f;
    float center3y = float(e_s3_size * cos16(state->speedfactor * 170.3884f * time_shift)) / 32767.0f;
    
    CRGB color;
    
    switch(variant) {
        case 0: // Sinusoid I
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    int8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.r = v;
                    
                    cx = x - state->emitter_x + center3x;
                    cy = y - state->emitter_y + center3y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.b = v;
                    color.g = 0;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 1: // Sinusoid II
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    uint8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.r = v;
                    
                    cx = x - state->emitter_x + center2x;
                    cy = y - state->emitter_y + center2y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.g = (v - (min(v, color.r) >> 1)) >> 1;
                    color.b = color.g >> 2;
                    color.r = max(v, color.r);
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 2: // Sinusoid III
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    int8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.r = v;
                    
                    cx = x - state->emitter_x + center2x;
                    cy = y - state->emitter_y + center2y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.b = v;
                    
                    cx = x - state->emitter_x + center3x;
                    cy = y - state->emitter_y + center3y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.g = v;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 3: // Sinusoid IV
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    int8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy) + time_shift * state->speedfactor * 100)) / 32767.0f);
                    color.r = ~v;
                    
                    cx = x - state->emitter_x + center2x;
                    cy = y - state->emitter_y + center2y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy) + time_shift * state->speedfactor * 100)) / 32767.0f);
                    color.g = ~v;
                    
                    cx = x - state->emitter_x + center3x;
                    cy = y - state->emitter_y + center3y;
                    v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy) + time_shift * state->speedfactor * 100)) / 32767.0f);
                    color.b = ~v;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 4: // Colored sinusoid
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    int8_t v = 127 * (1 + float(sin16(_scale * (beatsin16(2, 1000, 1750) / 2550.0f) * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.r = v;
                    
                    v = 127 * (1 + float(sin16(_scale * (beatsin16(1, 570, 1050) / 2250.0f) * SQRT_VARIANT(cx * cx + cy * cy) + 13 * time_shift * state->speedfactor)) / 32767.0f);
                    color.b = v;
                    
                    v = 127 * (1 + float(cos16(_scale * (beatsin16(3, 1900, 2550) / 2550.0f) * SQRT_VARIANT(cx * cx + cy * cy) + 41 * time_shift * state->speedfactor)) / 32767.0f);
                    color.g = v;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 5: // Sinusoid in net
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    int8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy) + time_shift * state->speedfactor * 5)) / 32767.0f);
                    color.g = ~v;
                    
                    v = 127 * (1 + float(sin16(_scale * (x + 0.005f * time_shift * state->speedfactor))) / 32767.0f);
                    color.b = ~v;
                    
                    v = 127 * (1 + float(sin16(_scale * (y + 0.0055f * time_shift * state->speedfactor))) / 32767.0f);
                    color.r = ~v;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 6: // Spiral
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    uint8_t v = 127 * (1 + sinf(3 * atan2(cy, cx) + _scale2 * hypot(cy, cx)));
                    
                    // Cut out spiral center
                    float d = SQRT_VARIANT(cx * cx + cy * cy) / 10.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    color.r = v;
                    
                    cx = x - state->emitter_x + center2x;
                    cy = y - state->emitter_y + center2y;
                    v = 127 * (1 + sinf(3 * atan2(cy, cx) + _scale2 * hypot(cy, cx)));
                    d = SQRT_VARIANT(cx * cx + cy * cy) / 10.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    color.b = v;
                    
                    cx = x - state->emitter_x + center3x;
                    cy = y - state->emitter_y + center3y;
                    v = 127 * (1 + float(sin16(atan2(cy, cx) * 31255 + _scale3 * hypot(cy, cx))) / 32767.0f);
                    d = SQRT_VARIANT(cx * cx + cy * cy) / 10.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    color.g = v;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 7: // Spiral variant 1
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    uint8_t v = 127 * (1 + float(sin16(atan2(cy, cx) * 31255 + _scale3 * hypot(cy, cx))) / 32767.0f);
                    
                    float d = SQRT_VARIANT(cx * cx + cy * cy) / 10.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    color.g = v;
                    
                    cx = x - state->emitter_x + center3x;
                    cy = y - state->emitter_y + center3y;
                    v = 127 * (1 + float(sin16(atan2(cy, cx) * 31255 + _scale3 * hypot(cy, cx))) / 32767.0f);
                    d = SQRT_VARIANT(cx * cx + cy * cy) / 10.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    color.r = v;
                    color.b = 0;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
            
        case 8: // Spiral variant 2
            for (uint8_t y = 0; y < HEIGHT; y++) {
                for (uint8_t x = 0; x < WIDTH; x++) {
                    float cx = x - state->emitter_x + center1x;
                    float cy = y - state->emitter_y + center1y;
                    uint8_t v = 127 * (1 + float(sin16(_scale * SQRT_VARIANT(cx * cx + cy * cy))) / 32767.0f);
                    color.g = v;
                    
                    cx = x - state->emitter_x + center2x;
                    cy = y - state->emitter_y + center2y;
                    v = 127 * (1 + float(sin16(atan2(cy, cx) * 31255 + _scale3 * hypot(cy, cx))) / 32767.0f);
                    
                    float d = SQRT_VARIANT(cx * cx + cy * cy) / 16.0f;
                    if (d < 0.06f) d = 0.06f;
                    if (d < 1.0f)
                        v = constrain(v - int16_t(1 / d / d), 0, 255);
                    
                    color.g = max(v, color.g);
                    color.b = v;
                    color.r = 0;
                    
                    leds[XY(x, y)] = color;
                }
            }
            break;
    }
    
    // Apply brightness
    FastLED.setBrightness(params.brightness);
}