#include "constants.h"
#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

Adafruit_INA219 ina219;

void InitIna219() {
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);
    pinMode(ADC_PIN, INPUT);
    if (!ina219.begin()) {
        Serial.println("Failed to find INA219 chip");
    }
}

// ─── InaResult ────────────────────────────────────────────────────────────────
typedef struct {
    float shuntvoltage;   // mV
    float busvoltage;     // V  (INA219 bus, excluding shunt)
    float current_mA;     // mA (positive = load current)
    float power_mW;       // mW
} InaResult;

InaResult ReadIna() {
    InaResult r;
    r.shuntvoltage = ina219.getShuntVoltage_mV();
    r.busvoltage   = ina219.getBusVoltage_V();
    r.current_mA   = ina219.getCurrent_mA();
    r.power_mW     = ina219.getPower_mW();
    return r;
}

String InaGet() {
    InaResult r = ReadIna();
    float loadvoltage = r.busvoltage + (r.shuntvoltage / 1000.0f);
    return  "\nBus Voltage:   " + String(r.busvoltage)   + " V"  +
            "\nShunt Voltage: " + String(r.shuntvoltage) + " mV" +
            "\nLoad Voltage:  " + String(loadvoltage)    + " V"  +
            "\nCurrent:       " + String(r.current_mA)   + " mA" +
            "\nPower:         " + String(r.power_mW)     + " mW";
}

// ─── Battery voltage: average of ADC and INA bus ──────────────────────────────
float ReadBatV() {
    // ADC channel averaged over 16 samples
    uint32_t sum = 0;
    const int samples = 16;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(ADC_PIN);
    }
    float adc   = sum / (float)samples;
    float vadc  = adc / ADC_MAX * ADC_REF;
    float v_adc = vadc * DIVIDER_RATIO;

    // Shunt is on the load side, so battery voltage = bus + shunt drop
    InaResult r = ReadIna();
    float v_ina = r.busvoltage + (r.shuntvoltage / 1000.0f);

    // Average both sources to reduce systematic error of each
    return (v_adc + v_ina) * 0.5f;
}

// ─── SOC: OCV curve for NCR18650B (1S3P) + IR-drop compensation ──────────────
//
// OCV points measured at C/20 (~170 mA per cell).
// Source: Panasonic NCR18650B datasheet + LiitoKala published measurements.
// Pack internal resistance 1S3P: ~33 mOhm (100 mOhm / 3 cells in parallel).
//
// Tune BAT_IR_OHM against a known load for best accuracy.

static const float BAT_IR_OHM = 0.033f;  // Ohm, total pack internal resistance (1S3P)

typedef struct {
    float   voltage;   // OCV, V
    uint8_t percent;
} LiIonPoint;


static const LiIonPoint curve[] = {
    {4.20f, 100},
    {4.15f,  97},
    {4.10f,  93},
    {4.05f,  88},
    {4.00f,  83},
    {3.95f,  77},
    {3.90f,  71},
    {3.85f,  65},
    {3.80f,  58},
    {3.75f,  51},
    {3.70f,  44},
    {3.65f,  37},
    {3.60f,  30},
    {3.55f,  23},
    {3.50f,  17},
    {3.45f,  12},
    {3.40f,   8},
    {3.30f,   4},
    {3.20f,   2},
    {3.00f,   0},
};
static const int CURVE_LEN = sizeof(curve) / sizeof(curve[0]);

// Linear interpolation over OCV lookup table
static uint8_t ocv2Percent(float ocv) {
    if (ocv >= curve[0].voltage)             return 100;
    if (ocv <= curve[CURVE_LEN - 1].voltage) return 0;
    for (int i = 0; i < CURVE_LEN - 1; i++) {
        if (ocv <= curve[i].voltage && ocv > curve[i + 1].voltage) {
            float dv = curve[i].voltage  - curve[i + 1].voltage;
            float dp = curve[i].percent  - curve[i + 1].percent;
            float k  = (curve[i].voltage - ocv) / dv;
            return (uint8_t)(curve[i].percent - dp * k);
        }
    }
    return 0;
}

// Public SOC function: compensates IR-drop to approximate OCV.
// current_mA — load current (positive when discharging, shunt on load side).
uint8_t voltage2Percent(float measuredV, float current_mA) {
    // Recover OCV: voltage sags under load, add back the IR drop
    float ocv = measuredV + (current_mA / 1000.0f) * BAT_IR_OHM;
    return ocv2Percent(ocv);
}