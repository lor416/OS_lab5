#include <iostream>
#include <cmath>
#include <string>
#include "Common.h"

int main(int argc, char* argv[])
{
    const double PI = 3.14;
    if (argc < 2) return 1;
    int id = std::stoi(argv[1]);

    std::wstring pipeInName = L"\\\\.\\pipe\\worker_in_" + std::to_wstring(id);
    std::wstring pipeOutName = L"\\\\.\\pipe\\worker_out_" + std::to_wstring(id);

    HANDLE hPipeIn = CreateFileW(pipeInName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE hPipeOut = CreateFileW(pipeOutName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipeIn == INVALID_HANDLE_VALUE || hPipeOut == INVALID_HANDLE_VALUE) return 1;

    while (true)
    {
        Task task;
        DWORD read, written;

        if (!ReadFile(hPipeIn, &task, sizeof(Task), &read, NULL) || read == 0) break;
        if (task.type == TASK_EXIT) break;

        // Преобразование Фурье
        Result res;
        res.size = task.size;
        for (int k = 0; k < res.size; ++k)
        {
            res.real[k] = 0;
            res.imag[k] = 0;
            for (int n = 0; n < res.size; ++n)
            {
                double angle = 2 * PI * k * n / res.size;
                res.real[k] += task.data[n] * cos(angle);
                res.imag[k] -= task.data[n] * sin(angle);
            }
        }
        std::cout << "[Worker " << id << "] Processing task..." << std::endl;
        Sleep(1000); 

        WriteFile(hPipeOut, &res, sizeof(Result), &written, NULL);
    }

    CloseHandle(hPipeIn);
    CloseHandle(hPipeOut);
    return 0;
}