// effects/rainbow.cpp
// Средний эффект: бегущая радуга
// Использует: brightness, speed

#include "../effectBase.h"
#include <math.h>

// HSV в RGB конверсия
void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t& r, uint8_t& g, uint8_t& b) {
    if (s == 0) {
        r = g = b = v;
        return;
    }
    
    uint8_t region = h / 43;
    uint8_t remainder = (h - (region * 43)) * 6;
    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
}

void effect_rainbow(LedMatrix& matrix, EffectContext& ctx, uint32_t dt) {
    uint16_t width = matrix.getWidth();
    uint16_t height = matrix.getHeight();
    
    // speed: 0-255 → скорость вращения радуги
    // speed = 128 - средняя скорость
    float speed_factor = ctx.params.speed / 128.0f;
    
    // Смещение цвета со временем
    uint8_t hue_offset = (ctx.elapsed_ms * speed_factor * 0.05f);
    
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            // Вычислить угол для каждого пикселя
            float angle = atan2f(y - height/2, x - width/2);
            uint8_t hue = (uint8_t)(angle * 40.74f) + hue_offset;  // 40.74 = 255/(2*PI)
            
            uint8_t r, g, b;
            hsv_to_rgb(hue, 255, 255, r, g, b);
            matrix.setPixel(x, y, r, g, b);
        }
    }
}