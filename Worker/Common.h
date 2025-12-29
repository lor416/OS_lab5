#pragma once
#include <windows.h>

//#define MAX_ARRAY_SIZE 10

// Типы задач
enum TaskType
{
    TASK_COMPUTE, // Вычислить преобразование Фурье
    TASK_EXIT     // Завершить работу
};

// Структура задачи (от Browser к Worker)
struct Task
{
    TaskType type;
    double data[10];
    int size;
};

// Структура результата (от Worker к Browser)
struct Result
{
    double real[10];
    double imag[10];
    int size;
};