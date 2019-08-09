#include <Windows.h>

#include <vehdbi.hpp>

struct EntrypointHandler : Handler {
    void Handle(PCONTEXT context) override {
        std::ofstream("CONOUT$") << "trigged on entrypoint" << std::endl;
    }
};

VehDBI CreateDBI() {
    AllocConsole();
    // create dbi
    VehDBI dbi;
    // handler sample
    size_t entrypoint = Utils::GetEntryPointAddress();
    dbi.AddHandler(entrypoint, std::make_unique<EntrypointHandler>());
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
