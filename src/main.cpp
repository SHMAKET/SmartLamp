#include <Arduino.h>
#include <FastLED.h>
#include <EncButton.h>

#include "constants.h"

CRGB leds[NUM_LEDS];
ButtonT<BTN_PIN> touch;

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

    FastLED.setBrightness(10);
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.show();
}

void loop() {
    touch.tick();

    float vbat = readBatteryVoltage();
    Serial.printf("Battery voltage: %.3f V\n", vbat);
}
