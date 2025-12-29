#include <iostream>
#include <string>
#include "Common.h"
#include "MathUtils.h"

int main(int argc, char* argv[])
{
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
        Result res = CalculateDFT(task);

        std::cout << "[Worker " << id << "] Processing task..." << std::endl;
        Sleep(1000);
        WriteFile(hPipeOut, &res, sizeof(Result), &written, NULL);
    }

    CloseHandle(hPipeIn);
    CloseHandle(hPipeOut);
    return 0;
}