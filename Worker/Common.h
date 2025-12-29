#pragma once
#include <windows.h>

enum TaskType
{
    TASK_COMPUTE, 
    TASK_EXIT     
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