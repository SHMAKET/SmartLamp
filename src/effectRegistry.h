#pragma once

#include "effectBase.h"
#include "constants.h"

// Forward declarations of effects
void effect_static_color(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt);
void effect_rainbow(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt);
void effect_fire(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt);
void effect_sinusoid (CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt);

// X-MACRO effect registration table
// Format: EFFECT(name, function, target_fps, default_brightness, default_speed, default_k_factor)
// 
// To add a new effect:
// 1. Create file effects/your_effect.cpp
// 2. Add forward declaration above
// 3. Add EFFECT(...) line to table below
// 4. Done. Compilation will automatically register the effect.


//                                                 FPS  Br  Spd  Kf
#define EFFECT_TABLE \
    EFFECT("static",        effect_static_color,    60, 10,   1, 0) \
    EFFECT("fire",          effect_fire,            60, 10, 120, 0) \
    EFFECT("rainbow",       effect_rainbow,         60, 10, 120, 0) \
    EFFECT("sinusoid",      effect_sinusoid,        60, 10, 120, -80)

// Generate descriptor array at compile-time
#define EFFECT(name, func, fps, br, spd, kf) \
    {name, func, fps, EffectParams(br, spd, kf)},

static const EffectDescriptor EFFECT_REGISTRY[] PROGMEM = {
    EFFECT_TABLE
};
#undef EFFECT

// Number of registered effects
#define EFFECT_COUNT (sizeof(EFFECT_REGISTRY) / sizeof(EffectDescriptor))