#pragma once

#include <ESPAsyncWebServer.h>

class EffectManager;

extern AsyncWebSocket ws;

void initWebSocket(AsyncWebServer& server, EffectManager* manager);
void sendState(AsyncWebSocketClient* client);
void broadcastState();