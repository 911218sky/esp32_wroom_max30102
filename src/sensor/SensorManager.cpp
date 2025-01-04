#include "SensorManager.h"

// Initialize sensor objects
MAX30105 particleSensor;
SpO2Calculator spO2Calc;
HeartRateCalculator heartRateCalc;

// Tracking variables for smoothing
float lastSpO2 = -1;
float lastBPM = -1;
const float ALPHA = 0.8;  // Smoothing factor

// Setup MAX30102 sensor
bool initializeSensor() {
    // Check sensor connection
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 not found. Please check wiring/power.");
        return false;
    }

    // Configure sensor settings
    particleSensor.setup(60, 4, 2, 800, 215, 16384);
    particleSensor.enableDIETEMPRDY();
    return true;
}

// Main sensor data processing loop
void processSensorData() {
    // Get raw sensor readings
    long irValue = particleSensor.getIR();
    long redValue = particleSensor.getRed();
    float temperature = particleSensor.readTemperature();

    // Check if finger is placed on sensor
    if (irValue < 5000 || temperature < 25) {
        lastSpO2 = -1;
        lastBPM = -1;

        // Send no-finger status
        StaticJsonDocument<100> doc;
        doc["status"] = "no_finger";
        String jsonString;
        serializeJson(doc, jsonString);
        webSocket.broadcastTXT(jsonString);

        delay(100);
        return;
    }

    // Process readings
    spO2Calc.addSample(redValue, irValue);
    heartRateCalc.addSample(irValue);

    sendSensorData();
}

// Send processed sensor data via WebSocket
void sendSensorData() {
    if (spO2Calc.isReady() && heartRateCalc.isReady()) {
        StaticJsonDocument<200> doc;

        // Calculate and smooth SpO2
        float newSpO2 = spO2Calc.calculateSpO2();
        if (newSpO2 > 0) {
            lastSpO2 = (lastSpO2 < 0) ? newSpO2 : ALPHA * lastSpO2 + (1 - ALPHA) * newSpO2;
            doc["spO2"] = round(lastSpO2 * 100) / 100.0;
        }

        // Calculate and smooth heart rate
        float newBPM = heartRateCalc.calculateHeartRate();
        if (newBPM > 0) {
            lastBPM = (lastBPM < 0) ? newBPM : ALPHA * lastBPM + (1 - ALPHA) * newBPM;
            doc["hr"] = round(lastBPM * 100) / 100.0;
            doc["hrQuality"] = round(heartRateCalc.getSignalQuality() * 100) / 100.0;
        }

        // Send JSON data
        String jsonString;
        serializeJson(doc, jsonString);
        webSocket.broadcastTXT(jsonString);
    }
}