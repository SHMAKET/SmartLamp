#ifndef EFFECT_MANAGER_H
#define EFFECT_MANAGER_H

#include "effectBase.h"
#include "effectRegistry.h"

class EffectManager {
private:
    LedMatrix* matrix;
    uint8_t current_effect_id;
    EffectContext context;
    uint32_t last_frame_time;
    uint32_t frame_interval_ms;
    CRGB leds[NUM_LEDS];
    
public:
    EffectManager(LedMatrix* m) 
        : matrix(m)
        , current_effect_id(0)
        , last_frame_time(0)
        , frame_interval_ms(16)  // 60 FPS по умолчанию
    {
        FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    }
    
    // Получить количество эффектов
    uint8_t getEffectCount() const {
        return EFFECT_COUNT;
    }
    
    // Получить дескриптор эффекта по ID
    const EffectDescriptor* getEffectDescriptor(uint8_t id) const {
        if (id >= EFFECT_COUNT) return nullptr;
        return &EFFECT_REGISTRY[id];
    }
    
    // Получить имя эффекта по ID
    const char* getEffectName(uint8_t id) const {
        const EffectDescriptor* desc = getEffectDescriptor(id);
        return desc ? desc->name : nullptr;
    }
    
    // Найти ID эффекта по имени
    int8_t findEffectByName(const char* name) const {
        for (uint8_t i = 0; i < EFFECT_COUNT; i++) {
            if (strcmp(EFFECT_REGISTRY[i].name, name) == 0) {
                return i;
            }
        }
        return -1;
    }
    
    // Текущий эффект
    uint8_t getCurrentEffectId() const {
        return current_effect_id;
    }
    
    // Переключить эффект
    bool setEffect(uint8_t id) {
        if (id >= EFFECT_COUNT) return false;
        
        current_effect_id = id;
        context.reset();
        
        // Установить целевой FPS
        const EffectDescriptor* desc = getEffectDescriptor(id);
        if (desc) {
            frame_interval_ms = 1000 / desc->target_fps;
        }
        
        return true;
    }
    
    // Следующий эффект (циклично)
    void nextEffect() {
        setEffect((current_effect_id + 1) % EFFECT_COUNT);
    }
    
    // Предыдущий эффект (циклично)
    void prevEffect() {
        if (current_effect_id == 0) {
            setEffect(EFFECT_COUNT - 1);
        } else {
            setEffect(current_effect_id - 1);
        }
    }
    
    // Получить параметры текущего эффекта
    EffectParams& getParams() {
        return context.params;
    }
    
    // Установить параметры
    void setParams(const EffectParams& params) {
        context.params = params;
    }
    
    // Получить флаги текущего эффекта
    uint8_t getCurrentEffectFlags() const {
        const EffectDescriptor* desc = getEffectDescriptor(current_effect_id);
        return desc ? desc->flags : 0;
    }
    
    // Проверить, использует ли текущий эффект параметр
    bool hasSpeed() const {
        return getCurrentEffectFlags() & EFFECT_HAS_SPEED;
    }
    
    bool hasKFactor() const {
        return getCurrentEffectFlags() & EFFECT_HAS_KFACTOR;
    }
    
    // Главный цикл обновления (вызывать в loop())
    void update(uint32_t current_time_ms) {
        uint32_t dt = current_time_ms - last_frame_time;
        
        // Ограничение FPS
        if (dt < frame_interval_ms) {
            return;
        }
        
        // Вызвать функцию текущего эффекта
        const EffectDescriptor* desc = getEffectDescriptor(current_effect_id);
        if (desc && desc->function) {
            desc->function(*matrix, context, dt);
            
            // Применить глобальную яркость
            matrix->applyBrightness(context.params.brightness);
            
            // Обновить счетчики
            context.frame_count++;
            context.elapsed_ms += dt;
        }
        
        last_frame_time = current_time_ms;
        
        // Здесь должна быть отправка данных на WS2812
        // Это твоя зона ответственности, например:
        // FastLED.show();
        // или
        // neopixelWrite(...);

        uint8_t * buffer = matrix->getBuffer();

        for (int i = 0; i < NUM_LEDS; i++) {
            // берем 3 байта на каждый светодиод
            leds[i] = CRGB(buffer[i*3], buffer[i*3+1], buffer[i*3+2]);
        }
        FastLED.show();
    }
    
    // Форсировать обновление (игнорируя FPS limit)
    void forceUpdate(uint32_t current_time_ms) {
        last_frame_time = 0;
        update(current_time_ms);
    }
};

#endif // EFFECT_MANAGER_H