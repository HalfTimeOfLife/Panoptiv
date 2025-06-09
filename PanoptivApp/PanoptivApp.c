#include <stdio.h>
#include <tchar.h>
#include <wchar.h>
#include <Windows.h>

int main() {

	printf("[+] Panoptiv application loaded!\n");

	HMODULE hDll = LoadLibraryW(_T("PanoptivDLL.dll"));

	if (!hDll) {
		printf("[-] Error while loading library: error %lu\n", GetLastError());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}