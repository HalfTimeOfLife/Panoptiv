#include <ntddk.h>

#define DLL_PATH L"PanoptivDLL.dll"
#define TARGET_EXE_NAME L"dummy.exe"

VOID ProcessDetectionRoutine(PEPROCESS Process, HANDLE hProcess, PPS_CREATE_NOTIFY_INFO ProcessInformation)
{
    UNREFERENCED_PARAMETER(Process);
    UNREFERENCED_PARAMETER(hProcess);


    if (ProcessInformation && ProcessInformation->ImageFileName) {
        /* Test wether the newly created process is our target */
        if (wcsstr(ProcessInformation->ImageFileName->Buffer, TARGET_EXE_NAME)) {
            DbgPrint("[+] Detected process: %wZ\n", ProcessInformation->ImageFileName);
            /* INJECT DLL */
		}
    }
}

NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT driverObject) {
    UNREFERENCED_PARAMETER(driverObject);

    NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(ProcessDetectionRoutine, TRUE);
    if (!NT_SUCCESS(status))
    {
        DbgPrint("[!] Failed to remove injection routine: 0x%X\n", status);
        return status;
    }

    DbgPrint("[-] Panoptiv unloaded\n");
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath) {
    UNREFERENCED_PARAMETER(registryPath);
    driverObject->DriverUnload = DriverUnload;

    

    NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(ProcessDetectionRoutine, FALSE);
    if (!NT_SUCCESS(status))
    {
		DbgPrint("[!] Failed to set injection routine: 0x%X\n", status);
        return status;
    }

    DbgPrint("[+] Panoptiv loaded\n");
    return STATUS_SUCCESS;
}
