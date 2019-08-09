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
1. [Handler](#1-handler) : Trigger on specified address.
2. [Tracer](#2-tracer) : Tracing instruction with debugging event.
3. [BTCallback](#3-btcallback) : Callback on every instruction in text section.

### 1. Handler
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

`VehDBI::AddHandler` get two arguments, address(`size_t`) and handler (`std::unique_ptr<Handler>`).

`Handler` require only one method, which will be invoked in specified address.
```c++
// Interface for debug event handler.
struct Handler {
    // Default virtual destructor.
    virtual ~Handler() = default;
    // Handle debug event.
    virtual void Handle(PCONTEXT context) = 0;
};
```

### 2. Tracer
Sample tracer which tracing branch instruction, [branch_tracer](./lib/src/branch_tracer.cpp).

`Tracer` require two methods.
- HandleSingleStep: Handle single step exception.
- HandleBreakpoint: Handle breakpoint exception.

If tracer set software bp, `HandleBreakpoint` should recover the opcode.
```c++
// Interface for code trace handler.
struct Tracer {
    // Default virtual destructor.
    virtual ~Tracer() = default;
    // Handle single step exception.
    virtual void HandleSingleStep(PCONTEXT context, Utils::SoftwareBP& bp) = 0;
    // Handle software breakpoint exception.
    virtual void HandleBreakpoint(PCONTEXT context, Utils::SoftwareBP& bp) = 0;
};
```

`VehDBI::AddTracer` get three arguments, tracer start point(`size_t`), end point(`size_t`) and tracer(`std::unique_ptr<Tracer>`).
- If start point is 0, dbi automatically start tracer on entrypoint.
- If end point is 0, tracer doesn't stop until process termination.
```c++
dbi.AddTracer(0, 0, std::make_unique<BranchTracer>());
```

### 3. BTCallback
Indeed, BTCallback is callback for branch tracer, which call `BTCallback::run` at every instruction. 

VehDBI basically run branch tracer on text section. And `VehDBI::AddBTCallback` add given callback to the default branch tracer. Then added callback will be invoked on every instruction in text section.

Sample BTCallback which log branch instruction, [logger](./lib/src/logger.cpp).
```c++
// btcallback sample
auto logger = std::make_unique<Logger>("CONOUT$");
// tracer sample
dbi.AddTracer(0, 0, std::make_unique<BranchTracer>(std::move(logger)));
```

Which is same with `VehDBI::AddBTCallback`, if tracer (start, end) point is (0, 0).
```c++
dbi.AddBTCallback(std::make_unique<Logger>("CONOUT$"));
```

`BTCallback` require only one method, which will be invoked on every instruction.
```c++
// Callback for branch tracer.
struct BTCallback {
    // Default destructor.
    virtual ~BTCallback() = default;
    // Callback.
    virtual void run(BTInfo const& info, PCONTEXT context) = 0;
};
```