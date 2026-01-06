#include "constants.h"
#include <Arduino.h>

float readBatteryVoltage() {
    uint32_t sum = 0;
    const int samples = 16;

    for (int i = 0; i < samples; i++) {
        sum += analogRead(ADC_PIN);
    }

    float adc = sum / (float)samples;
    float vadc = adc / ADC_MAX * ADC_REF;
    return vadc * DIVIDER_RATIO;
}

typedef struct {
    float voltage;
    uint8_t percent;
} LiIonPoint;

#pragma region Replace 
//TODO: Replace voltage-only SOC estimation with current-integrated model
static const LiIonPoint curve[] = {
    {4.20,100},
    {4.10, 90},
    {4.00, 80},
    {3.90, 70},
    {3.80, 60},
    {3.70, 50},
    {3.60, 40},
    {3.50, 30},
    {3.40, 20},
    {3.30, 10},
    {3.20, 5 },
    {3.00, 0 }
};

uint8_t voltage2Percent(float v)
{
    if (v >= curve[0].voltage) return 100;
    if (v <= curve[11].voltage) return 0;

    for (int i = 0; i < 11; i++) {
        if (v <= curve[i].voltage && v > curve[i + 1].voltage) {
            float dv = curve[i].voltage - curve[i + 1].voltage;
            float dp = curve[i].percent - curve[i + 1].percent;
            float k = (curve[i].voltage - v) / dv;
            return curve[i].percent - (uint8_t)(dp * k);
        }
    }

    return 0;
}
#pragma endregion

uint8_t readBatteryPercent() { return voltage2Percent(readBatteryVoltage()); }