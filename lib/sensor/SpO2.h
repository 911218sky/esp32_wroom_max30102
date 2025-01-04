#ifndef SPO2_H
#define SPO2_H

#include <Arduino.h>

class SpO2Calculator {
public:
   SpO2Calculator();
   
   // Add red and infrared light samples
   void addSample(long red,    // Red light - measures oxygenated hemoglobin
                 long ir);     // IR light - measures deoxygenated hemoglobin
   
   // Check if ready to calculate SpO2
   bool isReady();
   
   // Calculate SpO2 value
   float calculateSpO2();
   
private:
   static const int BUFFER_SIZE = 100; // Size for raw sample storage
   long redBuffer[BUFFER_SIZE];        // Red light history
   long irBuffer[BUFFER_SIZE];         // IR light history
   int bufferIndex;                    // Current buffer position
   bool bufferFull;                    // Buffer has enough data
   
   // Moving average filter
   float redFiltered;                  // Filtered red value
   float irFiltered;                   // Filtered IR value
   const float ALPHA_FILTER = 0.95;    // Filter coefficient
   
   // Signal component calculations 
   float calculateDC(const long* buffer,    // Calculate signal average
                    int size);    
   float calculateAC(const long* buffer,    // Calculate pulsatile component
                    int size);              
   
   // SpO2 averaging
   static const int SPO2_AVG_SIZE = 10;     // Size of SpO2 averaging buffer
   float spo2Buffer[SPO2_AVG_SIZE];         // Recent SpO2 values
   int spo2BufferIndex;                     // Current SpO2 buffer position
   bool spo2BufferFull;                     // SpO2 buffer has enough data
   
   float calculateAverageSpO2();            // Calculate averaged SpO2
};

#endif // SPO2_H