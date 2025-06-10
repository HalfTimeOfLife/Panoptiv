#include <ntddk.h>


NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT driverObject) {
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("[-] Panoptiv unloaded\n");
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath) {
    UNREFERENCED_PARAMETER(registryPath);

    driverObject->DriverUnload = DriverUnload;

    DbgPrint("[+] Driver loaded\n");

    return STATUS_SUCCESS;
}
