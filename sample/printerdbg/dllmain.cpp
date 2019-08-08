#include <Windows.h>

#include <branch_tracer.hpp>
#include <debugger.hpp>

#include "custom_handler.hpp"

Debugger CreateDebugger() {
    // create debugger
    Debugger dbg;
    dbg.AddHandler(0x4063DF, std::make_unique<InModifyDoubleHandler>());
    dbg.AddHandler(0x5C648A, std::make_unique<MenuClickHandler>());
    dbg.AddHandler(0x5C2DEC, std::make_unique<BtnOrderCancelClickHandler>());
    dbg.AddHandler(0x5BFDB0, std::make_unique<BtnSaleClickHandler>());
    dbg.AddHandler(0x5D6804, std::make_unique<BtnAccountClickHandler>());
    dbg.AddHandler(0x5D16B8, std::make_unique<BtnAccountCancelClickHandler>());
    dbg.AddHandler(0x5CE464, std::make_unique<BtnCashClickHandler>());
    dbg.AddHandler(0x5CFC20, std::make_unique<BtnCreditCardClickHandler>());
    dbg.AddHandler(0x4833E8, std::make_unique<WritePrinterHook>());

    dbg.AddTracer(0x5D6804, 0x5D809E, std::make_unique<BranchTracer>("./test2.txt", 0x401720, 0x8D1400));
    dbg.AddTracer(0x5E6898, 0x5E6C7B, std::make_unique<BranchTracer>("./receipt_total.txt", 0x401720, 0x8D1400));

    return std::move(dbg);
};

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
