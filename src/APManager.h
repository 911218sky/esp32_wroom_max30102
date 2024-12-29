#ifndef APMANAGER_H
#define APMANAGER_H

#include <WiFi.h>
#include <esp_wifi.h>

// AP Configuration constants
extern const char *ssid;
extern const char *password;
extern const int CHANNEL;
extern const int MAX_CONNECTIONS;
extern const bool HIDE_SSID;
extern const int BEACON_INTERVAL;
extern const int WIFI_RETRY_COUNT;

// Function declarations
void setupAP();
void maintainAP();
void onStationConnected(arduino_event_id_t event, arduino_event_info_t info);
void onStationDisconnected(arduino_event_id_t event, arduino_event_info_t info);
void checkStationStatus();

#endif