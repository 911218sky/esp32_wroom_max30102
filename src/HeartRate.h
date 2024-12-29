#ifndef HEARTRATE_H
#define HEARTRATE_H

#include <Arduino.h>

class HeartRateCalculator
{
public:
  HeartRateCalculator();

  // 添加紅外光樣本
  void addSample(long ir); // 紅外光值 - 用於檢測血液脈動

  // 檢查是否準備好計算心率
  bool isReady(); // 返回是否已收集足夠數據可以計算心率

  // 計算心率值
  float calculateHeartRate(); // 返回每分鐘心跳次數(BPM)

  // 獲取信號質量
  float getSignalQuality(); // 返回0-1之間的值,表示信號可靠程度

private:
  static const int BUFFER_SIZE = 100;     // 主緩衝區大小 - 用於存儲原始信號
  static const int PEAK_BUFFER_SIZE = 10; // 峰值緩衝區大小 - 用於存儲檢測到的心跳峰值
  long irBuffer[BUFFER_SIZE];             // 紅外光原始數據緩衝區
  long filteredBuffer[BUFFER_SIZE];       // 濾波後的數據緩衝區
  int bufferIndex;                        // 緩衝區當前索引
  bool bufferFull;                        // 緩衝區是否已滿

  unsigned long lastPeakTime;                    // 上一次檢測到峰值的時間
  unsigned long peakIntervals[PEAK_BUFFER_SIZE]; // 峰值間隔時間緩衝區
  int intervalIndex;                             // 間隔緩衝區索引
  long peakValues[PEAK_BUFFER_SIZE];             // 峰值大小緩衝區
  float threshold;                               // 峰值檢測閾值
  bool isRising;                                 // 信號是否處於上升階段

  float movingAverage; // 移動平均值 - 用於平滑信號
  float alpha;         // 平滑係數
  float signalQuality; // 信號質量指標
  long baselineValue;  // 基線值 - 信號的DC分量
  float noiseLevel;    // 噪聲水平估計

  bool detectPeak(long currentIR);  // 檢測信號峰值
  long filterSample(long sample);   // 對樣本進行濾波處理
  void updateThreshold();           // 更新峰值檢測閾值
  void calculateSignalQuality();    // 計算信號質量
  float calculateAverageInterval(); // 計算平均心跳間隔
};

#endif