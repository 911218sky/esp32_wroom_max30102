#ifndef HEARTRATE_H
#define HEARTRATE_H

#include <Arduino.h>

// HeartRateCalculator: Process IR sensor data to detect heart rate
class HeartRateCalculator
{
public:
  // Initialize heart rate calculator
  HeartRateCalculator();

  // Process new IR sensor reading
  void addSample(long ir);

  // Check if enough peaks detected for calculation
  bool isReady();

  // Get current heart rate in BPM
  float calculateHeartRate();

  // Get signal quality (0-1)
  float getSignalQuality();

private:
  // Buffer sizes
  static const int BUFFER_SIZE = 100;      // Raw signal buffer
  static const int PEAK_BUFFER_SIZE = 10;  // Peak detection buffer
  
  // Signal buffers
  long irBuffer[BUFFER_SIZE];        // Raw IR readings
  long filteredBuffer[BUFFER_SIZE];  // Filtered signal
  int bufferIndex;                  // Current buffer position
  bool bufferFull;                  // Buffer fill status

  // Peak detection variables
  unsigned long lastPeakTime;                 // Last peak timestamp
  unsigned long peakIntervals[PEAK_BUFFER_SIZE];  // Time between peaks
  int intervalIndex;                          // Current peak buffer position
  long peakValues[PEAK_BUFFER_SIZE];          // Peak amplitudes
  float threshold;                           // Peak detection threshold
  bool isRising;                            // Signal rising flag

  // Signal processing variables
  float movingAverage;  // Smoothed signal
  float alpha;          // Smoothing factor
  float signalQuality;  // Quality metric
  long baselineValue;   // Signal baseline
  float noiseLevel;     // Noise estimate

  // Signal processing functions
  bool detectPeak(long currentIR);   // Find peaks
  long filterSample(long sample);    // Filter signal
  void updateThreshold();           // Adjust peak threshold
  void calculateSignalQuality();    // Update quality metric
  float calculateAverageInterval(); // Get average peak interval
};

#endif