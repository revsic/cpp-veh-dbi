#include <Windows.h>

#include <vehdbi.hpp>

VehDBI CreateDBI() {
    // allocate console
    AllocConsole();
    // create dbi
    VehDBI dbi;
    dbi.AddBTCallback(std::make_unique<Logger>("CONOUT$"));
    return std::move(dbi);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static VehDBI dbi = CreateDBI();

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        VehDBI::Run(std::move(dbi));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
