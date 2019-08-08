#include <Windows.h>

#include <branch_tracer.hpp>
#include <debugger.hpp>

Debugger CreateDebugger() {
    // allocate console
    AllocConsole();
    // create debugger
    Debugger dbg;
    dbg.AddTracer(0, 0, std::make_unique<BranchTracer>("CONOUT$", false));
    return std::move(dbg);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static Debugger dbg = CreateDebugger();

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Debugger::Run(std::move(dbg));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
