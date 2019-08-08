# cpp-veh-dbi
C++ Implementation of VEH based windows dynamic binary instrumentation

- Copyright (c) 2019 YoungJoong Kim, cpp-veh-debugger is licensed under the [MIT license](./LICENSE).
- This repository aims to write programmable VEH based DBI tool.

## Environments
Tested environments
- Windows 10
- Visual Studio 2019

## Usage
To build library and samples, run [setup.ps1](./script/setup.ps1) and [build.ps1](./script/build.ps1).
```
.\script\setup
.\script\build
```
Then executables are copied at `bin` directory.
```
ls bin
```
Run sample branch tracer on notepad.
```
.\bin\dllinjector C:\Windows\notepad.exe .\bin\branchtracer.dll
```
Then you can see the console that log the Windows API call.
```
+00007FF66FF9AC85,00007FFCEF55CD30,KERNEL32.DLL,GetStartupInfoW
+00007FF66FF9AD18,00007FFCF038FC10,ntdll.dll,RtlRetrieveNtUserPfn
+00007FF66FF9AD18,00007FFCF038FC10,ntdll.dll,RtlRetrieveNtUserPfn
+00007FF66FF9AD5A,00007FFCEEE6A6A0,msvcrt.dll,initterm
+00007FF66FF9AE4A,00007FFCEEE513B0,msvcrt.dll,ismbblead
+00007FF66FF9AE4A,00007FFCEEE513B0,msvcrt.dll,ismbblead
```
