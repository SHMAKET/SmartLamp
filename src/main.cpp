#include "constants.h"
#include "effectManager.h"

#include <Arduino.h>
#include <FastLED.h>
#include <GyverButton.h>

CRGB leds[NUM_LEDS];
EffectManager effects(leds);
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

float readBatteryVoltage() {
    uint32_t sum = 0;
    const int samples = 32;

    for (int i = 0; i < samples; i++) {
        sum += analogRead(ADC_PIN);
    }

    float adc = sum / (float)samples;
    float vadc = adc / ADC_MAX * ADC_REF;
    return vadc * DIVIDER_RATIO;
}

void setup() {
    Serial.begin(115200);

    // Initialize FastLED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MW);
    FastLED.clear();
    FastLED.show();

    touch.setDebounce(UB_DEB_TIME);
    touch.setTimeout(UB_HOLD_TIME);
    touch.setClickTimeout(UB_CLICK_TIME);

    analogReadResolution(12);

    delay(500);

    // Print all registered effects
    Serial.println("=== Registered Effects ===");
    for (uint8_t i = 0; i < effects.getEffectCount(); i++) {
        const EffectDescriptor* desc = effects.getEffectDescriptor(i);
        Serial.printf("[%d] %s (FPS: %d)\n", i, desc->name, desc->target_fps);
        Serial.printf("    Defaults: brightness=%d, speed=%d, k_factor=%d\n",
                    desc->default_params.brightness,
                    desc->default_params.speed,
                    desc->default_params.k_factor);
    }
    
    // Set first effect with its default parameters
    effects.setEffect(2);

    Serial.println("\nSystem ready!");
}

void loop() {
    uint32_t now = millis();
    
    if (touch.isClick()) {
        effects.nextEffect();
        
        float vbat = readBatteryVoltage();
        Serial.printf("Battery voltage: %.3f V\n", vbat);
    }
    
    effects.update(now);
    FastLED.show();
    touch.tick();
}
