#include "WebSocketManager.h"

WebSocketsServer webSocket(81); // WebSocket server on port 81

// Initialize WebSocket server
void setupWebSocket() {
   webSocket.begin();
   webSocket.onEvent(webSocketEvent);
}

// Handle WebSocket events and messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
   switch (type) {
   case WStype_DISCONNECTED:
       Serial.printf("[%u] Disconnected!\n", num);
       break;
   case WStype_CONNECTED:
       Serial.printf("[%u] Connected!\n", num);
       break;
   case WStype_TEXT:
       // Handle incoming text messages
       break;
   case WStype_BIN:
       // Handle incoming binary messages 
       break;
   }
}

// Process WebSocket events
void handleWebSocket() {
   webSocket.loop();
}