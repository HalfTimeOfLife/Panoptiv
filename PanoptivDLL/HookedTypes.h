#ifndef HOOKS_H
#define HOOKS_H

#include <stdio.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <powrprof.h>
#include <psapi.h>

typedef int (WINAPI* MessageBoxA_s)(HWND, LPCSTR, LPCSTR, UINT);
MessageBoxA_s OriginalMessageBoxA = NULL;
BYTE originalBytes_MessageBoxA[5];

typedef HWND(WINAPI* GetForegroundWindow_s)();
GetForegroundWindow_s OriginalGetForegroundWindow = NULL;
BYTE originalBytes_GetForegroundWindow[5];

typedef BOOLEAN(WINAPI* GetPwrCapabilities_s)(PSYSTEM_POWER_CAPABILITIES);
GetPwrCapabilities_s OriginalGetPwrCapabilities = NULL;

typedef BOOL(WINAPI* GetDiskFreeSpaceExA_s)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
GetDiskFreeSpaceExA_s OriginalGetDiskFreeSpaceExA = NULL;

typedef DWORD(WINAPI* GetAdaptersInfo_s)(PIP_ADAPTER_INFO, PULONG);
GetAdaptersInfo_s OriginalGetAdaptersInfo = NULL;

typedef BOOL(WINAPI* GlobalMemoryStatusEx_s)(LPMEMORYSTATUSEX);
GlobalMemoryStatusEx_s OriginalGlobalMemoryStatusEx = NULL;

typedef BOOL(WINAPI* GetCursorPos_s)(LPPOINT);
GetCursorPos_s OriginalGetCursorPos = NULL;

typedef void (WINAPI* GetSystemInfo_s)(LPSYSTEM_INFO);
GetSystemInfo_s OriginalGetSystemInfo = NULL;

typedef LSTATUS(WINAPI* RegQueryValueExA_s)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
RegQueryValueExA_s OriginalRegQueryValueExA = NULL;

typedef int (WINAPI* GetSystemMetrics_s)(int);
GetSystemMetrics_s OriginalGetSystemMetrics = NULL;

typedef BOOL(WINAPI* EnumServicesStatus_s)(
    SC_HANDLE hSCManager,
    DWORD dwServiceType,
    DWORD dwServiceState,
    LPENUM_SERVICE_STATUS lpServices,
    DWORD cbBufSize,
    LPDWORD pcbBytesNeeded,
    LPDWORD lpServicesReturned,
    LPDWORD lpResumeHandle
    );
EnumServicesStatus_s OriginalEnumServicesStatus = NULL;

#endif /* HOOKS_H */