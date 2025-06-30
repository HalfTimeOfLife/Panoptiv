# Panoptiv

<p align="center">
  <img src="panoptiv.webp" alt="Panoptiv" width="35%" />
</p>

*A Windows Kernel Driver and a DLL that bypass anti vm techniques using Windows API functions and assembly instruction.*

---

This project is a Windows kernel driver that can be added to a virtual machine (sandbox) to counter anti vm techniques using function from the Windows API as well as some assembly instruction. 

The final objective (if possible) is the following :

> The DLL is installed by the driver, it's goal will only be to counter anti vm techniques using functions from the Windows API while the driver itself will take care of the techniques based on assembly instruction.

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

The goal of the project is to have a functionnal DLL, injector and Windows driver. You can find in this repository 3 directories which all contains the code for their respective element of the project :

- [PanoptivAPP](PanoptivApp/) : Directory containing the code for the DLL injector.
- [PanoptivDLL](PanoptivDLL/) : Directory containing the code for the DLL.
- [PanoptivDriver](PanoptivDriver/) : Directory containing the code for the Driver.

### How to use it ?

There should be the latest compiled version for each of the element from this project. But you can recompile the project using Visual Studio.
## Details

As expected, this driver isn't signed by Microsoft so it can only be used in a Windows system where the signed verification is disabled (aka, only test system). You also need to set you windows in Test Mode in order to install all the driver.

> The following article describe how to accomplish that : [What Is Test Mode? How to Enable or Disable It in Windows 10/11?](https://www.minitool.com/news/windows-10-test-mode.html)
## TODO

- Finish the readme.md
- Add a helper command for the injector
- Add the documentation for each function (docstring already implemented)
- Add the driver code
