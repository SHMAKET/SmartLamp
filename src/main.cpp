#include "constants.h"

#include <Arduino.h>
#include <FastLED.h>
#include <GyverButton.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "effectManager.h"
#include "batteryManager.h"

CRGB leds[NUM_LEDS];
EffectManager effects(leds);
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
AsyncWebServer server(80);

enum class LampState : uint8_t { OFF, ON };
LampState currentState = LampState::ON;

void toggleState();
void setupWeb();

void setup() {
    Serial.begin(115200);
    delay(300);

    WiFi.mode(WIFI_AP);
    WiFi.setTxPower(WIFI_POWER_7dBm);
    WiFi.softAP(AP_SSID, AP_PASS);
    WiFi.softAPConfig(
        IPAddress (192,168,3,1),
        IPAddress (192,168,3,1),
        IPAddress (255,255,255,0)
    );
    
    //WiFi.begin("DENWIFI", "31415926");
    delay(500);
    
    Serial.println(WiFi.localIP().toString());

    // Initialize FastLED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MW);
    FastLED.clear();
    FastLED.show();

    touch.setDebounce(UB_DEB_TIME);
    touch.setTimeout(UB_HOLD_TIME);
    touch.setClickTimeout(UB_CLICK_TIME);

    analogReadResolution(12);

    delay(300);

    // Print all registered effects
    Serial.println("\n===Registered Effects===");
    for (uint8_t i = 0; i < effects.getEffectCount(); i++) {
        const EffectDescriptor* desc = effects.getEffectDescriptor(i);
        Serial.printf("[%d] %s (FPS: %d) ", i, desc->name, desc->target_fps);
        Serial.printf("    Defaults: brightness=%d, speed=%d, k_factor=%d\n",
                    desc->default_params.brightness,
                    desc->default_params.speed,
                    desc->default_params.k_factor);
    }

    // Set first effect with its default parameters
    effects.setEffect(3);

    // Запускаем файловую систему
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed");
        return;
    }
    setupWeb();

    delay(100);

    Serial.println("\nSystem ready!");
}

void loop() {
    touch.tick();
    
    // single click — on/off
    if (touch.isSingle()) { toggleState(); }
    
    // processing clicks only when lamp ON
    if (currentState == LampState::ON) {
        switch (touch.hasClicks() ? touch.getClicks() : 0U) {
            case 2U: effects.nextEffect(); break;
            case 3U: effects.prevEffect(); break;
        }
        
        uint32_t now = millis();
        if (effects.update(now)) { FastLED.show(); }
    }
}

void toggleState() {
    if (currentState == LampState::OFF) {
        currentState = LampState::ON;
    } else {
        currentState = LampState::OFF;
        FastLED.clear();
        FastLED.show();
    }
}

void setupWeb() {
    server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html");

    server.on("/toggleState", HTTP_GET, [](AsyncWebServerRequest *request) { 
        toggleState();
        request->send(200, "text/plain", "OK");
    });

    server.on("/batteryStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
        float vbat = readBatteryVoltage();
        request->send(200, "text/plain", "Battery voltage: " + String(vbat) +"V " + String(voltage2Percent(vbat)) + "%  ");
    });

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not Found");
    });

    server.begin();
}