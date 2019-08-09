#include <Windows.h>

#include <vehdbi.hpp>

DBI CreateDBI() {
    // allocate console
    AllocConsole();
    // create debugger
    DBI dbi;
    dbi.AddBTCallback(std::make_unique<Logger>("CONOUT$"));
    return std::move(dbi);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static DBI dbi = CreateDBI();

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DBI::Run(std::move(dbi));
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
