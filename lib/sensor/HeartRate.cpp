#include "HeartRate.h"
#include <Arduino.h>
#include <cmath>

// Initialize variables and buffers for heart rate calculation
HeartRateCalculator::HeartRateCalculator()
    : bufferIndex(0), bufferFull(false), lastPeakTime(0),
      intervalIndex(0), threshold(50000), isRising(false),
      movingAverage(0), alpha(0.95), signalQuality(0),
      baselineValue(0), noiseLevel(0)
{
  // Clear IR and filtered buffers
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    irBuffer[i] = 0;
    filteredBuffer[i] = 0;
  }
  // Clear peak buffers
  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    peakIntervals[i] = 0;
    peakValues[i] = 0;
  }
}

// Process new IR sensor data
void HeartRateCalculator::addSample(long ir)
{
  irBuffer[bufferIndex] = ir;

  // Filter and store signal
  long filtered = filterSample(ir);
  filteredBuffer[bufferIndex] = filtered;

  // Update moving average
  movingAverage = alpha * movingAverage + (1 - alpha) * filtered;

  if (detectPeak(filtered))
  {
    unsigned long currentTime = millis();
    if (lastPeakTime != 0)
    {
      unsigned long interval = currentTime - lastPeakTime;
      // Check if interval is within valid range (500-1700ms)
      if (interval >= 500 && interval <= 1700)
      {
        peakIntervals[intervalIndex] = interval;
        peakValues[intervalIndex] = filtered;
        intervalIndex = (intervalIndex + 1) % PEAK_BUFFER_SIZE;

        updateThreshold();
        calculateSignalQuality();
      }
    }
    lastPeakTime = currentTime;
  }

  // Update buffer index
  bufferIndex++;
  if (bufferIndex >= BUFFER_SIZE)
  {
    bufferIndex = 0;
    bufferFull = true;
  }
}

// Apply simple filter to smooth signal
long HeartRateCalculator::filterSample(long sample)
{
  static const float b[] = {0.1f, 0.2f, 0.4f, 0.2f, 0.1f};
  static const int ORDER = 5;
  static long x[5] = {0};

  // Shift samples and add new one
  for (int i = ORDER - 1; i > 0; i--)
  {
    x[i] = x[i - 1];
  }
  x[0] = sample;

  // Apply filter
  long filtered = 0;
  for (int i = 0; i < ORDER; i++)
  {
    filtered += b[i] * x[i];
  }

  return filtered;
}

// Detect peaks in filtered signal
bool HeartRateCalculator::detectPeak(long filtered)
{
  static long lastFiltered = 0;
  bool peak = false;

  if (filtered > threshold)
  {
    if (filtered > lastFiltered)
    {
      isRising = true;
    }
    else if (isRising && filtered < lastFiltered)
    {
      peak = true;
      isRising = false;
    }
  }
  else
  {
    isRising = false;
  }

  lastFiltered = filtered;
  return peak;
}

// Update threshold based on average peak values
void HeartRateCalculator::updateThreshold()
{
  float peakAvg = 0;
  int count = 0;

  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    if (peakValues[i] > 0)
    {
      peakAvg += peakValues[i];
      count++;
    }
  }

  if (count > 0)
  {
    peakAvg /= count;
    threshold = peakAvg * 0.7;
  }
}

// Calculate signal quality based on peak and interval variance
void HeartRateCalculator::calculateSignalQuality()
{
  float peakVariance = 0;
  float intervalVariance = 0;
  int count = 0;

  float peakMean = 0;
  float intervalMean = calculateAverageInterval();

  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    if (peakValues[i] > 0 && peakIntervals[i] > 0)
    {
      peakMean += peakValues[i];
      count++;
    }
  }

  if (count > 0)
  {
    peakMean /= count;

    for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
    {
      if (peakValues[i] > 0 && peakIntervals[i] > 0)
      {
        peakVariance += pow(peakValues[i] - peakMean, 2);
        intervalVariance += pow(peakIntervals[i] - intervalMean, 2);
      }
    }

    peakVariance = sqrt(peakVariance / count) / peakMean;
    intervalVariance = sqrt(intervalVariance / count) / intervalMean;

    signalQuality = 1.0 - (peakVariance * 0.5 + intervalVariance * 0.5);
    if (signalQuality < 0)
      signalQuality = 0;
  }
}

// Calculate average interval between peaks
float HeartRateCalculator::calculateAverageInterval()
{
  float sum = 0;
  int count = 0;

  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    if (peakIntervals[i] > 0)
    {
      sum += peakIntervals[i];
      count++;
    }
  }

  return (count > 0) ? sum / count : 0;
}

// Get current signal quality
float HeartRateCalculator::getSignalQuality()
{
  return signalQuality;
}

// Check if enough data for heart rate calculation
bool HeartRateCalculator::isReady()
{
  int validIntervals = 0;
  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    if (peakIntervals[i] > 0)
      validIntervals++;
  }
  return validIntervals >= 3;
}

// Calculate heart rate from peak intervals
float HeartRateCalculator::calculateHeartRate()
{
  if (!isReady() || signalQuality < 0.5)
  {
    return -1.0;
  }

  float averageInterval = calculateAverageInterval();
  if (averageInterval == 0)
    return -1.0;

  float bpm = 60000.0 / averageInterval;

  // Check if BPM is in valid range
  if (bpm < 40.0 || bpm > 180.0)
    return -1.0;

  return bpm;
}