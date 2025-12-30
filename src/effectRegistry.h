#ifndef EFFECT_REGISTRY_H
#define EFFECT_REGISTRY_H

#include "effectBase.h"

// Forward declarations эффектов
void effect_static_color(LedMatrix& matrix, EffectContext& ctx, uint32_t dt);
void effect_rainbow(LedMatrix& matrix, EffectContext& ctx, uint32_t dt);
void effect_fire(LedMatrix& matrix, EffectContext& ctx, uint32_t dt);

// X-MACRO таблица эффектов
// Формат: EFFECT(name, function, flags, target_fps)
// 
// Чтобы добавить новый эффект:
// 1. Создай файл effects/your_effect.cpp
// 2. Добавь forward declaration выше
// 3. Добавь строку EFFECT(...) в таблицу ниже
// 4. Всё. Компиляция автоматически зарегистрирует эффект.
//
#define EFFECT_TABLE \
    EFFECT("static",  effect_static_color, EFFECT_NONE,                                    60) \
    EFFECT("rainbow", effect_rainbow,      EFFECT_HAS_SPEED,                               30) \
    EFFECT("fire",    effect_fire,         EFFECT_HAS_SPEED | EFFECT_HAS_KFACTOR,          24)

// Генерация массива дескрипторов в compile-time
#define EFFECT(name, func, flags, fps) {name, func, flags, fps},
static const EffectDescriptor EFFECT_REGISTRY[] = {
    EFFECT_TABLE
};
#undef EFFECT

// Количество зарегистрированных эффектов
#define EFFECT_COUNT (sizeof(EFFECT_REGISTRY) / sizeof(EffectDescriptor))

#endif // EFFECT_REGISTRY_H