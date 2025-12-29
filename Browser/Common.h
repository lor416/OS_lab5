#pragma once
#include <windows.h>
enum TaskType
{
    TASK_COMPUTE, // Вычислить преобразование Фурье
    TASK_EXIT     // Завершить работу
};

struct Task
{
    TaskType type;
    double data[10];
    int size;
};
struct Result
{
    double real[10];
    double imag[10];
    int size;
};