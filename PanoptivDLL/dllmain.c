#include "pch.h"
#include <stdio.h>
#include <Windows.h>

static int (WINAPI* OriginalMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = MessageBoxA;

int WINAPI HookedMessageBoxA(
    HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    printf("[!] Content of hooked message :\n");
    printf("\t - lpText : %s\n", lpText);
    printf("\t - lpCaption : %s\n", lpCaption);
    printf("\t - lpText : %ld\n", uType);
    return MessageBoxA(hWnd, "Hooked Message!", "Hook", uType);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        printf("[+] DLL injected !\n");

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        printf("[-] DLL detached ...\n");
        break;
    }
    return TRUE;
}

