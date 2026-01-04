#pragma once

#include <stdint.h>
#include <FastLED.h>

#include "constants.h"

// Forward declarations
struct EffectContext;

// Effect parameter structure - each effect has its own independent settings
struct EffectParams {
    uint8_t brightness;  // 0-255
    uint8_t speed;       // 0-255
    int8_t k_factor;     // -128..127
    
    EffectParams() : brightness(255), speed(128), k_factor(0) {}
    
    EffectParams(uint8_t br, uint8_t spd, int8_t kf) 
        : brightness(br), speed(spd), k_factor(kf) {}
    
    // Validation
    void validate() {
        // brightness and speed are always valid (0-255)
        // k_factor is always valid (-128..127)
    }
};

// Effect function signature
// leds - FastLED array for rendering
// params - current effect parameters (can change at any moment)
// ctx - execution context with state and counters
// dt - delta time since last frame in milliseconds
typedef void (*EffectFunction)(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt);

// Effect metadata (stored in Flash)
struct EffectDescriptor {
    const char* name;           // Code name: "fire", "rainbow", "static"
    EffectFunction function;    // Pointer to effect function
    uint8_t target_fps;        // Target FPS (60, 30, 24)
    
    // Default parameters for this effect
    EffectParams default_params;
};

// Execution context for effect
struct EffectContext {
    uint32_t frame_count;       // Frame counter
    uint32_t elapsed_ms;        // Elapsed time in milliseconds
    void* state;                // Pointer to internal state (optional)
    
    EffectContext() : frame_count(0), elapsed_ms(0), state(nullptr) {}
    
    // Reset context when switching effects
    void reset() {
        frame_count = 0;
        elapsed_ms = 0;
        // state is cleared manually by effect if needed
    }
};


constexpr bool HORIZONTAL =
    (STRIP_DIRECTION == 0 || STRIP_DIRECTION == 2);

constexpr bool PRIMARY_REVERSED =
    HORIZONTAL
        ? (STRIP_DIRECTION == 2)   // left
        : (STRIP_DIRECTION == 1);  // up

constexpr bool SECONDARY_REVERSED =
    HORIZONTAL
        ? (CONNECTION_ANGLE == 0 || CONNECTION_ANGLE == 3)
        : (CONNECTION_ANGLE == 2 || CONNECTION_ANGLE == 3);

constexpr bool ZIGZAG = (MATRIX_TYPE == 0);

// converts mantric coordinates into an LED index on the strip
inline uint16_t XY(uint8_t x, uint8_t y)
{
    y = (HEIGHT - 1) - y;

    uint16_t primary, secondary;
    uint16_t primary_len;

    if constexpr (HORIZONTAL) {
        primary = x;
        secondary = y;
        primary_len = WIDTH;
    } else {
        primary = y;
        secondary = x;
        primary_len = HEIGHT;
    }

    if constexpr (PRIMARY_REVERSED) {
        primary = primary_len - 1 - primary;
    }

    if constexpr (SECONDARY_REVERSED) {
        secondary = (HORIZONTAL ? HEIGHT : WIDTH) - 1 - secondary;
    }

    uint16_t idx;

    if constexpr (!ZIGZAG) {
        // parallel
        idx = primary;
    } else {
        // zigzag
        if (secondary & 1u) {
            idx = primary_len - 1 - primary;
        } else {
            idx = primary;
        }
    }

    return secondary * primary_len + idx;
}