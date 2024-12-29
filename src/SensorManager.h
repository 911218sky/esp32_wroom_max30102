#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Wire.h>
#include <MAX30105.h>
#include <ArduinoJson.h>
#include "SpO2.h"
#include "HeartRate.h"
#include "WebSocketManager.h"

bool initializeSensor();
void processSensorData();
void sendSensorData();

#endif
