// effects/fire.cpp
// Сложный эффект: симуляция огня
// Использует: brightness, speed, k_factor

#include "../effectBase.h"
#include <stdlib.h>

// Внутреннее состояние эффекта
struct FireState {
    uint8_t* heat_map;  // Тепловая карта
    uint16_t width;
    uint16_t height;
    
    FireState(uint16_t w, uint16_t h) : width(w), height(h) {
        heat_map = new uint8_t[w * h];
        memset(heat_map, 0, w * h);
    }
    
    ~FireState() {
        delete[] heat_map;
    }
};

// Палитра огня (черный -> красный -> оранжевый -> желтый -> белый)
void fire_palette(uint8_t heat, uint8_t& r, uint8_t& g, uint8_t& b) {
    // heat: 0-255
    
    if (heat < 85) {
        // Черный -> темно-красный
        r = heat * 3;
        g = 0;
        b = 0;
    } else if (heat < 170) {
        // Темно-красный -> оранжевый
        heat -= 85;
        r = 255;
        g = heat * 3;
        b = 0;
    } else {
        // Оранжевый -> желтый -> белый
        heat -= 170;
        r = 255;
        g = 255;
        b = heat * 3;
    }
}

inline long random(long min, long max) {
    return min + (random() % (max - min));
}

void effect_fire(LedMatrix& matrix, EffectContext& ctx, uint32_t dt) {
    uint16_t width = matrix.getWidth();
    uint16_t height = matrix.getHeight();
    
    // Инициализация состояния при первом кадре
    if (ctx.state == nullptr) {
        ctx.state = new FireState(width, height);
    }
    
    FireState* state = (FireState*)ctx.state;
    
    // Параметры:
    // speed: 0-255 → скорость распространения огня (128 = средняя)
    // k_factor: -128..127 → интенсивность охлаждения
    //   k_factor < 0 = медленное охлаждение (высокое пламя)
    //   k_factor > 0 = быстрое охлаждение (низкое пламя)
    
    uint8_t cooling = 55 + ctx.params.k_factor / 4;  // 23..87
    uint8_t sparking = 120 + ctx.params.speed / 2;   // 120..247
    
    // Шаг 1: Охлаждение (сверху вниз)
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint8_t cooldown = random(0, cooling);
            uint16_t idx = y * width + x;
            
            if (state->heat_map[idx] > cooldown) {
                state->heat_map[idx] -= cooldown;
            } else {
                state->heat_map[idx] = 0;
            }
        }
    }
    
    // Шаг 2: Поднятие тепла вверх (диффузия)
    for (uint16_t y = height - 1; y >= 2; y--) {
        for (uint16_t x = 0; x < width; x++) {
            uint16_t idx = y * width + x;
            uint16_t idx_below = (y - 1) * width + x;
            
            state->heat_map[idx] = (state->heat_map[idx_below] + 
                                    state->heat_map[idx_below - 1] + 
                                    state->heat_map[idx_below] + 
                                    state->heat_map[idx - width]) / 4;
        }
    }
    
    // Шаг 3: Новые искры внизу
    for (uint16_t x = 0; x < width; x++) {
        if (random(0, 255) < sparking) {
            uint16_t idx = x;
            state->heat_map[idx] = random(160, 255);
        }
    }
    
    // Шаг 4: Отрисовка на LED матрицу
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            uint16_t idx = y * width + x;
            uint8_t heat = state->heat_map[idx];
            
            uint8_t r, g, b;
            fire_palette(heat, r, g, b);
            matrix.setPixel(x, y, r, g, b);
        }
    }
}

// ВАЖНО: При удалении эффекта нужно очистить state
// Это делается в твоем коде при переключении:
// if (ctx.state) {
//     delete (FireState*)ctx.state;
//     ctx.state = nullptr;
// }