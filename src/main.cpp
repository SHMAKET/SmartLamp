#include <Arduino.h>
#include <FastLED.h>
#include <EncButton.h>

#include "constants.h"
#include "effectManager.h"

ButtonT<BTN_PIN> touch;
LedMatrix matrix(WIDTH, HEIGHT);
EffectManager effects(&matrix);

float readBatteryVoltage() {
    uint32_t sum = 0;
    const int samples = 32;

    for (int i = 0; i < samples; i++) {
        sum += analogRead(ADC_PIN);
        delayMicroseconds(200);
    }

    float adc = sum / (float)samples;
    float vadc = adc / ADC_MAX * ADC_REF;
    return vadc * DIVIDER_RATIO;
}

void setup() {
    Serial.begin(115200);

    pinMode(BTN_PIN, INPUT);
    analogReadResolution(12);

    delay(100);

    // Вывести список всех эффектов
    Serial.println("=== Registered Effects ===");
    for (uint8_t i = 0; i < effects.getEffectCount(); i++) {
        const EffectDescriptor* desc = effects.getEffectDescriptor(i);
        Serial.printf("[%d] %s (FPS: %d, Flags: 0x%02X)\n", 
                    i, desc->name, desc->target_fps, desc->flags);
    }
    
    // Установить начальные параметры
    EffectParams params;
    params.brightness = 10;
    params.speed = 128;
    params.k_factor = 0;
    effects.setParams(params);
    
    // Установить первый эффект
    effects.setEffect(0);
}

void loop() {
    uint32_t now = millis();
    
    touch.tick();
    effects.update(now);
    float vbat = readBatteryVoltage();
    
    if (touch.click()) {
        Serial.printf("Battery voltage: %.3f V\n", vbat);
        effects.nextEffect();
    }
}
