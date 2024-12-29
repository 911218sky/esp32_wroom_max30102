#include "SensorManager.h"

MAX30105 particleSensor;
SpO2Calculator spO2Calc;
HeartRateCalculator heartRateCalc;

float lastSpO2 = -1;
float lastBPM = -1;
const float ALPHA = 0.8;

bool initializeSensor() {
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 not found. Please check wiring/power.");
        return false;
    }

    particleSensor.setup(60, 4, 2, 800, 215, 16384);
    particleSensor.enableDIETEMPRDY();
    return true;
}

void processSensorData() {
    long irValue = particleSensor.getIR();
    long redValue = particleSensor.getRed();
    float temperature = particleSensor.readTemperature();

    if (irValue < 5000 || temperature < 25) {
        lastSpO2 = -1;
        lastBPM = -1;

        StaticJsonDocument<100> doc;
        doc["status"] = "no_finger";
        String jsonString;
        serializeJson(doc, jsonString);
        webSocket.broadcastTXT(jsonString);

        delay(100);
        return;
    }

    spO2Calc.addSample(redValue, irValue);
    heartRateCalc.addSample(irValue);

    sendSensorData();
}

void sendSensorData() {
    if (spO2Calc.isReady() && heartRateCalc.isReady()) {
        StaticJsonDocument<200> doc;

        float newSpO2 = spO2Calc.calculateSpO2();
        if (newSpO2 > 0) {
            lastSpO2 = (lastSpO2 < 0) ? newSpO2 : ALPHA * lastSpO2 + (1 - ALPHA) * newSpO2;
            doc["spO2"] = round(lastSpO2 * 100) / 100.0;
        }

        float newBPM = heartRateCalc.calculateHeartRate();
        if (newBPM > 0) {
            lastBPM = (lastBPM < 0) ? newBPM : ALPHA * lastBPM + (1 - ALPHA) * newBPM;
            doc["hr"] = round(lastBPM * 100) / 100.0;
            doc["hrQuality"] = round(heartRateCalc.getSignalQuality() * 100) / 100.0;
        }

        String jsonString;
        serializeJson(doc, jsonString);
        webSocket.broadcastTXT(jsonString);
    }
}
