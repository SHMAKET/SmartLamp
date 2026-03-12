#include "websocket.h"
#include "effectManager.h"
#include <ArduinoJson.h>

AsyncWebSocket ws("/ws");

static EffectManager* effectManager = nullptr;

void sendState(AsyncWebSocketClient* client)
{
    EffectParams& p = effectManager->getParams();

    JsonDocument doc;

    doc["b"] = p.brightness;
    doc["s"] = p.speed;
    doc["k"] = p.k_factor;

    String json;
    serializeJson(doc, json);

    client->text(json);
}

void broadcastState()
{
    EffectParams& p = effectManager->getParams();

    JsonDocument doc;

    doc["b"] = p.brightness;
    doc["s"] = p.speed;
    doc["k"] = p.k_factor;

    String json;
    serializeJson(doc, json);

    ws.textAll(json);
}

static void handleMessage(void* arg, uint8_t* data, size_t len)
{
    AwsFrameInfo* info = (AwsFrameInfo*)arg;

    if (!(info->final && info->index == 0 && info->len == len))
        return;

    data[len] = 0;

    JsonDocument doc;

    if (deserializeJson(doc, (char*)data))
        return;

    if (doc["b"])
        effectManager->setBrightness(doc["b"]);

    if (doc["s"])
        effectManager->setSpeed(doc["s"]);

    if (doc["k"])
        effectManager->setKFactor(doc["k"]);

    broadcastState();
}

static void onEvent(AsyncWebSocket* server,
                    AsyncWebSocketClient* client,
                    AwsEventType type,
                    void* arg,
                    uint8_t* data,
                    size_t len)
{
    switch (type)
    {

        case WS_EVT_CONNECT:
            sendState(client);
            break;

        case WS_EVT_DATA:
            handleMessage(arg, data, len);
            break;

        default:
            break;
    }
}

void initWebSocket(AsyncWebServer& server, EffectManager* manager)
{
    effectManager = manager;

    ws.onEvent(onEvent);
    server.addHandler(&ws);
}