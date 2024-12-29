#include "SpO2.h"
#include <math.h>

SpO2Calculator::SpO2Calculator()
    : bufferIndex(0), bufferFull(false),
      redFiltered(0), irFiltered(0),
      spo2BufferIndex(0), spo2BufferFull(false)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        redBuffer[i] = 0;
        irBuffer[i] = 0;
    }
    for (int i = 0; i < SPO2_AVG_SIZE; i++)
    {
        spo2Buffer[i] = 0.0;
    }
}

void SpO2Calculator::addSample(long red, long ir)
{
    // 添加樣本到緩衝區
    redBuffer[bufferIndex] = red;
    irBuffer[bufferIndex] = ir;
    bufferIndex++;
    if (bufferIndex >= BUFFER_SIZE)
    {
        bufferIndex = 0;
        bufferFull = true;
    }

    // 應用低通濾波器
    redFiltered = ALPHA_FILTER * redFiltered + (1.0 - ALPHA_FILTER) * red;
    irFiltered = ALPHA_FILTER * irFiltered + (1.0 - ALPHA_FILTER) * ir;
}

bool SpO2Calculator::isReady()
{
    return bufferFull;
}

float SpO2Calculator::calculateSpO2()
{
    if (!isReady())
        return -1.0;

    // 計算 DC 和 AC 組件
    float dcRed = calculateDC(redBuffer, BUFFER_SIZE);
    float dcIr = calculateDC(irBuffer, BUFFER_SIZE);
    float acRed = calculateAC(redBuffer, BUFFER_SIZE);
    float acIr = calculateAC(irBuffer, BUFFER_SIZE);

    if (dcRed == 0 || dcIr == 0)
        return -1.0;

    // 計算比例 R
    float R = (acRed / dcRed) / (acIr / dcIr);

    // 使用校準公式將 R 轉換為 SpO2
    // 這個公式需要根據實驗數據進行校準
    // 這裡使用一個假設的非線性公式
    float spO2 = 110.0 - 13.5 * R; // 需要根據實際數據調整

    // 限制 SpO2 值在合理範圍內
    spO2 = constrain(spO2, 70.0, 100.0);

    // 添加到移動平均緩衝區
    spo2Buffer[spo2BufferIndex] = spO2;
    spo2BufferIndex++;
    if (spo2BufferIndex >= SPO2_AVG_SIZE)
    {
        spo2BufferIndex = 0;
        spo2BufferFull = true;
    }

    // 返回移動平均的 SpO2
    return calculateAverageSpO2();
}

float SpO2Calculator::calculateDC(const long *buffer, int size)
{
    float sum = 0.0;
    for (int i = 0; i < size; i++)
    {
        sum += buffer[i];
    }
    return sum / size;
}

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
