# Panoptiv

<p align="center">
  <img src="panoptiv.webp" alt="Panoptiv" width="35%" />
</p>

*A Windows Kernel Driver and a DLL that bypass anti vm techniques using Windows API functions and assembly instruction.*

---

This project is a Windows kernel driver that can be added to a virtual machine (sandbox) to counter anti vm techniques using function from the Windows API as well as some assembly instruction. 

The DLL is installed by the driver, it's goal will only be to counter anti vm techniques using functions from the Windows API while the driver itself will take care of the techniques based on assembly instruction.

 The Windows API functions countered are the following :

- `GetForegroundWindow`
- `GetAdaptersInfo`
- `GlobalMemoryStatusEx`
- `GetCursorPos`
- `GetSystemInfo`
- `GetDiskFreeSpaceExA`
- `EnumServicesStatus`
- `GetPwrCapabilities`
- `RegOpenKeyExA`

For the assembly instruction :

- `cpuid`
- `sidt`
- `rdtsc`

## Documentation

### What is in this project ?

### How to use it ?

## Details

As expected, this driver isn't signed by Microsoft so it can only be used in a Windows system where the signed verification is disabled (aka, only test system). You also need to set you windows in Test Mode in order to install all the driver.

## TODO

- Finish the readme.md
- Add the driver code for patching 
