// effects/fire.cpp
// Complex effect: fire simulation
// Uses: brightness, speed, k_factor

#include "../effectBase.h"
#include "../constants.h"

// Internal state for fire effect
struct FireState {
    uint8_t* heat_map;  // Heat map buffer
    
    FireState() {
        heat_map = new uint8_t[NUM_LEDS];
        memset(heat_map, 0, NUM_LEDS);
    }
    
    ~FireState() {
        delete[] heat_map;
    }
};

// Fire color palette (black → red → orange → yellow → white)
CRGB fire_palette(uint8_t heat) {
    if (heat < 85) {
        // Black → dark red
        return CRGB(heat * 3, 0, 0);
    } else if (heat < 170) {
        // Dark red → orange  
        heat -= 85;
        return CRGB(255, heat * 3, 0);
    } else {
        // Orange → yellow → white
        heat -= 170;
        return CRGB(255, 255, heat * 3);
    }
}

void effect_fire(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
    // Initialize state on first frame
    if (ctx.state == nullptr) {
        ctx.state = new FireState();
    }
    
    FireState* state = (FireState*)ctx.state;
    
    // Parameters:
    // speed: 0-255 → fire spread speed (128 = medium)
    // k_factor: -128..127 → cooling intensity
    //   k_factor < 0 = slow cooling (tall flames)
    //   k_factor > 0 = fast cooling (short flames)
    
    uint8_t cooling = 55 + params.k_factor / 4;  // 23..87
    uint8_t sparking = 120 + params.speed / 2;   // 120..247
    
    // Step 1: Cool down (top to bottom)
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t cooldown = random8(0, cooling);
        if (state->heat_map[i] > cooldown) {
            state->heat_map[i] -= cooldown;
        } else {
            state->heat_map[i] = 0;
        }
    }
    
    // Step 2: Heat rises (diffusion)
    for (uint16_t y = HEIGHT - 1; y >= 2; y--) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            uint16_t idx = XY(x, y);
            uint16_t idx_below = XY(x, y - 1);
            uint16_t idx_left = (x > 0) ? XY(x - 1, y - 1) : idx_below;
            uint16_t idx_up = XY(x, y - 2);
            
            state->heat_map[idx] = (state->heat_map[idx_below] + 
                                    state->heat_map[idx_left] + 
                                    state->heat_map[idx_below] + 
                                    state->heat_map[idx_up]) / 4;
        }
    }
    
    // Step 3: Randomly ignite new sparks at bottom
    for (uint16_t x = 0; x < WIDTH; x++) {
        if (random8() < sparking) {
            uint16_t idx = XY(x, 0);
            state->heat_map[idx] = random8(160, 255);
        }
    }
    
    // Step 4: Render to LED matrix
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = fire_palette(state->heat_map[i]);
    }
    
    // Apply brightness
    FastLED.setBrightness(params.brightness);
}

// NOTE: State cleanup happens automatically in EffectManager::setEffect()
// when switching to a different effect