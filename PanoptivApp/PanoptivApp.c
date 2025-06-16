/**
 * @file    PanoptivApp.c
 * @author  HalfTimeOfLife
 * @brief   Application to inject a DLL into a newly launched process.
 *
 * This application creates a target process in suspended mode,
 * injects a specified DLL into it using remote thread execution,
 * and then resumes the process.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <io.h>
#include <tchar.h>
#include <Windows.h>

#include "Helper.h"


/**
 * @brief Injects a DLL into a target process.
 *
 * This function creates a new process in suspended mode, allocates memory
 * in the target process, writes the DLL path into it, and then creates a
 * remote thread to load the DLL using LoadLibraryA.
 *
 * @param DllPath         Path to the DLL to be injected (null-terminated string).
 * @param ExecutablePath  Path to the target executable (null-terminated string).
 *
 * @return true if the DLL was successfully injected, false otherwise.
 */

bool DllInjector(char* DllPath, char* ExecutablePath) {

    STARTUPINFOA startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };
    startupInfo.cb = sizeof(STARTUPINFOA);

    /* Create target process in suspended mode */
    if (!CreateProcessA(ExecutablePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,
        NULL, NULL, &startupInfo, &processInfo)) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while creating target process : %lu\n", GetLastError());
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Allocate Memory in the process for the DLL */
    LPVOID pDllPath =
        VirtualAllocEx(processInfo.hProcess, NULL, strlen(DllPath) + 1,
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDllPath) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while allocating memory in the target process : %lu\n", GetLastError());
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Write DLL path in the process memory */
    if (!WriteProcessMemory(processInfo.hProcess, pDllPath, DllPath,
        strlen(DllPath) + 1, NULL)) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while writing dll path : %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Get handle of kernel32.dll */
    HMODULE hKernel = GetModuleHandleA("kernel32.dll");
    if (!hKernel) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not get handle of kernel32.dll: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Get address of LoadLibraryA */
    LPTHREAD_START_ROUTINE pLoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "LoadLibraryA");
    if (!pLoadLibraryA) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not get address of LoadLibraryA: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Create remote thread to load the DLL */ 
    HANDLE hRemoteThread = CreateRemoteThread(processInfo.hProcess, NULL, 0, pLoadLibraryA, pDllPath, 0, NULL);
    if (!hRemoteThread) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not create remote thread: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Resume the main thread of the process */
    ResumeThread(processInfo.hThread);

    /* Wait for the remote thread to finish */
    WaitForSingleObject(hRemoteThread, INFINITE);

    /* Free the memory allocated for the DLL path */
    VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);

    /* Wait for the target process to finish */
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    /* Clean up */
    CloseHandle(hRemoteThread);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    return true;
}

/**
 * @file    PanoptivApp.c
 * @author  HalfTimeOfLife
 * @brief   Application to inject a DLL into a newly launched process.
 *
 * This application creates a target process in suspended mode,
 * injects a specified DLL into it using remote thread execution,
 * and then resumes the process.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <io.h>
#include <tchar.h>
#include <Windows.h>

#include "Helper.h"

#define SYNC_EVENT_NAME "PanoptivSyncEvent"

/**
 * @brief Injects a DLL into a target process.
 *
 * This function creates a new process in suspended mode, allocates memory
 * in the target process, writes the DLL path and event name into it, and then creates a
 * remote thread to load the DLL using LoadLibraryA.
 *
 * @param DllPath         Path to the DLL to be injected (null-terminated string).
 * @param ExecutablePath  Path to the target executable (null-terminated string).
 *
 * @return true if the DLL was successfully injected, false otherwise.
 */

