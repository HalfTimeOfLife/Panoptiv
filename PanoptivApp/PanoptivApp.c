#include <stdio.h>
#include <stdbool.h>
#include <io.h>
#include <tchar.h>
#include <Windows.h>

bool DllInjector(char* DllPath, char* ExecutablePath) {

    STARTUPINFOA startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };

    /* Create target process in suspended mode */
    if (!CreateProcessA(ExecutablePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,
        NULL, NULL, &startupInfo, &processInfo)) {
        printf("[-] Error while creating target process : %lu\n", GetLastError());
        return false;
    }

    /* Allocate Memory in the process for the DLL */
    LPVOID pDllPath =
        VirtualAllocEx(processInfo.hProcess, NULL, strlen(DllPath) + 1,
            MEM_COMMIT || MEM_RESERVE, PAGE_READWRITE);

    if (!pDllPath) {
        printf("[-] Error while allocating memory in the target process : "
            "%lu\n",
            GetLastError());
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return false;
    }

    /* Write DLL path in the process memory */
    if (!WriteProcessMemory(processInfo.hProcess, pDllPath, DllPath,
        strlen(DllPath) + 1, NULL)) {
        printf("[-] Error while writing dll path : %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return false;
    }

    return true;
}

int main() {
    printf("[+] Panoptiv application starting ...\n");

    /* Allocate buffers */
    char DllPath[512] = { 0 };
    char ExecutablePath[512] = { 0 };

    /* Get Dll Path */
    printf("[+] Enter the dll path : ");
    scanf_s("%511s", DllPath, (unsigned)_countof(DllPath));

    /* Get Executable Path */
    printf("[+] Enter the executable path : ");
    scanf_s("%511s", ExecutablePath, (unsigned)_countof(ExecutablePath));

    if (_access(DllPath, 0) == -1 || _access(ExecutablePath, 0) == -1) {
        printf("[-] Error: Dll or Executable path does not exist.\n");
        printf("[!] Dll path : %s\n", DllPath);
        printf("[!] Executable path : %s\n", ExecutablePath);
        printf("[!] Please check the paths and try again.\n");
        return EXIT_FAILURE;
    }

    return DllInjector(DllPath, ExecutablePath) ? EXIT_SUCCESS : EXIT_FAILURE;
}