# Panoptiv

<div style="text-align: center;">
  <img src="panoptiv.webp" alt="Panoptiv" width="35%" />
</div>

A Windows Kernel Driver that bypass anti vm techniques using Windows API functions and assembly instruction.

---

This project is a Windows kernel driver that can be added to a virtual machine (sandbox) to counter anti vm techniques using function from the Windows API as well as some assembly instruction. The Windows API functions countered are the following :

- `GetForegroundWindow`
- `GetAdaptersInfo`
- `GlobalMemoryStatusEx`
- `GetCursorPos`
- `GetSystemInfo`
- `GetDiskFreeSpaceExA`
- `EnumServicesStatus`
- `GetPwrCapabilities`

For the assembly instruction :

- `cpuid`
- `sidt`
- `rdtsc`

## Driver installation

As expected, this driver isn't signed by Microsoft ( maybe one day )