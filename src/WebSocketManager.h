#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <WebSocketsServer.h>

extern WebSocketsServer webSocket;

// Functions
void setupWebSocket();
void handleWebSocket();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

#endif
