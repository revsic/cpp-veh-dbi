# cpp-veh-dbi
C++ Implementation of VEH based windows dynamic binary instrumentation

- Copyright (c) 2019 YoungJoong Kim, cpp-veh-dbi is licensed under the [MIT license](./LICENSE).
- This repository aims to write simple and fast programmable DBI tool.

## Environments
Tested environments
- Windows 10
- Visual Studio 2019
- CMake 3.14.0

## Usage
To build library and samples,
```
.\script\setup
.\script\build
```
Then executables can be found in the `bin` directory.
```
ls bin
```
To run sample branch tracer for notepad,
```
.\bin\dllinjector C:\Windows\notepad.exe .\bin\branchtracer.dll
```
Then console will log the Windows API call.
```
+00007FF66FF9AC85,00007FFCEF55CD30,KERNEL32.DLL,GetStartupInfoW
+00007FF66FF9AD18,00007FFCF038FC10,ntdll.dll,RtlRetrieveNtUserPfn
+00007FF66FF9AD18,00007FFCF038FC10,ntdll.dll,RtlRetrieveNtUserPfn
+00007FF66FF9AD5A,00007FFCEEE6A6A0,msvcrt.dll,initterm
+00007FF66FF9AE4A,00007FFCEEE513B0,msvcrt.dll,ismbblead
+00007FF66FF9AE4A,00007FFCEEE513B0,msvcrt.dll,ismbblead
```

## Structure
Struct `VehDBI` has three ways to instrument binary.
1. Handler : Trigger on specified address.
2. Tracer : Tracing instruction with debugging event.
3. BTCallback : Callback on every instruction in text section.

Sample handler which triggered on entrypoint.
```c++
struct EntrypointHandler : Handler {
    void Handle(PCONTEXT context) override {
        std::ofstream("CONOUT$") << "trigged on entrypoint" << std::endl;
    }
};

// create dbi
VehDBI dbi;
// handler sample
size_t entrypoint = Utils::GetEntryPointAddress();
dbi.AddHandler(entrypoint, std::make_unique<EntrypointHandler>());
```