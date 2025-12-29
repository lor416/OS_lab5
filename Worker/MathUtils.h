#pragma once
#include "Common.h"
#include <cmath>

// Общая логика расчета ДПФ
inline Result CalculateDFT(const Task& task)
{
    const double PI = 3.141592653589793;
    Result res;
    res.size = task.size;

    // Если size = 0, циклы не выполнятся, что обеспечивает безопасность
    for (int k = 0; k < res.size; ++k)
    {
        res.real[k] = 0;
        res.imag[k] = 0;
        for (int n = 0; n < res.size; ++n)
        {
            double angle = 2.0 * PI * k * n / res.size;
            res.real[k] += task.data[n] * cos(angle);
            res.imag[k] -= task.data[n] * sin(angle);
        }
    }
    return res;
}