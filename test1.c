#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

int main()
{
    HANDLE hProcess;
    DWORD processID;

    printf("Enter process ID:\n");
    scanf("%d", &processID);

    // Открытие процесса
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    if (hProcess == NULL) {
        printf("Could not open process\n");
        return 1;
    }

    // Получение имени процесса
    char processName[MAX_PATH];
    if (GetModuleBaseName(hProcess, NULL, processName, MAX_PATH) == 0) {
        printf("Could not get process name\n");
    } else {
        printf("Process name: %s\n", processName);
    }

    // Получение ID родительского процесса
    DWORD parentID = 0;
    HANDLE hParent = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hParent != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(pe32);
        if (Process32First(hParent, &pe32)) {
            do {
                if (pe32.th32ProcessID == processID) {
                    parentID = pe32.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hParent, &pe32));
        }
        CloseHandle(hParent);
    }
    if (parentID == 0) {
        printf("Could not get parent process ID\n");
    } else {
        printf("Parent process ID: %d\n", parentID);
    }

    // Получение пути к исполняемому файлу процесса
    char processPath[MAX_PATH];
    DWORD pathSize = GetModuleFileNameEx(hProcess, NULL, processPath, MAX_PATH);
    if (pathSize == 0) {
        printf("Could not get process path\n");
    } else {
        printf("Process path: %s\n", processPath);
    }

    // Получение использования памяти процессом
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        printf("Working Set Size: %u bytes\n", pmc.WorkingSetSize);
    } else {
        printf("Could not get memory info\n");
    }

    // Закрытие процесса
    CloseHandle(hProcess);

    return 0;
}
