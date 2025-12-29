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

    std::vector<HANDLE> hPipesIn(N);  
    std::vector<HANDLE> hPipesOut(N); 
    std::vector<PROCESS_INFORMATION> pi(N);

    for (int i = 0; i < N; ++i)
    {
        std::wstring pipeInName = L"\\\\.\\pipe\\worker_in_" + std::to_wstring(i);
        std::wstring pipeOutName = L"\\\\.\\pipe\\worker_out_" + std::to_wstring(i);
        hPipesIn[i] = CreateNamedPipeW(pipeInName.c_str(), PIPE_ACCESS_OUTBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);
        hPipesOut[i] = CreateNamedPipeW(pipeOutName.c_str(), PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 1024, 1024, 0, NULL);

        STARTUPINFOW si = { sizeof(si) };
        std::wstring cmd = L"Worker.exe " + std::to_wstring(i);
        CreateProcessW(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi[i]);
        ConnectNamedPipe(hPipesIn[i], NULL);
        ConnectNamedPipe(hPipesOut[i], NULL);

    }

    for (int j = 0; j < M; ++j)
    {
        int workerIdx = j % N;
        Task task;
        task.type = TASK_COMPUTE;
        task.size = 10;
        for (int k = 0; k < 10; ++k) task.data[k] = (double)(j + k);

        DWORD written, read;
        WriteFile(hPipesIn[workerIdx], &task, sizeof(Task), &written, NULL);

        Result res;
        ReadFile(hPipesOut[workerIdx], &res, sizeof(Result), &read, NULL);
        std::cout << "[Browser] Task " << j << " processed by Worker " << workerIdx << "\n";
    }

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