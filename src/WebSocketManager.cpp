#include "WebSocketManager.h"

WebSocketsServer webSocket(81);

void setupWebSocket() {
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;
    case WStype_CONNECTED:
        Serial.printf("[%u] Connected!\n", num);
        break;
    case WStype_TEXT:
        // Handle text messages if needed
        break;
    case WStype_BIN:
        // Handle binary messages if needed
        break;
    }
}

void handleWebSocket() {
    webSocket.loop();
}
