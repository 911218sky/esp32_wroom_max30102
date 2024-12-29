#include "APManager.h"

const char *ssid = "ESP32-AP";
const char *password = "12345678";

void setupAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Serial.println("AP Started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
}