bool DllInjector(char* DllPath, char* ExecutablePath) {

    STARTUPINFOA startupInfo = { 0 };
    PROCESS_INFORMATION processInfo = { 0 };
    startupInfo.cb = sizeof(STARTUPINFOA);

    // Crée l'événement de synchronisation
    HANDLE hEvent = CreateEventA(NULL, FALSE, FALSE, SYNC_EVENT_NAME);
    if (!hEvent) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not create sync event: %lu\n", GetLastError());
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Create target process in suspended mode */
    if (!CreateProcessA(ExecutablePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,
        NULL, NULL, &startupInfo, &processInfo)) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while creating target process : %lu\n", GetLastError());
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Allocate Memory in the process for the DLL path */
    LPVOID pDllPath =
        VirtualAllocEx(processInfo.hProcess, NULL, strlen(DllPath) + 1,
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDllPath) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while allocating memory in the target process : %lu\n", GetLastError());
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    if (!WriteProcessMemory(processInfo.hProcess, pDllPath, DllPath,
        strlen(DllPath) + 1, NULL)) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while writing dll path : %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    // Alloue et écrit le nom de l'événement dans le processus cible
    LPVOID pEventName = VirtualAllocEx(processInfo.hProcess, NULL, strlen(SYNC_EVENT_NAME) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pEventName) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while allocating memory for event name: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }
    if (!WriteProcessMemory(processInfo.hProcess, pEventName, SYNC_EVENT_NAME, strlen(SYNC_EVENT_NAME) + 1, NULL)) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error while writing event name: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        VirtualFreeEx(processInfo.hProcess, pEventName, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Get handle of kernel32.dll */
    HMODULE hKernel = GetModuleHandleA("kernel32.dll");
    if (!hKernel) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not get handle of kernel32.dll: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        VirtualFreeEx(processInfo.hProcess, pEventName, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Get address of LoadLibraryA */
    LPTHREAD_START_ROUTINE pLoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "LoadLibraryA");
    if (!pLoadLibraryA) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not get address of LoadLibraryA: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        VirtualFreeEx(processInfo.hProcess, pEventName, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Create remote thread to load the DLL */
    HANDLE hRemoteThread = CreateRemoteThread(processInfo.hProcess, NULL, 0, pLoadLibraryA, pDllPath, 0, NULL);
    if (!hRemoteThread) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Could not create remote thread: %lu\n", GetLastError());
        VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
        VirtualFreeEx(processInfo.hProcess, pEventName, 0, MEM_RELEASE);
        TerminateProcess(processInfo.hProcess, 1);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(hEvent);
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return false;
    }

    /* Resume the main thread of the process */
    ResumeThread(processInfo.hThread);

    // Attend que la DLL signale l'événement
    DWORD waitRes = WaitForSingleObject(hEvent, 10000); // 10 secondes max
    if (waitRes == WAIT_OBJECT_0) {
        set_console_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("[+] DLL injectée et signal reçue depuis la DLL !\n");
    } else if (waitRes == WAIT_TIMEOUT) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Timeout en attendant le signal de la DLL.\n");
    } else {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Erreur lors de l'attente du signal de la DLL: %lu\n", GetLastError());
    }
    set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    /* Wait for the remote thread to finish */
    WaitForSingleObject(hRemoteThread, INFINITE);

    /* Free the memory allocated for the DLL path and event name */
    VirtualFreeEx(processInfo.hProcess, pDllPath, 0, MEM_RELEASE);
    VirtualFreeEx(processInfo.hProcess, pEventName, 0, MEM_RELEASE);

    /* Wait for the target process to finish */
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    /* Clean up */
    CloseHandle(hRemoteThread);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    CloseHandle(hEvent);

    return true;
}

int main(void) {
    set_console_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("[+] Panoptiv application starting ...\n");

    /* Allocate buffers */
    char DllPath[512] = { 0 };
    char ExecutablePath[512] = { 0 };

    /* Get Dll Path */
    printf("[+] Enter the dll path : ");
    scanf_s("%511s", DllPath, (unsigned)_countof(DllPath));
    if (_access(DllPath, 0) == -1) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Dll path does not exist.\n");
        set_console_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("[!] Dll path : %s\n", DllPath);
        printf("[!] Please check the dll path and try again.\n");
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return EXIT_FAILURE;
    }

    /* Get Executable Path */
    set_console_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("[+] Enter the executable path : ");
    scanf_s("%511s", ExecutablePath, (unsigned)_countof(ExecutablePath));
    if (_access(ExecutablePath, 0) == -1) {
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] Error: Executable path does not exist.\n");
        set_console_color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("[!] Executable path : %s\n", ExecutablePath);
        printf("[!] Please check the executable path and try again.\n");
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return EXIT_FAILURE;
    }
    set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    return DllInjector(DllPath, ExecutablePath) ? EXIT_SUCCESS : EXIT_FAILURE;
}