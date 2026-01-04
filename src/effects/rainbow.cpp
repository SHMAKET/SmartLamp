// effects/rainbow.cpp
// Medium effect: rotating rainbow
// Uses: brightness, speed

#include "../effectBase.h"
#include "../constants.h"

void effect_rainbow(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    // speed: 0-255 → rotation speed of rainbow
    // speed = 128 - medium speed
    float speed_factor = params.speed / 128.0f;
    
    // Hue offset over time
    uint8_t hue_offset = (ctx.elapsed_ms * speed_factor * 0.05f);
    
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            // Calculate angle for each pixel
            float angle = atan2f(y - HEIGHT/2.0f, x - WIDTH/2.0f);
            uint8_t hue = (uint8_t)(angle * 40.74f) + hue_offset;  // 40.74 = 255/(2*PI)
            
            leds[XY(x, y)] = CHSV(hue, 255, 255);
        }
    }
    
    // Apply brightness
    FastLED.setBrightness(params.brightness);
}