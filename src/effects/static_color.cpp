// effects/static_color.cpp
// Simple effect: static white color
// Uses: brightness (controls overall brightness)

#include "../effectBase.h"
#include "../constants.h"

void effect_static_color(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    // Fill entire matrix with white color at specified brightness
    fill_solid(leds, NUM_LEDS, CRGB::White);
    
    // Apply brightness to all LEDs TODO: move to effectManager
    FastLED.setBrightness(params.brightness);
    
    // For different colors, use:
    // fill_solid(leds, NUM_LEDS, CRGB::Red);
    // fill_solid(leds, NUM_LEDS, CRGB(255, 128, 0)); // Orange
}