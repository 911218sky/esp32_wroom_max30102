#include "HeartRate.h"
#include <Arduino.h>
#include <cmath>

HeartRateCalculator::HeartRateCalculator()
    : bufferIndex(0), bufferFull(false), lastPeakTime(0),
      intervalIndex(0), threshold(50000), isRising(false),
      movingAverage(0), alpha(0.95), signalQuality(0),
      baselineValue(0), noiseLevel(0)
{
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    irBuffer[i] = 0;
    filteredBuffer[i] = 0;
  }
  for (int i = 0; i < PEAK_BUFFER_SIZE; i++)
  {
    peakIntervals[i] = 0;
    peakValues[i] = 0;
  }
}

void HeartRateCalculator::addSample(long ir)
{
  irBuffer[bufferIndex] = ir;

  long filtered = filterSample(ir);
  filteredBuffer[bufferIndex] = filtered;

  movingAverage = alpha * movingAverage + (1 - alpha) * filtered;

  if (detectPeak(filtered))
  {
    unsigned long currentTime = millis();
    if (lastPeakTime != 0)
    {
      unsigned long interval = currentTime - lastPeakTime;
      // 調整間隔範圍，增加最小間隔以降低心率
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

  bufferIndex++;
  if (bufferIndex >= BUFFER_SIZE)
  {
    bufferIndex = 0;
    bufferFull = true;
  }
}

long HeartRateCalculator::filterSample(long sample)
{
  static const float b[] = {0.1f, 0.2f, 0.4f, 0.2f, 0.1f};
  static const int ORDER = 5;
  static long x[5] = {0};

  for (int i = ORDER - 1; i > 0; i--)
  {
    x[i] = x[i - 1];
  }
  x[0] = sample;

  long filtered = 0;
  for (int i = 0; i < ORDER; i++)
  {
    filtered += b[i] * x[i];
  }

  return filtered;
}

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

float HeartRateCalculator::getSignalQuality()
{
  return signalQuality;
}

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

  if (bpm < 40.0 || bpm > 180.0)
    return -1.0;

  return bpm;
}