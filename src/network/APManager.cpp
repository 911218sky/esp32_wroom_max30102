#include "APManager.h"

// AP Configuration
const char *ssid = "ESP32-AP";
const char *password = "12345678";
const int CHANNEL = 11;           // Range: 1-13
const int MAX_CONNECTIONS = 4;   // Max clients
const bool HIDE_SSID = false;    // Hide network
const int BEACON_INTERVAL = 100; // Beacon interval (ms)
const int WIFI_RETRY_COUNT = 3;  // Setup retries

// Monitoring settings
static unsigned long lastCheckTime = 0;
const unsigned long CHECK_INTERVAL = 10000; // Check interval (ms)

// Handle new client connection
void onStationConnected(arduino_event_id_t event, arduino_event_info_t info)
{
    Serial.println("\n=== New device connected to AP! ===");
    Serial.print("MAC address: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%02X", info.wifi_sta_connected.bssid[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();
    Serial.print("Total devices connected: ");
    Serial.println(WiFi.softAPgetStationNum());
    Serial.println("================================");
}

// Handle client disconnection
void onStationDisconnected(arduino_event_id_t event, arduino_event_info_t info)
{
    Serial.println("\n=== Device disconnected from AP! ===");
    Serial.print("MAC address: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%02X", info.wifi_sta_disconnected.bssid[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();
    Serial.print("Total devices connected: ");
    Serial.println(WiFi.softAPgetStationNum());
    Serial.println("===================================");
}

// Display connected clients info
void checkStationStatus()
{
    wifi_sta_list_t stationList;
    tcpip_adapter_sta_list_t adapterList;

    esp_wifi_ap_get_sta_list(&stationList);
    tcpip_adapter_get_sta_list(&stationList, &adapterList);

    if (adapterList.num > 0)
    {
        Serial.println("\n=== Connected Stations Status ===");
        for (int i = 0; i < adapterList.num; i++)
        {
            Serial.printf("Station %d:\n", i + 1);
            Serial.printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                          adapterList.sta[i].mac[0], adapterList.sta[i].mac[1],
                          adapterList.sta[i].mac[2], adapterList.sta[i].mac[3],
                          adapterList.sta[i].mac[4], adapterList.sta[i].mac[5]);
            Serial.printf("  IP: %d.%d.%d.%d\n",
                          adapterList.sta[i].ip.addr & 0xff,
                          (adapterList.sta[i].ip.addr >> 8) & 0xff,
                          (adapterList.sta[i].ip.addr >> 16) & 0xff,
                          (adapterList.sta[i].ip.addr >> 24) & 0xff);
        }
        Serial.println("===============================");
    }
}

// Initialize WiFi Access Point
void setupAP()
{
    // Set AP mode and power settings
    WiFi.mode(WIFI_AP);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    esp_wifi_set_ps(WIFI_PS_NONE);

    // Start AP with retry mechanism
    int retry = 0;
    while (!WiFi.softAP(ssid, password, CHANNEL, HIDE_SSID, MAX_CONNECTIONS) && retry < WIFI_RETRY_COUNT)
    {
        Serial.println("AP Start Failed. Retrying...");
        retry++;
        delay(1000);
    }

    if (retry == WIFI_RETRY_COUNT)
    {
        Serial.println("AP Start Failed after retries. Restarting...");
        ESP.restart();
    }

    // Set event handlers
    WiFi.onEvent(onStationConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
    WiFi.onEvent(onStationDisconnected, ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

    // Show AP info
    Serial.println("\n=== AP Started Successfully ===");
    Serial.printf("SSID: %s\n", ssid);
    Serial.printf("Password: %s\n", password);
    Serial.printf("Channel: %d\n", CHANNEL);
    Serial.printf("Max Connections: %d\n", MAX_CONNECTIONS);
    Serial.printf("IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.println("============================");
}

// Periodic AP maintenance
void maintainAP()
{
    if (millis() - lastCheckTime >= CHECK_INTERVAL)
    {
        lastCheckTime = millis();

        // Check connected clients
        int stationCount = WiFi.softAPgetStationNum();
        if (stationCount == 0)
        {
            Serial.println("No stations connected");
        }
        else
        {
            Serial.printf("Number of connected stations: %d\n", stationCount);
            checkStationStatus();
        }

        // Verify AP status
        if (WiFi.getMode() != WIFI_AP)
        {
            Serial.println("AP mode lost! Restarting...");
            ESP.restart();
        }
    }
}