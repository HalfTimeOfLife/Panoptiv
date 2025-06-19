/**
 * @file    PanoptivApp.c
 * @author  HalfTimeOfLife
 * @brief   Hooking based Anti-VM DLL for Windows API functions.
 *
 * This DLL demonstrates how to hook different Windows API functions
 * for countering anti-VM techniques.
 *
 */

#include <stdio.h>
#include <Windows.h>

#include "pch.h"
#include "HookedTypes.h"

#include "Helper.h"

/**
 * @brief Function used for demonstration of the hooking process.
 *
 * This function replaces the original MessageBoxA to show a custom message,
 * demonstrating how API hooking works. It temporarily restores the original
 * bytes of MessageBoxA before calling it, then displays a custom message box.
 *
 * @param hWnd       Handle to the owner window of the message box to be created.
 * @param lpText     Pointer to the text to be displayed in the message box.
 * @param lpCaption  Pointer to the text to be displayed in the title bar of the message box.
 * @param uType      Specifies the contents and behavior of the dialog box.
 *
 * @return The result of the MessageBoxA function, one of the MB_* values.
 */
int WINAPI HookedMessageBoxA(
    HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    DWORD oldProtect;
    void* pFunction = (void*)OriginalMessageBoxA;

    VirtualProtect(pFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(pFunction, originalBytes_MessageBoxA, 5);
    VirtualProtect(pFunction, 5, oldProtect, &oldProtect);

    return ((MessageBoxA_s)pFunction)(hWnd, "Hooked Message!", "Hook", uType);
}

/*
* @brief Function used for hooking the function GetForegroundWindow.
* 
* This function simulates the behavior of GetForegroundWindow by returning a handle that 
* alternates between two fake handles.
* 
* @return A chosen handle.

*/
HWND WINAPI HookedGetForegroundWindow(void)
{
    static int toggle = 0;
    HWND fakeHandle1 = (HWND)0x12345678;
    HWND fakeHandle2 = (HWND)0x87654321;

    toggle = !toggle;
    return toggle ? fakeHandle1 : fakeHandle2;
}


/*
* @brief Function used for hooking the function GetCursorPos.
*/
BOOL WINAPI HookedGetCursorPos(POINT point)
{
    DWORD oldProtect;
    void* pFunction = (void*)OriginalMessageBoxA;

    VirtualProtect(pFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(pFunction, originalBytes_MessageBoxA, 5);
    VirtualProtect(pFunction, 5, oldProtect, &oldProtect);

    static int toggle = 0;
	POINT fakePoint1 = { 100, 100 };
	POINT fakePoint2 = { 200, 200 };

    toggle = !toggle;

    return toggle ? ((GetCursorPos_s)pFunction)(&fakePoint1) : ((GetCursorPos_s)pFunction)(&fakePoint2);
}

/**
 * @brief Installs a hook on the specified function in a given module.
 *
 * This function retrieves the address of the specified function in the specified
 * module, saves the original bytes, and replaces the function's prologue with a
 * jump to the hooked function.
 *
 * @param moduleName        The name of the module containing the function to hook.
 * @param functionName      The name of the function to hook.
 * @param hookFunction      Pointer to the function that will replace the original function.
 * @param backupBytes       Pointer to a buffer where the original bytes will be saved.
 * @param originalFunction  Pointer to a variable where the address of the original function will be stored.
 */
void InstallHook(char *moduleName, char *functionName, void *hookFunction, BYTE *backupBytes, void **originalFunction)
{
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (hModule == NULL) {
        printf("[-] Failed to get module handle for %s\n", moduleName);
        return;
    }
    FARPROC pFunction = GetProcAddress(hModule, functionName);
    if (pFunction == NULL) {
        printf("[-] Failed to get function address for %s in %s\n", functionName, moduleName);
        return;
    }

    DWORD oldProtect;
    memcpy(backupBytes, pFunction, 5);

    DWORD relAddr = ((DWORD)hookFunction - (DWORD)pFunction) - 5;

    VirtualProtect(pFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    BYTE patch[5] = { 0xE9 };
    memcpy(patch + 1, &relAddr, 4);
    memcpy(pFunction, patch, 5);

    VirtualProtect(pFunction, 5, oldProtect, &oldProtect);

    if (originalFunction)
        *originalFunction = (void*)pFunction;
    set_console_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("[+] Hook installed for %s in %s\n", functionName, moduleName);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        set_console_color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("[+] DLL injected !\n");
        setvbuf(stdout, NULL, _IONBF, 0);
        //InstallHook("user32.dll", "MessageBoxA", HookedMessageBoxA, originalBytes_MessageBoxA, (void**)&OriginalMessageBoxA);
        InstallHook("user32.dll", "GetForegroundWindow", HookedGetForegroundWindow, originalBytes_GetForegroundWindow, (void**)&OriginalGetForegroundWindow);
        break;

    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        set_console_color(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("[-] DLL detached ...\n\n\n");

        set_console_color(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("========================================\n");
        printf("     Target Process possible print      \n");
        printf("========================================\n");
        set_console_color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}