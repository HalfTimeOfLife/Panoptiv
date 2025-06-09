#include <ntddk.h>

NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT driverObject) {
    UNREFERENCED_PARAMETER(driverObject);

    KdPrint(("%wZ : Unloaded!\n", driverObject->DriverName));
    return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath) {
    UNREFERENCED_PARAMETER(registryPath);

    driverObject->DriverUnload = DriverUnload;

    KdPrint(("%wZ : Loaded!\n", driverObject->DriverName));

    
    return STATUS_SUCCESS;
}