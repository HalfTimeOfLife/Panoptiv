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
* 
* This function modifies the cursor position by adding a small random offset to the last known position (real or fake).
* 
* @param lpPoint Pointer to a POINT structure that receives the screen coordinates of the cursor.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/
BOOL WINAPI HookedGetCursorPos(LPPOINT lpPoint)
{
    if (!lpPoint) return FALSE;

    lastFakePoint.x += (generate_random_number() % 3) - 1;
    lastFakePoint.y += (generate_random_number() % 3) + 1;

    lpPoint->x = lastFakePoint.x;
    lpPoint->y = lastFakePoint.y;

    return TRUE;
}

/*
* @brief Function used for hooking the function GetPwrCapabilities.
* 
* This function modifies the system power capabilities to always indicate that all power states are available 
* and that the thermal control capacity is enable.
* 
* @param lpspc Pointer to a SYSTEM_POWER_CAPABILITIES structure that receives the power capabilities.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/
BOOL WINAPI HookedGetPwrCapabilities(PSYSTEM_POWER_CAPABILITIES lpspc) {
	if (!lpspc) return FALSE;

	lpspc->SystemS1 = TRUE; 
    lpspc->SystemS2 = TRUE;
    lpspc->SystemS3 = TRUE;
    lpspc->SystemS4 = TRUE;
    lpspc->SystemS5 = TRUE;
	lpspc->ThermalControl = TRUE;

	return TRUE;
}


/*
* @brief Function used for hooking the function GetDiskFreeSpaceExA.
* 
* This modifies the disk space information to always return a fixed amount of free and total bytes,
* 
* @param lpDirectoryName                Pointer to a string that specifies the directory on the disk.
* @param lpFreeBytesAvailableToCaller   Pointer to a variable that receives the number of free bytes available to the caller.
* @param lpTotalNumberOfBytes           Pointer to a variable that receives the total number of bytes on the disk.
* @param lpTotalNumberOfFreeBytes       Pointer to a variable that receives the total number of free bytes on the disk.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/
BOOL WINAPI HookedGetDiskFreeSpaceExA(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes) {

	if (!lpDirectoryName || !lpFreeBytesAvailableToCaller || !lpTotalNumberOfBytes || !lpTotalNumberOfFreeBytes) return FALSE;

    lpTotalNumberOfBytes->QuadPart = 1000000000000;
    lpTotalNumberOfFreeBytes->QuadPart = 500000000000;

    return TRUE;
}

/*
* @brief Function used for hooking the function GetSystemInfo.
* 
* This function modifies the system information to always report a fixed number of processors equal to 16.
*
* @param lpSystemInfo Pointer to a SYSTEM_INFO structure that receives the system information.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/
BOOL WINAPI HookedGetSystemInfo(LPSYSTEM_INFO lpSystemInfo) {
	if (!lpSystemInfo) return FALSE;

    lpSystemInfo->dwNumberOfProcessors = 16;

    return TRUE;
}

/*
* @brief Function used for hooking the function EnumServicesStatus.
* 
* This function modifies the service names returned by EnumServicesStatus to always return "bidule" for each service.
* 
* @param hSCManager         Handle to the service control manager database.
* @param dwServiceType      Type of services to be enumerated. This can be a combination of SERVICE_* flags.
* @param dwServiceState     State of services to be enumerated. This can be a combination of SERVICE_STATE_* flags.
* @param lpServices         Pointer to an array of ENUM_SERVICE_STATUS structures that receives the status information for the services.
* @param cbBufSize          Size of the buffer pointed to by lpServices, in bytes.
* @param pcbBytesNeeded     Pointer to a variable that receives the number of bytes needed to store all service status information.
* @param lpServicesReturned Pointer to a variable that receives the number of services returned in the lpServices buffer.
* @param lpResumeHandle     Pointer to a variable that receives a resume handle for the next call to EnumServicesStatus.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/

BOOL WINAPI HookedEnumServicesStatus(SC_HANDLE hSCManager, DWORD dwServiceType, DWORD dwServiceState, LPENUM_SERVICE_STATUS lpServices, DWORD cbBufSize, LPDWORD pcbBytesNeeded, LPDWORD lpServicesReturned, LPDWORD lpResumeHandle) {
	if (!hSCManager || !lpServices || !pcbBytesNeeded || !lpServicesReturned || !lpResumeHandle) return FALSE;

	if (!OriginalEnumServicesStatus) return FALSE;

    ENUM_SERVICE_STATUSA fakeServiceName[NUMBER_OF_SERVICES];

	SERVICE_STATUS customServiceStatus = { SERVICE_WIN32_OWN_PROCESS, SERVICE_RUNNING, SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN, NO_ERROR, 0, 0, 0};

    int i;
    for(i = 0; i < NUMBER_OF_SERVICES; i++) {
        fakeServiceName[i].lpServiceName = "bidule";
        fakeServiceName[i].lpDisplayName = "bidule";
		fakeServiceName[i].ServiceStatus = customServiceStatus;
	}

    lpServices = fakeServiceName;

    return TRUE;
}


/*
* @brief Function used for hooking the function GetAdaptersInfo.
* 
* This function modifies the MAC addresses of network adapters to a fixed value.
* 
* @param pAdapterInfo Pointer to a buffer that receives the adapter information.
* @param pOutBufLen   Pointer to a variable that specifies the size of the buffer pointed to by pAdapterInfo, and receives the size of the data copied to the buffer.
* 
* @return TRUE if the function succeeds, FALSE otherwise.
*/
DWORD WINAPI HookedGetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen) {
    if (!pAdapterInfo || !pOutBufLen) return ERROR_NOT_SUPPORTED;
    if (!OriginalGetAdaptersInfo) return ERROR_NOT_SUPPORTED;

    while (pAdapterInfo) {
        pAdapterInfo->Address[0] = 0x12;
        pAdapterInfo->Address[1] = 0x34;
        pAdapterInfo->Address[2] = 0x56;
        pAdapterInfo->Address[3] = 0x78;
        pAdapterInfo->Address[4] = 0x91;
        pAdapterInfo->Address[5] = 0x00;
        pAdapterInfo = pAdapterInfo->Next;
    }

    return ERROR_SUCCESS;
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
        /* InstallHook("user32.dll", "MessageBoxA", HookedMessageBoxA, originalBytes_MessageBoxA, (void**)&OriginalMessageBoxA); */
        InstallHook("user32.dll", "GetForegroundWindow", HookedGetForegroundWindow, originalBytes_GetForegroundWindow, (void**)&OriginalGetForegroundWindow);
		InstallHook("user32.dll", "GetCursorPos", HookedGetCursorPos, originalBytes_GetCursorPos, (void**)&OriginalGetCursorPos);
		InstallHook("powrprof.dll", "GetPwrCapabilities", HookedGetPwrCapabilities, originalBytes_GetPwrCapabilities, (void**)&OriginalGetPwrCapabilities);
		InstallHook("kernel32.dll", "GetDiskFreeSpaceExA", HookedGetDiskFreeSpaceExA, originalBytes_GetDiskFreeSpaceExA, (void**)&OriginalGetDiskFreeSpaceExA);
        InstallHook("kernel32.dll", "GetSystemInfo", HookedGetSystemInfo, originalBytes_GetSystemInfo, (void**)&OriginalGetSystemInfo);
		InstallHook("advapi32.dll", "EnumServicesStatus", HookedEnumServicesStatus, originalBytes_EnumServicesStatus, (void**)&OriginalEnumServicesStatus);
		InstallHook("iphlpapi.dll", "GetAdaptersInfo", HookedGetAdaptersInfo, originalBytes_GetAdaptersInfo, (void**)&OriginalGetAdaptersInfo);
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