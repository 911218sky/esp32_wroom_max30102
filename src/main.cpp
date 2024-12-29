#include <Arduino.h>
#include "APManager.h"
#include "WebServerManager.h"
#include "WebSocketManager.h"
#include "SensorManager.h"

void setup()
{
    Serial.begin(9600);
    Serial.println("Initializing...");

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // List files in SPIFFS
    listFiles();

    // Setup Access Point
    setupAP();

    // Setup Web Server
    setupServer();

    // Setup WebSocket
    setupWebSocket();

    // Initialize Sensor
    if (!initializeSensor())
    {
        Serial.println("Sensor initialization failed!");
        while (1)
        {
            delay(100);
        }
    }

    Serial.println("Sensor warming up...");
}

void loop()
{
    // Handle Web Server Requests
    handleServer();

    // Handle WebSocket
    handleWebSocket();

    // Read and send sensor data
    processSensorData();
}
