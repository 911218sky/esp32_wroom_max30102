#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Wire.h>
#include <MAX30105.h>
#include <ArduinoJson.h>
#include "SpO2.h"
#include "HeartRate.h"
#include "../network/WebSocketManager.h"

// Initialize MAX30102 sensor hardware
bool initializeSensor();

// Process raw sensor readings and calculate vitals 
void processSensorData();

// Send processed data via WebSocket
void sendSensorData();

#endif