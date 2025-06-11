#include <stdio.h>
#include <stdbool.h>
#include <io.h>
#include <tchar.h>
#include <Windows.h>

bool DllInjector(char* DllPath, char* ExecutablePath) {

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