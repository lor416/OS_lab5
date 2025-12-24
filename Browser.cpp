#include <iostream>
#include <vector>
#include <string>
#include "Common.h"

int main()
{
    int N, M;
    std::cout << "Enter number of workers (N): ";
    std::cin >> N;
    std::cout << "Enter total number of tasks (M): ";
    std::cin >> M;

    std::vector<HANDLE> hPipesIn(N);  // Каналы для отправки задач
    std::vector<HANDLE> hPipesOut(N); // Каналы для получения ответов
    std::vector<PROCESS_INFORMATION> pi(N);

    // 1. Создание каналов и запуск процессов
    for (int i = 0; i < N; ++i)
    {
        std::wstring pipeInName = L"\\\\.\\pipe\\worker_in_" + std::to_wstring(i);
        std::wstring pipeOutName = L"\\\\.\\pipe\\worker_out_" + std::to_wstring(i);

        // Создаем именованные каналы
        hPipesIn[i] = CreateNamedPipeW(pipeInName.c_str(), PIPE_ACCESS_OUTBOUND, 
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);

        hPipesOut[i] = CreateNamedPipeW(pipeOutName.c_str(), PIPE_ACCESS_INBOUND, 
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);

        // Запуск Worker.exe с передачей ID
        STARTUPINFOW si = { sizeof(si) };
        std::wstring cmd = L"Worker.exe " + std::to_wstring(i);
        CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi[i]);

        // Ожидаем подключения Worker-а к каналам
        ConnectNamedPipe(hPipesIn[i], NULL);
        ConnectNamedPipe(hPipesOut[i], NULL);
    }

    // 2. Распределение задач (Простой Round-robin)
    for (int j = 0; j < M; ++j)
    {
        int workerIdx = j % N;
        Task task;
        task.type = TASK_COMPUTE;
        task.size = 10;
        for (int k = 0; k < 10; ++k) task.data[k] = (double)(j + k);

        DWORD written, read;
        // Отправка задачи
        WriteFile(hPipesIn[workerIdx], &task, sizeof(Task), &written, NULL);
        
        // Ожидание результата
        Result res;
        ReadFile(hPipesOut[workerIdx], &res, sizeof(Result), &read, NULL);

        std::cout << "[Browser] Task " << j << " processed by Worker " << workerIdx << "\n";
    }

    // 3. Завершение работы
    for (int i = 0; i < N; ++i)
    {
        Task exitTask;
        exitTask.type = TASK_EXIT;
        DWORD written;
        WriteFile(hPipesIn[i], &exitTask, sizeof(Task), &written, NULL);

        WaitForSingleObject(pi[i].hProcess, INFINITE);
        CloseHandle(hPipesIn[i]);
        CloseHandle(hPipesOut[i]);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    std::cout << "All tasks finished. Press Enter to exit.";
    std::cin.ignore();
    std::cin.get();
    return 0;
}