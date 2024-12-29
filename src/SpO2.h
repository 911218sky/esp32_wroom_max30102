#ifndef SPO2_H
#define SPO2_H

#include <Arduino.h>

class SpO2Calculator {
public:
    SpO2Calculator();
    
    // 添加紅光和紅外光樣本
    void addSample(long red,    // 紅光值 - 用於測量血液中的含氧血紅蛋白
                  long ir);     // 紅外光值 - 用於測量血液中的脫氧血紅蛋白
    
    // 檢查是否準備好計算 SpO2
    bool isReady();
    
    // 計算 SpO2 值
    float calculateSpO2();
    
private:
    static const int BUFFER_SIZE = 100; // 緩衝區大小 - 用於存儲最近的N個樣本進行分析
    long redBuffer[BUFFER_SIZE];        // 紅光值緩衝區 - 存儲紅光讀數歷史數據
    long irBuffer[BUFFER_SIZE];         // 紅外光值緩衝區 - 存儲紅外光讀數歷史數據
    int bufferIndex;                    // 緩衝區當前索引 - 追踪下一個要寫入的位置
    bool bufferFull;                    // 緩衝區是否已滿 - 表示是否已收集足夠的數據開始計算
    
    // 移動平均濾波器參數
    float redFiltered;                  // 紅光濾波後的值 - 用於減少信號噪聲
    float irFiltered;                   // 紅外光濾波後的值 - 用於減少信號噪聲
    const float ALPHA_FILTER = 0.95;    // 濾波器係數 - 決定新舊數據的權重比例
    
    // 計算 DC 和 AC 組件
    float calculateDC(const long* buffer,    // 輸入信號緩衝區 - 用於計算信號的平均值(DC成分)
                     int size);              // 緩衝區大小 - 決定計算平均值時使用的樣本數
    float calculateAC(const long* buffer,    // 輸入信號緩衝區 - 用於計算信號的脈動成分(AC成分)
                     int size);              // 緩衝區大小 - 決定計算脈動成分時使用的樣本數
    
    // 移動平均 SpO2
    static const int SPO2_AVG_SIZE = 10;     // SpO2平均值緩衝區大小 - 用於平滑SpO2讀數
    float spo2Buffer[SPO2_AVG_SIZE];         // SpO2值緩衝區 - 存儲最近的SpO2計算結果
    int spo2BufferIndex;                     // SpO2緩衝區當前索引 - 追踪下一個要寫入的SpO2值位置
    bool spo2BufferFull;                     // SpO2緩衝區是否已滿 - 表示是否已有足夠的SpO2值進行平均
    
    float calculateAverageSpO2();            // 計算SpO2平均值 - 返回多個SpO2讀數的平均值以提高準確性
};

#endif // SPO2_H