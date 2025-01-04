#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <WebSocketsServer.h>

// Global WebSocket server instance
extern WebSocketsServer webSocket;

// Initialize WebSocket server
void setupWebSocket();

// Process WebSocket events
void handleWebSocket();

// Handle WebSocket connections and messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

#endif