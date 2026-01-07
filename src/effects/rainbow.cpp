// effects/rainbow.cpp
// Medium effect: rotating rainbow
// Uses: brightness, speed

#include "../effectBase.h"
#include "../constants.h"


void rainbowHorVertRoutine(bool isVertical, CRGB* leds, const EffectParams& params, uint8_t hue) {
  for (uint8_t i = 0U; i < (isVertical?WIDTH:HEIGHT); i++) {
    CHSV thisColor = CHSV(((uint8_t)(hue + i * params.k_factor % 67U) * 2U), 255U, 255U);

    for (uint8_t j = 0U; j < (isVertical?HEIGHT:WIDTH); j++)      
      leds[XY((isVertical?i:j), (isVertical?j:i))] = thisColor;
  }
}
void effect_rainbow(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
  // speed: 0-255 → rotation speed of rainbow
  // speed = 128 - medium speed
  float speed_factor = params.speed / 128.0f;
  // Hue offset over time
  uint8_t hue_offset = (ctx.elapsed_ms * speed_factor * 0.05f); 

  switch (map(params.k_factor, -128, 127, 1, 3))
  {
    case 1: rainbowHorVertRoutine(false, leds, params, hue_offset); break;
    case 2:
    for (uint8_t i = 0U; i < WIDTH; i++)
      for (uint8_t j = 0U; j < HEIGHT; j++)
      {
        float twirlFactor = 9.0F * ((params.k_factor - 33) / 100.0F);
        CRGB thisColor = CHSV((uint8_t)(hue_offset + ((float)WIDTH / (float)HEIGHT * i + j * twirlFactor)), 255U, 255U);
        leds[XY(i, j)] = thisColor;
      }
    break;
    case 3: rainbowHorVertRoutine(true, leds, params, hue_offset); break;
  }
  
/*
  if (params.k_factor < -43) {
    rainbowHorVertRoutine(false, leds, params, hue_offset);
  } else if (params.k_factor > 42) {
    rainbowHorVertRoutine(true, leds, params, hue_offset);
  } else {
    for (uint8_t i = 0U; i < WIDTH; i++)
      for (uint8_t j = 0U; j < HEIGHT; j++)
      {
        float twirlFactor = 9.0F * ((params.k_factor+94) / 100.0F);
        CRGB thisColor = CHSV((uint8_t)(hue_offset + ((float)WIDTH / (float)HEIGHT * i + j * twirlFactor)), 255U, 255U);
        leds[XY(i, j)] = thisColor;
      }
  }
*/
  // Apply brightness
  FastLED.setBrightness(params.brightness);
}


/*
void effect_rainbow_old(CRGB* leds, const EffectParams& params, EffectContext& ctx, uint32_t dt) {
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
*/