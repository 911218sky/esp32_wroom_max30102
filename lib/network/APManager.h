#ifndef APMANAGER_H
#define APMANAGER_H

#include <WiFi.h>
#include <esp_wifi.h>

// WiFi Access Point Settings
extern const char *ssid;           // AP name
extern const char *password;       // AP password
extern const int CHANNEL;          // WiFi channel
extern const int MAX_CONNECTIONS;  // Max clients
extern const bool HIDE_SSID;       // Hide AP in scan
extern const int BEACON_INTERVAL;  // Beacon timing
extern const int WIFI_RETRY_COUNT; // Connection retries

// Core Functions
void setupAP();                    // Start AP
void maintainAP();                // AP maintenance
void onStationConnected           // New client handler
    (arduino_event_id_t event, arduino_event_info_t info);
void onStationDisconnected        // Client disconnect handler
    (arduino_event_id_t event, arduino_event_info_t info);
void checkStationStatus();        // Monitor clients

#endif