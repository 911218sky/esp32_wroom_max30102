#include "SpO2.h"
#include <math.h>

// Initialize SpO2 calculator buffers and variables 
SpO2Calculator::SpO2Calculator()
   : bufferIndex(0), bufferFull(false),
     redFiltered(0), irFiltered(0), 
     spo2BufferIndex(0), spo2BufferFull(false)
{
   // Clear raw signal buffers
   for (int i = 0; i < BUFFER_SIZE; i++)
   {
       redBuffer[i] = 0;
       irBuffer[i] = 0;
   }
   // Clear SpO2 averaging buffer
   for (int i = 0; i < SPO2_AVG_SIZE; i++)
   {
       spo2Buffer[i] = 0.0;
   }
}

// Add new red and IR samples to buffers
void SpO2Calculator::addSample(long red, long ir)
{
   // Add samples to buffer
   redBuffer[bufferIndex] = red;
   irBuffer[bufferIndex] = ir;
   bufferIndex++;
   if (bufferIndex >= BUFFER_SIZE)
   {
       bufferIndex = 0;
       bufferFull = true;
   }

   // Apply low-pass filter
   redFiltered = ALPHA_FILTER * redFiltered + (1.0 - ALPHA_FILTER) * red;
   irFiltered = ALPHA_FILTER * irFiltered + (1.0 - ALPHA_FILTER) * ir;
}

// Check if enough samples collected
bool SpO2Calculator::isReady()
{
   return bufferFull;
}

// Calculate SpO2 percentage from buffered data
float SpO2Calculator::calculateSpO2()
{
   if (!isReady())
       return -1.0;

   // Calculate DC and AC components
   float dcRed = calculateDC(redBuffer, BUFFER_SIZE);
   float dcIr = calculateDC(irBuffer, BUFFER_SIZE);
   float acRed = calculateAC(redBuffer, BUFFER_SIZE);
   float acIr = calculateAC(irBuffer, BUFFER_SIZE);

   if (dcRed == 0 || dcIr == 0)
       return -1.0;

   // Calculate R ratio
   float R = (acRed / dcRed) / (acIr / dcIr);

   // Convert R to SpO2 using calibration formula
   float spO2 = 110.0 - 13.5 * R; // Calibration needed

   // Limit SpO2 to valid range
   spO2 = constrain(spO2, 70.0, 100.0);

   // Add to averaging buffer
   spo2Buffer[spo2BufferIndex] = spO2;
   spo2BufferIndex++;
   if (spo2BufferIndex >= SPO2_AVG_SIZE)
   {
       spo2BufferIndex = 0;
       spo2BufferFull = true;
   }

   return calculateAverageSpO2();
}

// Calculate DC component (average)
float SpO2Calculator::calculateDC(const long *buffer, int size)
{
   float sum = 0.0;
   for (int i = 0; i < size; i++)
   {
       sum += buffer[i];
   }
   return sum / size;
}

// Calculate AC component (peak-to-peak/2)
float SpO2Calculator::calculateAC(const long *buffer, int size)
{
   long maxVal = buffer[0];
   long minVal = buffer[0];
   for (int i = 1; i < size; i++)
   {
       if (buffer[i] > maxVal)
           maxVal = buffer[i];
       if (buffer[i] < minVal)
           minVal = buffer[i];
   }
   float ac = (maxVal - minVal) / 2.0;
   return ac;
}

// Calculate moving average of SpO2 values
float SpO2Calculator::calculateAverageSpO2()
{
   float sum = 0.0;
   int count = spo2BufferFull ? SPO2_AVG_SIZE : spo2BufferIndex;
   for (int i = 0; i < count; i++)
   {
       sum += spo2Buffer[i];
   }
   if (count == 0)
       return -1.0;
   return sum / count;
}