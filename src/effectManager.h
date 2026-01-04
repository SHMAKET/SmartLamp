#pragma once

#include "effectBase.h"
#include "effectRegistry.h"
#include "constants.h"

class EffectManager {
private:
    CRGB* leds;                    // FastLED array
    uint8_t current_effect_id;
    EffectParams current_params;    // Current effect parameters (can change anytime)
    EffectContext context;
    uint32_t last_frame_time;
    uint32_t frame_interval_ms;
    
public:
    EffectManager(CRGB* led_array) 
        : leds(led_array)
        , current_effect_id(0)
        , last_frame_time(0)
        , frame_interval_ms(16)  // 60 FPS default
    {
        // Load default parameters for first effect
        loadDefaultParams(0);
    }
    
    // Get number of effects
    uint8_t getEffectCount() const {
        return EFFECT_COUNT;
    }
    
    // Get effect descriptor by ID
    const EffectDescriptor* getEffectDescriptor(uint8_t id) const {
        if (id >= EFFECT_COUNT) return nullptr;
        return &EFFECT_REGISTRY[id];
    }
    
    // Get effect name by ID
    const char* getEffectName(uint8_t id) const {
        const EffectDescriptor* desc = getEffectDescriptor(id);
        return desc ? desc->name : nullptr;
    }
    
    // Find effect ID by name
    int8_t findEffectByName(const char* name) const {
        for (uint8_t i = 0; i < EFFECT_COUNT; i++) {
            if (strcmp(EFFECT_REGISTRY[i].name, name) == 0) {
                return i;
            }
        }
        return -1;
    }
    
    // Current effect ID
    uint8_t getCurrentEffectId() const {
        return current_effect_id;
    }
    
    // Switch effect
    bool setEffect(uint8_t id) {
        if (id >= EFFECT_COUNT) return false;
        
        // Clean up old effect state if exists
        if (context.state) {
            // Note: specific cleanup depends on effect implementation
            // Each effect should handle its own state cleanup
            free(context.state);
            context.state = nullptr;
        }
        
        current_effect_id = id;
        context.reset();
        
        // Load default parameters for new effect
        loadDefaultParams(id);
        
        // Set target FPS
        const EffectDescriptor* desc = getEffectDescriptor(id);
        if (desc) {
            frame_interval_ms = 1000 / desc->target_fps;
        }
        
        return true;
    }
    
    // Next effect (cyclic)
    void nextEffect() {
        setEffect((current_effect_id + 1) % EFFECT_COUNT);
    }
    
    // Previous effect (cyclic)
    void prevEffect() {
        if (current_effect_id == 0) {
            setEffect(EFFECT_COUNT - 1);
        } else {
            setEffect(current_effect_id - 1);
        }
    }
    
    // Get current effect parameters (for reading/writing)
    EffectParams& getParams() {
        return current_params;
    }
    
    // Set parameters (immediate effect, no delay)
    void setParams(const EffectParams& params) {
        current_params = params;
        // TODO: нахера это проверка?
        current_params.validate();
    }
    
    // Set individual parameters (for convenience)
    void setBrightness(uint8_t value) {
        current_params.brightness = value;
    }
    
    void setSpeed(uint8_t value) {
        current_params.speed = value;
    }
    
    void setKFactor(int8_t value) {
        current_params.k_factor = value;
    }
    
    // Load default parameters for effect
    void loadDefaultParams(uint8_t id) {
        const EffectDescriptor* desc = getEffectDescriptor(id);
        if (desc) {
            current_params = desc->default_params;
        }
    }
    
    // Main update loop (call in loop())
    void update(uint32_t current_time_ms) {
        uint32_t dt = current_time_ms - last_frame_time;
        
        // FPS limiting
        if (dt < frame_interval_ms) {
            return;
        }
        
        // Call current effect function
        const EffectDescriptor* desc = getEffectDescriptor(current_effect_id);
        if (desc && desc->function) {
            // Effect receives current parameters (can be changed anytime)
            desc->function(leds, current_params, context, dt);
            
            // Update counters
            context.frame_count++;
            context.elapsed_ms += dt;
        }
        
        last_frame_time = current_time_ms;
        
        // FastLED.show() should be called in your main loop after this
    }
    
    // Force update (ignore FPS limit)
    void forceUpdate(uint32_t current_time_ms) {
        last_frame_time = 0;
        update(current_time_ms);
    }
    
    // Get frame interval for current effect
    uint32_t getFrameInterval() const {
        return frame_interval_ms;
    }
};