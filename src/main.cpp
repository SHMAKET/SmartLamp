#include "constants.h"

#include <Arduino.h>
#include <FastLED.h>
#include <GyverButton.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "effectManager.h"
#include "batteryManager.h"
#include "websocket.h"

CRGB leds[NUM_LEDS];
EffectManager effects(leds);
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
AsyncWebServer server(80);

enum class LampState : uint8_t { OFF, ON };
LampState currentState = LampState::ON;

void setupWeb() {
    server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html").setCacheControl("max-age=600");

    server.on("/api/batinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
        float batV    = ReadBatV();
        InaResult ina = ReadIna();

        JsonDocument doc;
        doc["voltage_V"]    = serialized(String(batV, 3));
        doc["soc_pct"]      = voltage2Percent(batV, ina.current_mA);
        doc["current_mA"]   = serialized(String(ina.current_mA, 2));
        doc["power_mW"]     = serialized(String(ina.power_mW, 2));
        doc["bus_V"]        = serialized(String(ina.busvoltage, 3));
        doc["shunt_mV"]     = serialized(String(ina.shuntvoltage, 3));

        String json;
        serializeJson(doc, json);
        request->send(200, "application/json", json);
    });

    server.on("/api/effectsList", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();

        for (uint8_t i = 0; i < effects.getEffectCount(); i++) {
            const EffectDescriptor* desc = effects.getEffectDescriptor(i);
            JsonObject obj = arr.add<JsonObject>();
            obj["id"]   = i;
            obj["name"] = desc->name;
        }

        String json;
        serializeJson(doc, json);
        request->send(200, "application/json", json);
    });

    initWebSocket(server, &effects);

    server.begin();
}

void setup() {
    Serial.begin(115200);
    delay(300);

    InitIna219();

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
    if (touch.isSingle()) { 
        if (currentState == LampState::OFF) {
            currentState = LampState::ON;
        } else {
            currentState = LampState::OFF;
            FastLED.clear();
            FastLED.show();
        }
    }
    
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